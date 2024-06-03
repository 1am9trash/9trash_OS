Lab 1
---

## Target
- 完成 basic shell 。
- 使用 rpi3’s peripherals 。
- 設定 mini UART 。
- 設定 Mailbox 。

## Peripherals Reference
- [ARM Peripherals](https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf): 以下內容基本上都可以由 document 中找到。

## MMIO (Memory-Mapped I/O)
- MMIO 將 peripheral register 映射到設備的 memory 中，在此機制下，通過讀寫對應的地址，就可以與 peripheral 通信。
- 由於已經映射設備內的 memory ，因此不需要專用的 I/O 指令，可直接透過一般讀寫指令互動(如 `mov` 、 `ldr` 等)。
- raspi3 透過 MMIO 存取 peripheral。
  - 在 ARM CPU 中， physical memory `0x3fnnnnnn`  一般為 peripheral 保留 ，對應 bus 上的 `0x7ennnnnn` 。
  - 在存取 `0x3fnnnnnn` 時，背後會去 bus 存取對應的 address ，最終與 peripheral register 互動。

## mini UART
- raspi3 中的 GPIO 腳位可以用作 UART 與 SPI 。
- mini UART 流程大致如下(詳細流程參考 `uart.c` 實作):
  ```c
  void uart_init() {
      // 1. 透過 aux 啟用 mini UART
      // 2. 禁用 TX/RX
      // 3. 透過 aux 配置 mini UART ，如 baud rate 等等
      // 4. 將 mini UART 對應到 GPIO 14 、 15 (alt5 for mini UART)
      // 5. 禁用 GPIO pull-up/down
      // 6. 重新開啟 TX/RX
  }
  ```

## Mailbox
- Mailbox tag buffer
  - 0-3: tag id
  - 4-7: value buffer size, which is maximum of request and response value buffer's length
  - 8-11: bit 31 for request(0) or response(1), other bits for length
  - 12-19: value buffer
- [Mailbox Structure](https://jsandler18.github.io/extra/prop-channel.html)
- [Mailbox Interface](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface)

## Implement

##### `start.s`
- 只使用一個 core (id=0) ，其他 core 都透過 `wfe` 閒置。
  ```armasm
  _start:
    mrs     x1, mpidr_el1     // get cpu id
    and     x1, x1, #3        // [0:2] is enough
    cbz     x1, 2f            // if cpu id = 0, goto 2f
  1:
    wfe
    b       1b
  2:
    // ...
  ```
- 根據 `linker` 配置， section 由 `0x80000` 開始依次放置，因此將 stack 設置到 `.text` (第一個 section )的開頭位置。( stack 一般由 high address 往 low address 長)
  ```armasm
  ldr     x1, =_start
  mov     sp, x1            // set stack pointer 
  ```
- 在 `linker` 中，有紀錄 `bss` 的相關參數，在初始時，要將該 section 都設定為 0 。
  ```armasm
  2:
    // ...
    ldr     x1, =__bss_start  // clear bss
    ldr     w2, =__bss_size
  3:
    cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b            // if w2 != 0, goto 3b
  4:
    bl      main
    b       1b                // halt core when return
  ```

##### `linker.ld`
- 加入 `.text` 、 `rodata` 、 `data` 、 `bss` 等 section 。
- 紀錄 `bss` 清空需要的相關參數。
  ```ld
  SECTIONS
  {
    // ...
    .bss ALIGN(16) (NOLOAD) :
    {
      __bss_start = .;
      *(.bss .bss.*)
      __bss_end = .;
    }
  }
  __bss_size = (__bss_end - __bss_start) >> 3;
  ```

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```