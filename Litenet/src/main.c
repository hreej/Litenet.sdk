#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xlitenet.h"
#include "xil_io.h"

// ============================================================================
// [重要] 输入源选择开关
// 1: 使用 SD 卡中的 .bin 文件 (image.bin)
// 0: 使用 input_image_packed.h 头文件中的数组
// ============================================================================
#define USE_SD_CARD     0

// ============================================================================
// 通用配置参数
// ============================================================================
#define INPUT_LEN       12288        // 输入数据长度 (u32 个数)
#define NUM_CLASSES     12           // 分类数量

// ============================================================================
// 根据选择引入不同的头文件和变量
// ============================================================================
#if USE_SD_CARD
    #include "ff.h"  // 引入 FatFs 文件系统头文件
    #define INPUT_FILE_NAME "image.bin"  // SD卡中的文件名
    
    static FATFS fs;               // 文件系统对象
    static FIL fil;                // 文件对象
    // 定义缓冲区来存放从SD卡读取的数据 (放在全局区以防止栈溢出)
    static u32 input_buffer[INPUT_LEN]; 
#else
    // 引入头文件数据 (请确保 input_image_packed.h 中的数组类型为 u32)
    #include "input_image_packed.h"
#endif

// ============================================================================
// 类别名称定义
// ============================================================================
const char *class_names[NUM_CLASSES] = {
    "cotton_aphids",          // Class 0
    "cotton_army_worm",       // Class 1
    "cotton_bacterialblight", // Class 2
    "cotton_healthy",         // Class 3
    "cotton_powdery_mildew",  // Class 4
    "cotton_target_spot",     // Class 5
    "pepper_bacterial_spot",  // Class 6
    "pepper_healthy",         // Class 7
    "rice_bacterialblight",   // Class 8
    "rice_blast",             // Class 9
    "rice_brownspot",         // Class 10
    "rice_tungro"             // Class 11
};

// ============================================================================
// SD卡 辅助函数 (仅在启用 SD 卡模式时编译)
// ============================================================================
#if USE_SD_CARD
int sd_mount() {
    FRESULT status;
    // 注册工作区 (Drive 0)
    status = f_mount(&fs, "", 1); 
    if (status != FR_OK) {
        xil_printf("ERROR: Failed to mount SD card. Status: %d\n\r", status);
        return XST_FAILURE;
    }
    xil_printf("SD Card mounted successfully.\n\r");
    return XST_SUCCESS;
}

int read_input_from_sd(const char *filename, u32 *destination, u32 length_u32) {
    FRESULT status;
    UINT bytes_read;
    u32 total_bytes = length_u32 * sizeof(u32);

    xil_printf("Opening file: %s\n\r", filename);
    status = f_open(&fil, filename, FA_READ);
    if (status != FR_OK) {
        xil_printf("ERROR: Could not open file %s. Status: %d\n\r", filename, status);
        return XST_FAILURE;
    }

    // 移动指针到开头
    f_lseek(&fil, 0);

    xil_printf("Reading %d bytes from SD card...\n\r", total_bytes);
    status = f_read(&fil, (void*)destination, total_bytes, &bytes_read);
    if (status != FR_OK) {
        xil_printf("ERROR: File read failed. Status: %d\n\r", status);
        f_close(&fil);
        return XST_FAILURE;
    }

    if (bytes_read != total_bytes) {
        xil_printf("WARNING: Expected %d bytes, but read %d bytes.\n\r", total_bytes, bytes_read);
    }

    f_close(&fil);
    xil_printf("File read closed successfully.\n\r");
    return XST_SUCCESS;
}
#endif

