# Пошаговая инструкция

Прежде, чем перейти к настройке проекта и работе с ним, установите следующие пакеты:

    bash$ sudo apt install build-essential uuid-dev iasl git  nasm  python-is-python3
    
**Важно!** Проверьте, что `nasm` последней версии.

## 1. Работа с репозиторием

### Создадим локальый репозиторий

    bash$ mkdir ~/src
    bash$ cd ~/src
    bash$ git clone https://github.com/tianocore/edk2  
    
### Инициализируем submodules

    bash$ cd ~/src/edk2
    bash$ git submodule update --init --recursive
    
### Скомпилируем инструменты для сборки

    bash$ make -C BaseTools
    bash$ . edksetup.sh  
    
### Настройка среды

    bash$ export EDK_TOOLS_PATH=$HOME/src/edk2/BaseTools
    bash$ . edksetup.sh BaseTools  
    
### Редактирование конфигурационных файлов

Далее нужно будет отредактировать `Conf/target.txt` файл для настройки платформы сборки, целевой архитектуры, цепочки инструментов и параметров многопоточности.

#### Устанавливаем информацию о сборке

В файле `Conf/target.txt` найдите следующие строки:
    
    ACTIVE_PLATFORM       = Nt32Pkg/Nt32Pkg.dsc
    TOOL_CHAIN_TAG        = MYTOOLS  
    
Эти строки нужно изменить:
    
    ACTIVE_PLATFORM       = OvmfPkg/OvmfPkgX64.dsc
    TOOL_CHAIN_TAG        = GCC5  
    
_Подсказка: если у вас не установлена gcc5, введите команду `sudo apt-get install gcc-5`_

Также нужно изменить `TARGET_ARCH`:

    TARGET_ARCH           = X64  
    
**Включите многопоточную сборку. Значение по умолчанию для `MAX_CONCURRENT_THREAD_NUMBER` равно 1, что отключает многопоточную сборку. Измените это значение в зависимости от возможностей многопоточности вашей системы. Формула такова: "1 + (2 x потоков процессора)".**  
_Пример: для Intel Core i5 (два процессорных ядра с гиперпоточностью) значение равно 9._

### Соберем Hello World!

    bash$ build  
    
