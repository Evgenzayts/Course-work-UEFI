#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>


extern EFI_BOOT_SERVICES *gBS;

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable){

	Print(L"Program start!\n");

    IN OUT UINTN MemoryMapSize = 0;
    IN OUT EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    OUT UINTN LocalMapKey;
    OUT UINTN DescriptorSize;
    OUT UINT32 DescriptorVersion;
    EFI_STATUS Status;
    
    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &LocalMapKey, &DescriptorSize,&DescriptorVersion);
    if (Status == EFI_BUFFER_TOO_SMALL){
        Print(L"Too small buffer!\n");

        if (MemoryMap != NULL){
            gBS->FreePool(MemoryMap);
        }

        if (gBS->AllocatePool(EfiBootServicesData, MemoryMapSize + 1, (VOID**) &MemoryMap) != EFI_SUCCESS){
            Print(L"Error in allocate!\n");
            goto Exit;
        }

        Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &LocalMapKey, &DescriptorSize, &DescriptorVersion);
        if (Status != EFI_SUCCESS) {
            Print(L"Error in second GetMemoryMap()\n");
            goto Exit;
        }    

    } else if (Status == EFI_INVALID_PARAMETER){
        Print(L"Invalid parameter! "
            "[MemoryMap or MemoryMapSize is NULL]\n");
        goto Exit;
    }

    Print(L"Success!\n");
    
    UINT8 *offset = (UINT8 *) MemoryMap;
    EFI_MEMORY_DESCRIPTOR *desc = NULL;
    for (UINT32 i = 0; i < MemoryMapSize/DescriptorSize; ++i) {
        desc = (EFI_MEMORY_DESCRIPTOR *)offset;

        Print(L"\nMap %d:\n", i);
        Print(L"  Type: %X\n", desc->Type); 
        Print(L"  PhysicalStart: %X\n", desc->PhysicalStart);
        Print(L"  VirtualStart: %X\n", desc->VirtualStart);
        Print(L"  NumberOfPages: %X\n", desc->NumberOfPages);
        Print(L"  Attribute: %X\n", desc->Attribute);

        offset += DescriptorSize;
    }

    Exit:
    if (MemoryMap != NULL) {
        gBS->FreePool(MemoryMap);
    }

    Print(L"Program end!\n");

    return EFI_SUCCESS;
}