// ============================================================================
// 主函数
// ============================================================================
int main()
{
    int status;
    int i;
    u32 result_packed;
    
    // 存放结果变量
    int8_t result_unpacked[NUM_CLASSES]; 
    float probs[NUM_CLASSES];
    float max_val, sum_exp;
    int predicted_class = -1;
    float max_prob = -1.0f;
    float classifier_scale = 0.43142056465148926;// 量化比例因子,可以用来调节softmax的平滑度改变输出的置信度

    // 1. 初始化平台
    init_platform();
    xil_printf("\n--- Litenet Application Start ---\n\r");

    // 2. 准备输入数据 (根据 USE_SD_CARD 选择)
#if USE_SD_CARD
    xil_printf(">> Mode: SD Card Input\n\r");
    
    // 挂载 SD 卡
    status = sd_mount();
    if (status != XST_SUCCESS) {
        xil_printf("System halted due to SD mount error.\n\r");
        return -1;
    }

    // 读取数据
    status = read_input_from_sd(INPUT_FILE_NAME, input_buffer, INPUT_LEN);
    if (status != XST_SUCCESS) {
        xil_printf("System halted due to file read error.\n\r");
        return -1;
    }
#else
    xil_printf(">> Mode: Header File Input (input_image_packed.h)\n\r");
#endif

    // 3. 初始化 LiteNet 加速器
    XLitenet HlsLitenet;
    XLitenet_Config *LitenetPtr;

    xil_printf("Initializing LiteNet Device...\n\r");
    LitenetPtr = XLitenet_LookupConfig(XPAR_XLITENET_0_DEVICE_ID);
    if (!LitenetPtr) {
        xil_printf("ERROR: Lookup of LiteNet configuration failed.\n\r");
        return XST_FAILURE;
    }

    status = XLitenet_CfgInitialize(&HlsLitenet, LitenetPtr);
    if (status != XST_SUCCESS) {
        xil_printf("ERROR: Could not initialize LiteNet.\n\r");
        return XST_FAILURE;
    }

    // 4. 将输入数据写入 BRAM (Input BRAM)
    xil_printf("Transferring input data to BRAM...\n\r");
    for(i = 0; i < INPUT_LEN; i++)
    {
#if USE_SD_CARD
        // 从 SD 卡读取的缓冲区写入
        Xil_Out32(XPAR_AXI_BRAM_CTRL_IN_S_AXI_BASEADDR + (i * 4), input_buffer[i]);
#else
        // 从头文件数组写入 (请确保数组名为 input_packed_arr)
        Xil_Out32(XPAR_AXI_BRAM_CTRL_IN_S_AXI_BASEADDR + (i * 4), (u32)input_packed_arr[i]);
#endif
    }

    // 5. 启动加速器
    xil_printf("Starting LiteNet Accelerator...\n\r");
    XLitenet_Start(&HlsLitenet);

    // 6. 等待计算完成
    while (!XLitenet_IsDone(&HlsLitenet));
    xil_printf("Inference Done!\n\r");

    // 7. 从 BRAM 读取结果并解包
    for(i = 0; i < 3; i++)
    {
        // 读取 32 位数据
        result_packed = Xil_In32(XPAR_AXI_BRAM_CTRL_OUT_S_AXI_BASEADDR + (i * 4));
        
        // 解包：将 u32 拆分为 4 个 int8
        result_unpacked[i*4 + 0] = (int8_t)(result_packed & 0xFF);
        result_unpacked[i*4 + 1] = (int8_t)((result_packed >> 8) & 0xFF);
        result_unpacked[i*4 + 2] = (int8_t)((result_packed >> 16) & 0xFF);
        result_unpacked[i*4 + 3] = (int8_t)((result_packed >> 24) & 0xFF);
    }

    // 8. 计算 Softmax 概率并打印
    // 第一步：找到最大值
    max_val = result_unpacked[0];
    for(i = 1; i < NUM_CLASSES; i++) {
        if(result_unpacked[i] > max_val) {
            max_val = result_unpacked[i];
        }
    }

    // 第二步：计算指数和
    sum_exp = 0.0f;
    for(i = 0; i < NUM_CLASSES; i++) {
        probs[i] = expf(((float)result_unpacked[i] - max_val)*classifier_scale);
        sum_exp += probs[i];
    }

    // 第三步：归一化并打印结果
    printf("\n================ Classification Results ================\n");
    printf(" ID | Score | Probability | Class Name\n");
    printf("----|-------|-------------|-----------------------------\n");

    for(i = 0; i < NUM_CLASSES; i++) {
        probs[i] = probs[i] / sum_exp; 

        printf(" %2d |  %4d |    %5.2f%%   | %-25s\n", 
                i, 
                result_unpacked[i], 
                probs[i] * 100.0f, 
                class_names[i]);

        if(probs[i] > max_prob) {
            max_prob = probs[i];
            predicted_class = i;
        }
    }

    printf("========================================================\n");
    if(predicted_class != -1) {
        printf("Prediction: [%s] with %.2f%% confidence.\n", 
                class_names[predicted_class], max_prob * 100.0f);
    }

    xil_printf("--- All operations completed ---\n\r");
    cleanup_platform();
    return 0;
}
