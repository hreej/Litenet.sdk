// ==============================================================
// File generated on Sat Dec 13 18:37:19 +0800 2025
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
// SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
// IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLITENET_H
#define XLITENET_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xlitenet_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Crtl_bus_BaseAddress;
} XLitenet_Config;
#endif

typedef struct {
    u32 Crtl_bus_BaseAddress;
    u32 IsReady;
} XLitenet;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLitenet_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLitenet_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLitenet_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLitenet_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XLitenet_Initialize(XLitenet *InstancePtr, u16 DeviceId);
XLitenet_Config* XLitenet_LookupConfig(u16 DeviceId);
int XLitenet_CfgInitialize(XLitenet *InstancePtr, XLitenet_Config *ConfigPtr);
#else
int XLitenet_Initialize(XLitenet *InstancePtr, const char* InstanceName);
int XLitenet_Release(XLitenet *InstancePtr);
#endif

void XLitenet_Start(XLitenet *InstancePtr);
u32 XLitenet_IsDone(XLitenet *InstancePtr);
u32 XLitenet_IsIdle(XLitenet *InstancePtr);
u32 XLitenet_IsReady(XLitenet *InstancePtr);
void XLitenet_EnableAutoRestart(XLitenet *InstancePtr);
void XLitenet_DisableAutoRestart(XLitenet *InstancePtr);


void XLitenet_InterruptGlobalEnable(XLitenet *InstancePtr);
void XLitenet_InterruptGlobalDisable(XLitenet *InstancePtr);
void XLitenet_InterruptEnable(XLitenet *InstancePtr, u32 Mask);
void XLitenet_InterruptDisable(XLitenet *InstancePtr, u32 Mask);
void XLitenet_InterruptClear(XLitenet *InstancePtr, u32 Mask);
u32 XLitenet_InterruptGetEnabled(XLitenet *InstancePtr);
u32 XLitenet_InterruptGetStatus(XLitenet *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
