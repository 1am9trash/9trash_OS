Lab 2
---

## Target
- 了解 initial ramdisk 跟 devicetree 。
- 完成 simple allocator 。

## Initial Ramdisk
- 使用說明
  - 透過 `cpio` 指令將 folder 壓縮成 file system 的格式，導入機器。
  - parse cpio 檔案時， "TRAILER!!!" 為最後一個檔案的名稱，遇到則結束。
  - qemu 中， initrd 預設被放在 `0x8000000` 的位置上，後續處理好 Device Tree 後，可從 dtb 中讀取 initrd 的位置，不必 hard code 。

## Simple Allocator
- 簡易的 `malloc` ，一開始切割一塊記憶體後續用來分配。
- 可驗證之前 `reboot` 指令的正確性。

## Device Tree
- device tree 是用來說明硬體 components property 的資料結構，使 OS 可以正確使用並管理 components ，而不需要將 base address 等資料 hard code 在 kernel 當中。
- 使用說明
  - dtb 的 address 預設會被存在 `x0` 中，由於 `start.s` 執行時，會將 `x0` 複寫掉，因此需要先將 dtb 的 address 存到其他位置，方便讀取。
  - dtb 中，為 32 bit 的 big endian ，需要轉換。
  - dtb 的 node 中， name 不為空字串，最少會補到 4 個 byte ，而 prop 則允許空字串，但如果長度不為 4 的倍數，都會補到 4 的倍數。
  - `FDT_NOP` 是留來修改 device tree 的，忽略即可。
- Reference
  - [Device Tree Structure](http://www.wowotech.net/device_model/dt-code-file-struct-parse.html)
  - [Device Tree 傳遞](https://blog.csdn.net/huang987246510/article/details/108189218)

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```