Lab 0
---

## Target
- Cross-Platform 編譯程式，並能在 raspi3b 正常執行。

## Cross-Platform
- 為了寫出能在目標設備( Arm 架構)上執行的程式，需要使用 cross complier ，能在其他架構上編譯出執行檔。
  ```sh
  # 編譯 object file
  aarch64-linux-gnu-gcc -c test.S -o test.o
  # 透過 linker 生成 ELF file
  aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf test.o
  # 透過 objcopy 生成 raw binary image
  aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
  ```
- 在編譯出執行檔後，再透過 qemu 模擬確認正確。
  ```sh
  qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm
  ```

## Linker
- `start.s` 中，將指令指定寫在 `.text.boot` section。
- `linker.ld` 中，將 object file 中的 `.text.boot` 合併至 ELF file 的 `.text` 。
- 因此最後執行時，就會執行到 `start.s` 的內容。

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```