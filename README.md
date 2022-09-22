Lab 2
---

## Initial Ramdisk
- 可透過`cpio`指令將folder壓縮成file system的格式，導入機器，再行處理
- "TRAILER!!!"為最後一個檔案的名稱，遇到代表遍歷到結尾
- qemu中，initrd被放在`0x8000000`的位置上，後續處理好Device Tree後，可從dtb中讀取initrd的位置，不必寫死

## Simple Allocator
- 簡易的`malloc`，一開始切割一塊記憶體後續用來分配
- 可驗證之前`reboot`指令的正確性

## Device Tree
- dtb的address預設會被存在x0中，由於start.s執行時，會將x0複寫掉，因此需要先將dtb的address load到其他一塊記憶體，方便讀取
- dtb中，為32 bit的big endian，需要轉換
- dtb的node中，name不為空字串，最少會補到4個byte，而prop則允許空字串，但如果長度不為4的倍數，都會補到4的倍數
- FDT_NOP是留來修改device tree的，忽略即可
- [Device Tree Structure](http://www.wowotech.net/device_model/dt-code-file-struct-parse.html)
- [Device Tree傳遞](https://blog.csdn.net/huang987246510/article/details/108189218)

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```