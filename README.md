Lab 3.5
---

> 在 Lab 3 中，了解 el 概念，實現 exception 跟 interrupt 處理，但後續發現有部分的處理是有問題的，因此 Lab 3.5 目的在於解決這些問題。

## Concurrent I/O Devices
- 原實作 
  - Lab 3 中將 uart 從 hard polling 改成 interrupt trigger 。
  - 概念是在 uart 發出 interrupt 時，由 exception table 中的 irq handler 捕獲，再交由 uart read/write 內容。
- 問題
  - irq exception 有許多種，不單單是 uart interrupt ，理論上應該檢查 register 值，判斷來源再對應處理，但這裡我直接把所有的訊號都當成 uart interrupt 處理，這是第一個問題。
  - 事實上，這裡我還犯了另一個錯，我並沒有把 uart interrupt 打開，因此 uart 是不會發 interrupt 的，但由於我沒有檢查訊號來源，因此 uart 還是會定期被 trigger (如被 timer interrupt trigger)，因此表現反而可以正常 read/write 。
- 解決方法
  - enable/disable interrupt: 為了能收到 uart interrupt ，要開啟 interrupt 。
    ```c
    void enable_interrupt() {
        asm volatile("msr DAIFClr, 0xf");
    }
    
    void disable_interrupt() {
        asm volatile("msr DAIFSet, 0xf");
    }
    ```
  - irq handler: 根據 `IRQ_PENDING_1` 判斷訊號種類，再行處理。
    ```c
    // check whether the interrupt is triggered by mini UART or not
    if (*IRQ_PENDING_1 & (1 << 29))
        uart_handler();
    ```

## SVC
- 問題
  - 在 Lab 3 中，我們會透過 `svc` 測試 exception handler 是否正常運作。
  - 理論上， `svc` 在跳轉前，會將 `elr` (exception linking register) 設為當前地址，方便在處理完 exception ，透過 `eret` 跳轉回 `elr` 繼續執行。
  - 在 Lab 3 中，這是正常執行的，但後續在 exception handler 加上檢查 exception level 的 code 後，發現 `elr` 會被更改，這代表這個值需要自己 handle ，系統不保證保留原值。
- 解決方法
  - 在 handler 執行前 `save_all` 儲存 register ，執行後 `load_all` 回復原狀。除了 general register 外，另外儲存 `elr` 跟 `spsr` 。
    ```armasm
    .macro save_all
      sub sp, sp, 16 * 17
      // ...
      mrs x22, elr_el1
      mrs x23, spsr_el1
      stp x22, x23, [sp, #16 * 16]
    .endm

    .macro load_all
      ldp x22, x23, [sp, #16 * 16]
      msr elr_el1, x22
      msr spsr_el1, x23
      // ...
      add sp, sp, 16 * 17
    .endm
    ```

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```