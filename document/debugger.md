Debugger
---

#### Run code in qemu with debugger 
- Compile
  ```sh
  # -g: reserve debugging information.
  aarch64-linux-gnu-gcc -Wall -g -c src/main.s -o build/main.o
  ```
- Run code in qemu
  ```sh
  # -s: set gdb server on 1234 port 
  # -S: freeze CPU at startup
  qemu-system-aarch64 -M raspi3b -kernel build/kernel8.img -display none -d in_asm -s -S
  ```
- Debugger
  ```sh
  file kernel8.elf
  target remote 127.0.0.1:1234
  ```