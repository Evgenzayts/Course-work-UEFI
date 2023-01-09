#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

extern EFI_BOOT_SERVICES *gBS;



char init[] = "\x66\x31\xc0\x66\xb8\xea\x00\x01\x00\x67\x66\xa3\xe4\x00\x01\x00"
			  "\x66\xb8\xf8\x00\x01\x00\x67\x66\xa3\x22\x01\x01\x00\x67\x66\x0f"
			  "\x01\x15\x20\x01\x01\x00\xe4\x92\x0c\x02\xe6\x92\xfa\xe4\x70\x0c"
			  "\x80\xe6\x70\x0f\x20\xc0\x0c\x01\x0f\x22\xc0\x67\x66\xff\x2d\x43"
			  "\x00\x01\x00\x49\x00\x01\x00\x18\x00\x66\xb8\x20\x00\x8e\xd8\x66"
			  "\xb8\x10\x00\x8e\xc0\xbc\x00\x00\x20\x00\x0f\x20\xe0\x83\xc8\x20"
			  "\x0f\x22\xe0\xbf\x00\x00\x17\x00\xb9\x00\x10\x00\x00\x31\xc0\xf3"
			  "\xab\xbf\x00\x00\x17\x00\x26\xc7\x07\x07\x10\x17\x00\xbf\x00\x10"
			  "\x17\x00\xb9\x02\x00\x00\x00\xb8\x07\x20\x17\x00\xab\x26\xc7\x07"
			  "\x00\x00\x00\x00\x83\xc7\x04\x05\x00\x10\x00\x00\xe2\xee\xbf\x00"
			  "\x20\x17\x00\xb9\x00\x04\x00\x00\xb8\x87\x00\x00\x00\xab\x26\xc7"
			  "\x07\x00\x00\x00\x00\x83\xc7\x04\x05\x00\x00\x20\x00\xb8\x00\x00"
			  "\x17\x00\x0f\x22\xd8\xb9\x80\x00\x00\xc0\x0f\x32\x0d\x00\x01\x00"
			  "\x00\x0f\x30\x0f\x20\xc0\x0d\x00\x00\x00\x80\x0f\x22\xc0\xff\x2d"
			  "\xe4\x00\x01\x00\x00\x00\x00\x00\x08\x00\x48\xc7\xc0\x00\x80\x00"
			  "\x00\xff\xe0\x90\x90\x90\x90\x90\x00\x00\x00\x00\x00\x00\x00\x00"
			  "\xff\xff\x00\x00\x00\x9a\xaf\x00\xff\xff\x00\x00\x00\x92\xcf\x00"
			  "\xff\xff\x00\x00\x00\x9a\xcf\x00\xff\xff\x00\x00\x00\x92\xcf\x00"
			  "\x27\x00";

char lgdt[] = "\x0f\x01\x15\x1e\x80\x00\x00\x48\x31\xc0\x8b\x05\x14\x00\x00\x00"
			  "\x0f\x22\xd8\x48\x31\xc0\x8b\x05\x0c\x00\x00\x00\xff\xe0\x00\x00"
			  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

char c_code[] = "\xbc\x00\x00\x20\x00\x48\x89\xe5\xe8\x13\x00\x00\x00\xeb\xfe\x90"
				"\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
				"\xf3\x0f\x1e\xfa\x48\x8b\x05\xe5\xff\xff\xff\xba\x00\x00\x0a\x00"
				"\x48\x89\x05\xe1\xff\xff\xff\x31\xc0\x48\x8b\x35\xd8\xff\xff\xff"
				"\x48\x8b\x0e\x48\xff\xc1\x48\x89\x0e\x48\x8b\x0a\x48\xff\xc1\x48"
				"\x89\x0a\x3d\xb7\x0b\x00\x00\x7f\x06\x48\x83\xc2\x10\xeb\x0f\x3d"
				"\x6f\x17\x00\x00\x7e\x04\x31\xc0\xeb\xcf\x48\x83\xea\x10\xff\xc0"
				"\xeb\xc7";

