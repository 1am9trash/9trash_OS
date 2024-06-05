Lab 3
---

## Target
- 處理 exception 。
- 處理 interrupt ， 了解 peripherals 如何 interrupt CPU 。
- Understand how to multiplex a timer.
- Understand how to concurrently handle I/O devices.

## Exception

##### Exception Handling
- `el0`: application
- `el1`: rich OS
- `el2`: hypervisor
- `el3`: firmware

##### `el2` to `el1`
- `hcr_el2`: Hypervisor Configuration Register
  - 用來控制 hypervisor configuration 。
  - `E2H` (bit 31): 用來控制 `el1` 中使用 `aarch64`(1) 或 `aarch32`(0) 。
- `spsr_el2`: Saved Program Status Register
  - 當發生到 `el2` 的 exception 時， 用來儲存 processor state ，包含 `el1h` 、 `el1t` 等。
  - DAIF (bit 6-9): 用以 disable interrupt 。
  - M (bit 0-3): current processor mode ， `0b0101` 代表 `EL1h` ，使用 `SP1` Stack Pointer。
- Implement
  ```armasm
  go_el1:
    mov x0, (1 << 31)         // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5             // EL1h (SPSel = 1) with interrupt disabled
                              // (0b1111 << 6) | (0b0101 << 0)
    msr spsr_el2, x0
    adr x1, set_el1
    msr elr_el2, x1
    eret                      // return to EL1
  
  set_el1:
    ldr x1, =0x60000          // set stack pointer 
    mov sp, x1
    ret
  ```

##### `el1` to `el0`
- `spsr_el1`: Saved Program Status Register
- Implement
  ```armasm
  go_el0:
    mov x0, 0x0
    msr spsr_el1, x0
    adr x1, set_el0
    msr elr_el1, x1
    eret                      // return to EL0
  
  set_el0:
    ldr x1, =0x40000          // set stack pointer 
    mov sp, x1
    ret
  ```

##### Exception 跳轉流程
- `el2`
  - 開始執行時，默認在 `el2` 。
  - 初始化 `bss` (可在 `el1` 做) 。
- `el1`
  - 設置 exception vector table: 將 table address 放到 `vbar_el1` 。
  - enable timer 。
- `el0`
  - enable uart (可在`el1`做) 。
  - 執行`main()` 。
- `eret`
  - 不同`el`跳轉需使用`eret` 。
  - 設置`spsr_elx`: 儲存process status 。
  - 設置`elr_elx`: 決定跳轉後位置 。

## Interrupt
- 由於將程式放在 `el0` 中執行，因此所有需要權限的操作都要透過interrupt提升 `el` 。
- interrupt 進入時，會根據 exception vector table 進入 `irq_router()` ，並執行對應的 handler 。

## User Program
- user program 會被提前編成 `img` 檔，透過 `cpio` 放入file system 。
- 需要執行特定程式時，透過 lab2 中的 `cpio.c` 找到其位置並跳轉 。
- 預設的 user program 會 `svc` 三個 exception 後 `ret` 。

## Concurrent I/O Devices
- 之前的 uart 是用 hard polling 的方式運行，浪費大量 cpu time ，現在改成 interrupt trigger 。
- `uart_putc()` 僅會將 data 存到 write queue 中，等到非同步的 uart interrupt 通知可以輸出時，才會真正輸出。
- 有 uart 輸入時，會產生非同步的 uart interrupt ，將 data 讀到 read queue 中，等到 `uart_putc()` 呼叫時，再從 read queue 中拿 data 。

## Address Layout Info
- `el2 sp`: `0x80000`
- `el1 sp`: `0x60000`
- `el0 sp`: `0x40000`
- initrd address: `0x8000000`
- heap: 在 `linker.ld` 宣告，放在 bss 後 。
- timer queue: 放在heap上，也是唯一使用 heap 的部分，目前尚無 `free()` 的機制，所以記憶體會越用越少 。

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```