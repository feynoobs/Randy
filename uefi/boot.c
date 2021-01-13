/**
 * 
 */

#include <efi/efi.h>
#include <efi/efilib.h>

#define INIT_HANDLES 24

static VOID Panic(EFI_HANDLE handle, EFI_BOOT_SERVICES *BS, EFI_STATUS status, CHAR16 *msg)
{
    Print(msg);
    uefi_call_wrapper(BS->Exit, 4, handle, status, 0, NULL);
}

static INT8 Memcmp(VOID *src1, VOID *src2, UINTN len)
{
    INT8 result = 0;
    for (UINTN i = 0; (i < len) && (result == 0); ++i) {
        result = *(UINT8 *)((UINT8 *)src1 + i) - *(UINT8 *)((UINT8 *)src2 + i);
    }

    return result;
}

VOID *Malloc(EFI_HANDLE handle, EFI_BOOT_SERVICES *BS, UINTN size)
{
    VOID *out;
    EFI_STATUS status;

    status = uefi_call_wrapper(BS->AllocatePool, 3, AllocateAddress, size, &out);
    if (status != EFI_SUCCESS) {
        Panic(handle, BS, status, L"malloc fail...");
    }

    return out;
}

VOID Free(EFI_HANDLE handle, EFI_BOOT_SERVICES *BS, VOID *pool)
{
    EFI_STATUS status;
    status = uefi_call_wrapper(BS->FreePool, 1, &pool);
    if (status != EFI_SUCCESS) {
        Panic(handle, BS, status, L"free fail...");
    }
}

VOID debugDumpDevicePath(EFI_DEVICE_PATH *devicePath)
{
    UINT8 cur = 0;
    for (;;) {
        UINT8 type = *(UINT8 *)((UINT8 *)devicePath + cur);
        UINT16 len = *(UINT16 *)((UINT8 *)devicePath + cur + 2);
        for (UINT16 i = 0; i < len; i++) {
            Print(L"%02x ", *(UINT8 *)((UINT8 *)devicePath + cur + i));
        }
        cur += len;
        Print(L"\n");
        if (type == END_DEVICE_PATH_TYPE) {
            break;
        }
    }
}

/**
 * @see 
 */
EFI_STATUS efi_main(EFI_HANDLE handle, EFI_SYSTEM_TABLE *ST)
{
    EFI_BOOT_SERVICES *BS = ST->BootServices;
    EFI_GUID guidLoadedImage = LOADED_IMAGE_PROTOCOL;
    EFI_GUID guidBlockIO = BLOCK_IO_PROTOCOL;
    EFI_GUID guidDevicePath = DEVICE_PATH_PROTOCOL;
    EFI_STATUS status;
    EFI_HANDLE *handles;
    UINTN handlesSize;
    UINTN numOfHandles;
    EFI_LOADED_IMAGE *this;
    EFI_DEVICE_PATH *thisPath;

    InitializeLib(handle, ST);

    status = uefi_call_wrapper(BS->HandleProtocol, 3, handle, &guidLoadedImage, &this);
    if (status != EFI_SUCCESS) {
        Panic(handle, BS, status, L"Load Image fail...");
    }

    status = uefi_call_wrapper(BS->HandleProtocol, 3, this->DeviceHandle, &guidDevicePath, &thisPath);
    if (status != EFI_SUCCESS) {
        Panic(handle, BS, status, L"Get this path fail...");
    }

    handlesSize = INIT_HANDLES * sizeof(EFI_HANDLE);
    handles = Malloc(handle, BS, handlesSize);
    status = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &guidBlockIO, NULL, &handlesSize, handles);
    switch (status) {
        // 普通
        case EFI_SUCCESS:
            break;
        // 要素が足りない場合拡大された要素でやり直し
        case EFI_BUFFER_TOO_SMALL:
            status = uefi_call_wrapper(BS->LocateHandle, 5, ByProtocol, &guidBlockIO, NULL, &handlesSize, handles);
            if (status != EFI_SUCCESS) {
                Panic(handles, BS, status, L"Block IO Protocol fail...2");
            }
            break;
        // 異常
        default:
            Panic(handles, BS, status, L"Block IO Protocol fail...1");
            break;
    }
    numOfHandles = handlesSize / sizeof(EFI_HANDLE);
    for (int i = 0; i < numOfHandles; ++i) {
        EFI_BLOCK_IO *blockIO;
        status = uefi_call_wrapper(BS->HandleProtocol, 3, handles[i], &guidBlockIO, &blockIO);
        if (status == EFI_SUCCESS) {
            // なんかよくわからんけどこれがTRUEだと物理実装されてそう
            if (blockIO->Media->LogicalPartition == TRUE) {
                EFI_DEVICE_PATH *devicePath;
                status = uefi_call_wrapper(BS->HandleProtocol, 3, handles[i], &guidDevicePath, &devicePath);
                if (status != EFI_SUCCESS) {
                    Panic(handles, BS, status, L"Get target path fail...");
                }
                EFI_DEVICE_PATH *curThisPath = thisPath;
                EFI_DEVICE_PATH *curDevicePath = devicePath;
                // EFIと同じデバイス(HDD)のパーティションを探す
                // sameDeviceがTRUEで処理抜けたら同じデバイス
                BOOLEAN sameDevice = TRUE;
                for (;;) {
                    if ((curThisPath->Type   == END_DEVICE_PATH_TYPE) ||
                        (curDevicePath->Type == END_DEVICE_PATH_TYPE)) {
                        break;
                    }
                    UINT16 thisLength = *(UINT16 *)((UINT8 *)curThisPath + 2);
                    UINT16 deviceLength = *(UINT16 *)((UINT8 *)curDevicePath + 2);
                    if ((curThisPath->Type   != MEDIA_DEVICE_PATH) ||
                        (curDevicePath->Type != MEDIA_DEVICE_PATH)) {
                        if (thisLength == deviceLength) {
                            if (Memcmp(curThisPath, curDevicePath, thisLength) != 0) {
                                sameDevice = FALSE;
                            }
                        }
                        else {
                            sameDevice = FALSE;
                        }
                    }
                    curThisPath = (EFI_DEVICE_PATH *)(((UINT8 *)curThisPath + thisLength));
                    curDevicePath = (EFI_DEVICE_PATH *)(((UINT8 *)curDevicePath + deviceLength));
                }
                if (sameDevice == TRUE) {
                    debugDumpDevicePath(devicePath);
                }
            }
        }
    }

    return EFI_LOAD_ERROR;
}
