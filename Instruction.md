# Пошаговая инструкция

Прежде, чем перейти к настройке проекта и работе с ним, установите последнюю версию `nasm`.

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

        cp ~/src/edk2/Build/MdeModule/DEBUG_GCC5/X64/MdeModulePkg/Application/HelloWorld/HelloWorld/DEBUG/HelloWorld.efi ~/run-ovmf/hda-contents/HelloWorld.efi

Запускаем QEMU, переходим в раздел fs0 (`fs0:`). С помощью команды `ls` можно увидеть файлы в директории (_Например: HelloWorld.efi_). Далее запускаем приложение:

        HelloWorld.efi
        
## 4. Вывод карты памяти
