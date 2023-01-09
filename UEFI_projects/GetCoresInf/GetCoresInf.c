#include <Uefi.h>
#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/MpService.h>

extern EFI_GUID gEfiMpServiceProtocolGuid;
extern EFI_BOOT_SERVICES *gBS;

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS                Status;
    EFI_MP_SERVICES_PROTOCOL  *MP=NULL;
    UINTN                     i;  
    UINTN                     NumProcessors;
    UINTN                     NumberOfEnabledProcessors;
    EFI_PROCESSOR_INFORMATION ProcessorInfo;

    Print(L"Program start!\n\n");
    
    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID**)&MP);

    if (EFI_ERROR (Status))
    {
        Print(L"Unable to initialize MP protocol interface!");
        return EFI_SUCCESS;
    }
    
    Status = MP->GetNumberOfProcessors(MP, &NumProcessors, &NumberOfEnabledProcessors);
    
    if (EFI_ERROR (Status))
    {
        Print( L"MP->GetNumEnabledProcessors:"
                "Unable to determine number of processors\n") ;
        return EFI_SUCCESS;
    }

    Print(L"Number of Processors %d\n",NumProcessors);
    Print(L"Number of Enabled Processors %d\n",NumberOfEnabledProcessors);
    
    for (i=0; i < NumProcessors; i++)
    {
        Status = MP->GetProcessorInfo(MP, i , &ProcessorInfo);

        if (EFI_ERROR(Status))
        {
            Print(L"Error in GetProcessorInfo()\n");
            return EFI_SUCCESS;
        }
        
        Print( L"Prcoessor #%d ACPI Processor ID = %lX,"
                "Flags = %x, Package = %x, Core = %x, Thread = %x\n", 
                i,
                ProcessorInfo.ProcessorId, 
                ProcessorInfo.StatusFlag,
                ProcessorInfo.Location.Package,
                ProcessorInfo.Location.Core,
                ProcessorInfo.Location.Thread
            );
    }

    Print(L"\nProgram end!\n");
    
    return EFI_SUCCESS;
}
