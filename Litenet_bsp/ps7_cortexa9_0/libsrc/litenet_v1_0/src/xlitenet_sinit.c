// ==============================================================
// File generated on Sat Dec 13 18:37:19 +0800 2025
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
// SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
// IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xlitenet.h"

extern XLitenet_Config XLitenet_ConfigTable[];

XLitenet_Config *XLitenet_LookupConfig(u16 DeviceId) {
	XLitenet_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XLITENET_NUM_INSTANCES; Index++) {
		if (XLitenet_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XLitenet_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XLitenet_Initialize(XLitenet *InstancePtr, u16 DeviceId) {
	XLitenet_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XLitenet_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XLitenet_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

