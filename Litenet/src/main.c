#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>       // 用于 expf() 函数
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xlitenet.h"
#include "xil_io.h"

// 引入输入数据
// 提示：请确保 input_image_packed.h 中的数组类型已改为 u32 或 unsigned int
#include "input_image_packed.h"

// 定义分类数量
#define NUM_CLASSES 12

// ============================================================================
// 类别名称定义 (根据您的要求更新)
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

int main()
{
    int status;
    int i;
    u32 result_packed;
    
    // 存放解包后的原始分数 (int8)
    int8_t result_unpacked[NUM_CLASSES]; 
    
    // 存放计算后的概率 (float)
    float probs[NUM_CLASSES];
    float max_val, sum_exp;
    int predicted_class = -1;
    float max_prob = -1.0f;

    // 1. 初始化平台
    init_platform();
    xil_printf("\n--- Litenet Application Start ---\n\r");

    // 2. 初始化 LiteNet 加速器
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

    // 3. 将输入图片数据写入 BRAM
    xil_printf("Writing input image to BRAM...\n\r");
    for(i = 0; i < 12288; i++)
    {
        Xil_Out32(XPAR_AXI_BRAM_CTRL_IN_S_AXI_BASEADDR + (i * 4), (u32)input_packed_arr[i]);
    }

    // 4. 启动加速器
    xil_printf("Starting LiteNet Accelerator...\n\r");
    XLitenet_Start(&HlsLitenet);

    // 5. 等待计算完成
    while (!XLitenet_IsDone(&HlsLitenet));
    xil_printf("Inference Done!\n\r");

    // 6. 从 BRAM 读取结果并解包
    // 输出是 3 个 u32，包含 12 个 int8
    for(i = 0; i < 3; i++)
    {
        // 读取 32 位数据
        result_packed = Xil_In32(XPAR_AXI_BRAM_CTRL_OUT_S_AXI_BASEADDR + (i * 4));
        
        // 解包：将 u32 拆分为 4 个 int8
        // 注意：根据 HLS 小端模式，低位是第一个数据
        result_unpacked[i*4 + 0] = (int8_t)(result_packed & 0xFF);
        result_unpacked[i*4 + 1] = (int8_t)((result_packed >> 8) & 0xFF);
        result_unpacked[i*4 + 2] = (int8_t)((result_packed >> 16) & 0xFF);
        result_unpacked[i*4 + 3] = (int8_t)((result_packed >> 24) & 0xFF);
    }

    // ---------------------------------------------------------
    // 7. 计算 Softmax 概率并打印
    // ---------------------------------------------------------
    
    // 第一步：找到最大值（为了数值稳定性，防止 exp 溢出）
    max_val = result_unpacked[0];
    for(i = 1; i < NUM_CLASSES; i++) {
        if(result_unpacked[i] > max_val) {
            max_val = result_unpacked[i];
        }
    }

    // 第二步：计算指数和
    sum_exp = 0.0f;
    for(i = 0; i < NUM_CLASSES; i++) {
        // exp(x - max)
        probs[i] = expf((float)result_unpacked[i] - max_val);
        sum_exp += probs[i];
    }

    // 第三步：归一化并打印结果
    printf("\n================ Classification Results ================\n");
    printf(" ID | Score | Probability | Class Name\n");
    printf("----|-------|-------------|-----------------------------\n");

    for(i = 0; i < NUM_CLASSES; i++) {
        probs[i] = probs[i] / sum_exp; // 归一化为 0.0 - 1.0

        // 打印格式：ID | 原始分 | 概率百分比 | 类别名
        // %-25s 表示左对齐字符串，占用25个字符宽度
        printf(" %2d |  %4d |    %5.2f%%   | %-25s\n", 
                i, 
                result_unpacked[i], 
                probs[i] * 100.0f, 
                class_names[i]);

        // 记录概率最大的类别
        if(probs[i] > max_prob) {
            max_prob = probs[i];
            predicted_class = i;
        }
    }

    printf("========================================================\n");
    printf("Prediction: [%s] with %.2f%% confidence.\n", 
            class_names[predicted_class], max_prob * 100.0f);

    xil_printf("--- All operations completed ---\n\r");
    cleanup_platform();
    return 0;
}