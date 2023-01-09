# Очень важная информация!
## Запуск EfiShell
1. Создать директорию из которой в дальнейшем будет производиться запуск qemu. (Например, run-ovmf)
2. Скопировать в run-ovmf файл OVMF.fd под именем bios.bin (cp /path/to/ovmf/OVMF.fd bios.bin). Файл OVMF.fd располагается в папке /usr/share/qemu. Также файл можно найти в директории ~/src/edk2/Build/OvmfX64/DEBUG_GCC5/FV, предварительно запустив команду build -p OvmfPkg/OvmfPkgX64.dsc из edk2.
3. Создать в run-ovmf директорию для сохранения *.efi файлов (Например hda-contents).
4. Запустить qemu командой qemu-system-x86_64 -pflash bios.bin -hda fat:rw:hda-contents -net none
5. Ссылка на оригинальный гайд: https://github.com/tianocore/tianocore.github.io/wiki/How-to-run-OVMF

*Примечание:* Репозиторий содержит папку run-ovmf/hda-contents, в которой находятся файлы .efi. Их можно запустить, чтобы свериться с выводом.

## Где и что смотреть
1. https://wiki.osdev.org/Symmetric_Multiprocessing
2. https://wiki.osdev.org/APIC
3. https://www.lab-z.com/mpscpu/
4. https://uefi.org/specifications
5. Документация EDK2 (https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)
6. https://wikileaks.org/ciav7p1/cms/page_36896783.html
