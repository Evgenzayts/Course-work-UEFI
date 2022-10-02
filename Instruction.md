# Пошаговая инструкция

## 1. Работа с репозиторием

### Создадим локальый репозиторий

    bash$ mkdir ~/src
    bash$ cd ~/src
    bash$ git clone https://github.com/tianocore/edk2
    
### Инициализируем submodules

    bash$ cd ~/src/edk2
    bash$ git submodule update --init

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
    
    ACTIVE_PLATFORM       = MdeModulePkg/MdeModulePkg.dsc
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
