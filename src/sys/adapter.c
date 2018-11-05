/**
 * @file sys/adapter.c
 *
 * @copyright 2018 Bill Zissimopoulos
 */
/*
 * This file is part of WinSpd.
 *
 * You can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 3 as published by the Free Software
 * Foundation.
 *
 * Licensees holding a valid commercial license may use this software
 * in accordance with the commercial license agreement provided in
 * conjunction with the software.  The terms and conditions of any such
 * commercial license agreement shall govern, supersede, and render
 * ineffective any application of the GPLv3 license to this software,
 * notwithstanding of any reference thereto in the software or
 * associated repository.
 */

#include <sys/driver.h>

ULONG SpdHwFindAdapter(
    PVOID DeviceExtension,
    PVOID HwContext,
    PVOID BusInformation,
    PVOID LowerDevice,
    PCHAR ArgumentString,
    PPORT_CONFIGURATION_INFORMATION ConfigInfo,
    PBOOLEAN Again)
{
    ULONG Result = SP_RETURN_NOT_FOUND;
    SPD_ENTER(adapter,
        ASSERT(PASSIVE_LEVEL == KeGetCurrentIrql()));

    ConfigInfo->MaximumTransferLength = SP_UNINITIALIZED_VALUE;
    ConfigInfo->NumberOfPhysicalBreaks = SP_UNINITIALIZED_VALUE;
    ConfigInfo->AlignmentMask = 0;
    ConfigInfo->NumberOfBuses = 1;
    ConfigInfo->ScatterGather = TRUE;
    ConfigInfo->Master = TRUE;
    ConfigInfo->CachesData = TRUE;
    ConfigInfo->AdapterScansDown = FALSE;
    ConfigInfo->MaximumNumberOfTargets = 1;
    ConfigInfo->MaximumNumberOfLogicalUnits = SCSI_MAXIMUM_LUNS_PER_TARGET;
    ConfigInfo->WmiDataProvider = FALSE;
    ConfigInfo->SynchronizationModel = StorSynchronizeFullDuplex;
    ConfigInfo->VirtualDevice = TRUE;

    Result = SP_RETURN_FOUND;

    SPD_LEAVE(adapter,
        "%p, HwContext=%p, BusInformation=%p, LowerDevice=%p, ArgumentString=\"%s\"",
        " = %lu",
        DeviceExtension, HwContext, BusInformation, LowerDevice, ArgumentString, Result);
    return Result;
}

BOOLEAN SpdHwInitialize(PVOID DeviceExtension0)
{
    BOOLEAN Result = FALSE;
    SPD_ENTER(adapter);

    SPD_DEVICE_EXTENSION *DeviceExtension = DeviceExtension0;

    KeInitializeSpinLock(&DeviceExtension->SpinLock);
    InitializeListHead(&DeviceExtension->LogicalUnitList);

    Result = TRUE;

    SPD_LEAVE(adapter,
        "%p", " = %d",
        DeviceExtension0, Result);
    return Result;
}

VOID SpdHwFreeAdapterResources(PVOID DeviceExtension)
{
    SPD_ENTER(adapter,
        ASSERT(PASSIVE_LEVEL == KeGetCurrentIrql()));

    SPD_LEAVE(adapter,
        "%p", "",
        DeviceExtension);
}

BOOLEAN SpdHwResetBus(PVOID DeviceExtension, ULONG PathId)
{
    BOOLEAN Result = FALSE;
    SPD_ENTER(adapter);

    Result = TRUE;

    SPD_LEAVE(adapter,
        "%p, PathId=%lu", " = %d",
        DeviceExtension, PathId, Result);
    return Result;
}

SCSI_ADAPTER_CONTROL_STATUS SpdHwAdapterControl(
    PVOID DeviceExtension,
    SCSI_ADAPTER_CONTROL_TYPE ControlType,
    PVOID Parameters)
{
    SCSI_ADAPTER_CONTROL_STATUS Result = ScsiAdapterControlUnsuccessful;
    SPD_ENTER(adapter);

    switch (ControlType)
    {
    case ScsiQuerySupportedControlTypes:
#define SetSupportedControlType(i)      \
    if (((PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters)->MaxControlType > (i))\
        ((PSCSI_SUPPORTED_CONTROL_TYPE_LIST)Parameters)->SupportedTypeList[(i)] = TRUE
        SetSupportedControlType(ScsiQuerySupportedControlTypes);
        SetSupportedControlType(ScsiStopAdapter);
        SetSupportedControlType(ScsiRestartAdapter);
        Result = ScsiAdapterControlSuccess;
        break;
#undef SetSupportedControlType

    case ScsiStopAdapter:
        Result = ScsiAdapterControlSuccess;
        break;

    case ScsiRestartAdapter:
        Result = ScsiAdapterControlSuccess;
        break;

    default:
        Result = ScsiAdapterControlUnsuccessful;
        break;
    }

    SPD_LEAVE(adapter,
        "%p, ControlType=%s", " = %d",
        DeviceExtension, AdapterControlSym(ControlType), (int)Result);
    return Result;
}