char gdt[] = "\x00\x00\x00\x00\x00\x00\x00\x00"  // 0
			 "\xff\xff\x00\x00\x00\x9a\xaf\x00"  // LM_code
			 "\xff\xff\x00\x00\x00\x92\xcf\x00"  // LM_data
			 "\xff\xff\x00\x00\x00\x9a\xcf\x00"  // PM_code
			 "\xff\xff\x00\x00\x00\x92\xcf\x00"; // PM_data

EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	UINT8* init_entry = (void*) 0x10000;
	UINT8* lgdt_entry = (void*) 0x8000;
	UINT8* c_entry = NULL;
	UINT8* gdt_entry = NULL;
	UINT8* pml4_entry = NULL;
	volatile unsigned pml4_size = 0x5000;
	volatile unsigned size = sizeof(c_code) - 1 +
							 sizeof(gdt) - 1 +
							 pml4_size;
	UINT8 apic_id = 1;

	Print(L"Programm start!\n");

	// Выделение памяти под код ядра, GDT и PML4
	if (gBS->AllocatePool(EfiBootServicesData,
						  size,
						  (void**) &c_entry) != EFI_SUCCESS)
	{
            Print(L"Error in allocate!\n");
            return EFI_SUCCESS;
    }

    Print(L"Kernel code on address: %p\n", c_entry);

    // Размещение кода в выделенной памяти
	gdt_entry = c_entry + sizeof(c_code) - 1;
	pml4_entry = gdt_entry + sizeof(gdt) - 1 + 0x1000;
	pml4_entry = (void*)((UINT64) pml4_entry &
				 (UINT64) 0xffffffffffff000);
	gBS->CopyMem(init_entry, init, sizeof(init) - 1);
	gBS->CopyMem(lgdt_entry, lgdt, sizeof(lgdt) - 1);
	gBS->CopyMem(c_entry, c_code, sizeof(c_code) - 1);
	gBS->CopyMem(gdt_entry, gdt, sizeof(gdt) - 1);

	// Разметка памяти
	gBS->SetMem((void*) pml4_entry, 0x4000, 0);

	// PML4
	UINT64 value;
	value = (UINT64) (pml4_entry + 0x1000) |
			(UINT64) 0x0000000000000007;
	gBS->CopyMem(pml4_entry, &value, 8);

	// Каталог страниц
	value = (UINT64) (pml4_entry + 0x2000) |
			(UINT64) 0x0000000000000007;
	gBS->CopyMem(pml4_entry + 0x1000, &value, 8);

	value = (UINT64) (pml4_entry + 0x3000) |
			(UINT64) 0x0000000000000007;
	gBS->CopyMem(pml4_entry + 0x1000 + 8, &value, 8);

	// Таблицы страниц
	for(volatile unsigned i = 0; i < 0x400; ++i) {
		value = (UINT64) (i * 0x200000) |
				(UINT64) 0x0000000000000087;
		gBS->CopyMem(pml4_entry + 0x2000 + i*8, &value, 8);
	}

	// Загрузка адресов в метки кода lgdt
	UINT16 gdt_size = sizeof(gdt) - 1;
	gBS->CopyMem(lgdt_entry + sizeof(lgdt) - 1 - 4, &c_entry, 4);
	gBS->CopyMem(lgdt_entry + sizeof(lgdt) - 1 - 8, &pml4_entry, 4);
	gBS->CopyMem(lgdt_entry + sizeof(lgdt) - 1 - 12, &gdt_entry, 4);
	gBS->CopyMem(lgdt_entry + sizeof(lgdt) - 1 - 14, &gdt_size, 2);

	// Передача управления ассемблерному коду
	UINT32* APIC_ERROR = (void*) 0xfee00280;
	UINT32* APIC_ICR_LOW = (void*) 0xfee00300;
	UINT32* APIC_ICR_HIG = (void*) 0xfee00310;

	// INIT
	*APIC_ERROR = 0;
	*APIC_ICR_HIG = (UINT32) apic_id << 24;
	*APIC_ICR_LOW = 0x00000500;

	for(volatile unsigned i = 0; i < 0xffffff; ++i);  // Задержка

	// SIPI
	*APIC_ERROR = 0;
	*APIC_ICR_LOW = ((UINT32) 0x00000600 | ((UINT32) 0x00010000 >> 12));
	
	Print(L"Progamm end!\n");
	
    return EFI_SUCCESS;
}