В итоге получим HelloWorld UEFI приложение:

    bash$ ls Build/MdeModule/DEBUG_*/*/HelloWorld.efi  
    
## 2. Работа с OVMF

OVMF — это основанный на EDK II проект, предоставляющий поддержку UEFI для виртуальных машин (QEMU и KVM).

Чтобы запустить OVMF с QEMU, нужно иметь версию QEMU версии 0.9.1 или новее. Для установки QEMU нужно ввести в консоль
        
        sudo apt-get install qemu  
        
### Создание директории

        bash$ mkdir ~/run-ovmf
        bash$ cd ~/run-ovmf  
        
Затем скопируйте файл OVMF.fd в этот каталог, но переименуйте OVMF.fd в bios.bin:

        bash$ cp ~/src/edk2/Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd bios.bin  
        
Затем создайте директорию для использования в качестве образа жесткого диска для QEMU:

        bash$ mkdir hda-contents  
        
### Запускаем QEMU используя OVMF

        bash$ qemu-system-x86_64 -pflash bios.bin -hda fat:rw:hda-contents -net none  
        
Вы должны увидеть графический логотип, и должна запуститься оболочка UEFI.

## 3. Запуск приложения в UEFI

Чтобы запустить приложение из UEFI-Shell, нужно поместить project_name.efi в папку hda-contents:

        bash$ cp ~/src/edk2/Build/MdeModule/DEBUG_GCC5/X64/MdeModulePkg/Application/HelloWorld/HelloWorld/DEBUG/HelloWorld.efi ~/run-ovmf/hda-contents/HelloWorld.efi

Запускаем QEMU, переходим в раздел fs0 (`fs0:`). С помощью команды `ls` можно увидеть файлы в директории (_Например: HelloWorld.efi_). Далее запускаем приложение:

        Shell/ HelloWorld.efi

## Дополнение
(Необязательно)

Для удобства был создан собственный конфигурационный файл `UEFI_projects.inf`, который должен находиться в одной папке с исполняемым С-файлом `Boot.c`. Путь к конфигурационному файлу указан в файле настроек `UEFI_projects.dsc`, путь к которому, в свою очередь, указан в `Conf/target.txt` (`ACTIVE_PLATFORM`).
        
## 4. Вывод карты памяти

### 4.1 Получение карты памяти
Для получения карты памяти необходимо использовать протокол BOOT_SERVICES `EFI_BOOT_SERVICES.GetMemoryMap()`.

```C
typedef
EFI_STATUS
(EFIAPI *EFI_GET_MEMORY_MAP) (
    IN OUT UINTN                 *MemoryMapSize,
    IN OUT EFI_MEMORY_DESCRIPTOR *MemoryMap,
    OUT UINTN                    *MapKey,
    OUT UINTN                    *DescriptorSize,
    OUT UINT32                   *DescriptorVersion
);
 ```
*MemoryMapSize* - указатель на размер выделенного буфера. На входе показывает размер выделенного буфера. На выходе содержит размер буфера, возвращаемый встроенным ПО, если буфер был достаточно большим, или размер буфера, необходимый для размещения карты памяти, если буфер был слишком мал.

*MemoryMap* - указатель на выделенный буфер, где будет размещена информация о карте памяти. Информация представляет собой массив, элементами которого являются структуры `EFI_MEMORY_DESCRIPTOR`.

*MapKey* - указатель на переменную, в которую вернётся ключ для
текущей карты памяти.

*DescriptorSize* - указатель на переменную, в которую будет возвращен размер `EFI_MEMORY_DESCRIPTOR`.

*DescriptorVersion* - версия `EFI_MEMORY_DESCRIPTOR`.

**Важно!** Вызвать `GetMemoryMap()`, получить необходимый размер буфера, после чего выделить память с помощью `EFI_BOOT_SERVICES.AllocatePool()`.

Структура `EFI_MEMORY_DESCRIPTOR`:

```C
typedef struct {
    UINT32               Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS  VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;
```

### 4.2 Вывод информации на экран
Для вывода на экран использовать функцию `Print()` (аналог `printf()` в edk2), расположенную в `edk2/MdePkg/Include/Library/UefiLib.h`.
Вывести по отдельности каждый элемент структуры `EFI_MEMORY_DESCRIPTOR`, используя спецификаторы языка `C`.

## 5. Вывод информации о процессорах

Для получения информации о процессорах необходимо использовать протокол MP_SERVICE протокол, функции которого находятся в библиотеке `Protocol/MpService.h`, и у которого есть зависимость `PiDxe.h`.

Прежде всего требуется инициализировать MpServiceProtocol с помощью функции LocateProtocol:

```C
typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL) (
 IN EFI_GUID *Protocol,
 IN VOID *Registration OPTIONAL,
 OUT VOID **Interface
 );
 
...

 Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID**)&MP);
 ```
Параметры:
*Protocol* - протокол, требуемый для поиска.

*Registration Optional* - ключ регистрации (если NULL - игнорируется).

*Interface* - возвращает указатель на интерфейс протокола.

Далее необходимо получить информацию о процессорах  `EFI_PROCESSOR_INFORMATION` *`ProcessorInfo`* и вывести на экран количество процессоров и сведения о каждом из них:

```C
ProcessorInfo.ProcessorId, 
ProcessorInfo.StatusFlag,
ProcessorInfo.Location.Package,
ProcessorInfo.Location.Core,
ProcessorInfo.Location.Thread
```

**Для получения данной информации следуется воспользоваться функциями `GetNumberOfProcessors` и `GetProcessorInfo`**

_Пример:_

![image](https://user-images.githubusercontent.com/72793414/211311767-ad19a733-8ef9-4f54-b1a8-88ba99d8f450.png)

**Важно!** _Для запуска QEMU на несколько процессоров использовать опцию  -smp._

## 6. Создание загрузчика ОС

Прежде чем передать управление операционной системе, загрузчик переводит процессор в Longmode, создает GDT и размечает память.
Переход в Longmode осуществляется с помощью ассемблерного кода. Для этого сначала необходимо сформировать GDT и PML4.

### 6.1 GDT и PML4
GDT и PML4 формируется два раза. Первый раз в ассемблерном коде формируется времменные GDT и PML4. Второй раз создаются постоянные GDT и PML4, которые 
будут использоваться операционной системой. Это предотвращает случайную "затирку" GDT, а также необходимо для корректной работы табличной трансляции в 
Longmode. Из-за особенностей работы многоядерной системы, случайная информация от разных ядер может затереть временную таблицу дескрипторов, из-за чего и 
требуется дважды формировать GDT.

Постоянные GDT и PML4 создаются в коде на языке С (для этого выделяется память и туда помещаются необходимые значения).
После этого происходит передача управления ассемблерному коду, который заранее размещен в оперативной памяти по адресу 10000h.

Чтобы загрузить код в оперативную память, требуется скомпилировать его (`FASM` или `NASM`), сформировать бинарный файл с помощью утилиты `xxd` и разместить бинарный код в заранеее выделенной памяти.
_Пример:_

```C
char example[] = "\x00\x00\x00\x00\x00\x00\x00\x00"
		 "\xff\xff\x00\x00\x00\x9a\xaf\x00"
		 "\xff\xff\x00\x00\x00\x92\xcf\x00"
		 "\xff\xff\x00\x00\x00\x9a\xcf\x00"
		 "\xff\xff\x00\x00\x00\x92\xcf\x00";
             
...
             
if (gBS->AllocatePool(EfiBootServicesData,
					  size,
					  (void**) &example_entry) != EFI_SUCCESS)
{
    Print(L"Error in allocate!\n");
    return EFI_SUCCESS;
}             

...

gBS->CopyMem(example_entry, example, sizeof(example) - 1);
```

### 6.2 Передача управления

**APIC** (Advanced Programmable Interrupt Controller) – улучшенный программируемый контроллер прерываний.

APIC предназначен для перенаправления прерываний и для рассылки прерываний между процессорами.

Контроллер состоит из двух модулей: LOCAL APIC (LAPIC) и I/O APIC.В отличие от LAPIC, который есть у каждого процессора, I/O APIC – один для 
всей системы. Когда некоторое прерывание приходит на вывод I/O APIC, контроллер направляет его в LAPIC одного из процессоров.

Наличие I/O APIC позволяет сбалансировано распределять прерывания от внешних устройств между процессорами. Регистры LAPIC отображаются в памяти на
физической странице FEE00xxx. Этот адрес одинаков для каждого LAPIC, существующего в системе.

Для отправки прерываний различным процессорам из программы используется регистр ICR (Interrupt Command Register). Он состоит из двух 32-х 
разрядных регистров, расположенных по адресам 0x300 и 0x310. С помощью регистра ICR можно запустить определенный процессор (или несколько 
процессоров) на выполнение заданного кода, например:

```C
uint32_t * const APIC_ICR_LOW = (void *) 0xfee00300;
uint32_t * const APIC_ICR_HIG = (void *) 0xfee00310;

// INIT
*APIC_ICR_HIG = (uint32_t) ap_apic_id << 24;	// ap_apic_id - номер процессора
*APIC_ICR_LOW = 0x00000500;			// На процессоре с нулевым id работает UEFI

for (volatile unsigned i = 0; i < 0xffffff; ++i) ;  // Задержка

// SIPI
*APIC_ICR_LOW = ((uint32_t) 0x00000600 | (init_code_entry >> 12));
```
Передача управления происходит по адресу 10000h, где расположен ассемблерный код в бинарном виде.
Этот код создает временные GDT и PML4, после чего осуществляет переход в Longmode.
Далее осуществляется загрузка постоянных GDT и PM4 и запуск кода ядра операционной системы.
