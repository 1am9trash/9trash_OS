Lab 3
---

## Exception Handling
- `el2`
  - 開始執行時，默認在`el2`
  - 初始化`bss`(可在`el1`做)
- `el1`
  - 設置exception vector table: 將table address放到`vbar_el1`
  - enable timer
- `el0`
  - enable uart(可在`el1`做)
  - 執行`main()`
- `eret`
  - 不同`el`跳轉需使用`eret`
  - 設置`spsr_elx`: 儲存process status
  - 設置`elr_elx`: 決定跳轉後位置

## Interrupt
- 由於將程式放在`el0`中執行，因此所有需要權限的操作都要透過interrupt提升`el`
- interrupt進入時，會根據exception vector table進入`irq_router()`，並執行對應的handler

## User Program
- user program會被提前編成`img`檔，透過`cpio`放入file system
- 需要執行特定程式時，透過lab2中的`cpio.c`找到其位置並跳轉
- 預設的user program會`svc`三個exception後`ret`

## Concurrent I/O Devices
- 之前的uart是用hard polling的方式運行，浪費大量cpu time，現在改成interrupt trigger
- `uart_putc()`僅會將data存到write queue中，等到非同步的uart interrupt通知可以輸出時，才會真正輸出
- 有uart輸入時，會產生非同步的uart interrupt，將data讀到read queue中，等到`uart_putc()`呼叫時，再從read queue中拿data

## Some Useful Info
- `el2 sp`: `0x80000`
- `el1 sp`: `0x60000`
- `el0 sp`: `0x40000`
- initrd address: `0x8000000`
- heap: 在`linker.ld`宣告，放在bss後 
- timer queue: 放在heap上，也是唯一使用heap的部分，目前尚無`free()`的機制，所以記憶體會越用越少

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```