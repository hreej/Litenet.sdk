// ==============================================================
// File generated on Sat Dec 13 18:37:19 +0800 2025
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
// SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
// IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlitenet.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLitenet_CfgInitialize(XLitenet *InstancePtr, XLitenet_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Crtl_bus_BaseAddress = ConfigPtr->Crtl_bus_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLitenet_Start(XLitenet *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL) & 0x80;
    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLitenet_IsDone(XLitenet *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLitenet_IsIdle(XLitenet *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLitenet_IsReady(XLitenet *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLitenet_EnableAutoRestart(XLitenet *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL, 0x80);
}

void XLitenet_DisableAutoRestart(XLitenet *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_AP_CTRL, 0);
}

void XLitenet_InterruptGlobalEnable(XLitenet *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_GIE, 1);
}

void XLitenet_InterruptGlobalDisable(XLitenet *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_GIE, 0);
}

void XLitenet_InterruptEnable(XLitenet *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_IER);
    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_IER, Register | Mask);
}

void XLitenet_InterruptDisable(XLitenet *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_IER);
    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_IER, Register & (~Mask));
}

void XLitenet_InterruptClear(XLitenet *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLitenet_WriteReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_ISR, Mask);
}

u32 XLitenet_InterruptGetEnabled(XLitenet *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_IER);
}

u32 XLitenet_InterruptGetStatus(XLitenet *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLitenet_ReadReg(InstancePtr->Crtl_bus_BaseAddress, XLITENET_CRTL_BUS_ADDR_ISR);
}

