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
Выводите по отдельности каждый элемент структуры `EFI_MEMORY_DESCRIPTOR`, используя спецификаторы языка `C`.
