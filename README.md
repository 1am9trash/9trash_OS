9trash OS
---

A simple operating system from scratch runs on Raspberry Pi 3 Model B. \
Self-learning project based on [NYCU OSDI Spring 2022 course](https://oscapstone.github.io/).

## Document
- [Build Environment](document/environment.md)
- [Run qemu with debugger](document/debugger.md)

## Schedule
- I started this project in 2022 summer vocation. I plan to finish it in 2023 winter vocation.
- The source code may be found in lab branchs.

  | Lab | Main Content | Finish Date | Intro |
  | ---- | ---- | ---- | --- |
  | 0 | environment setup | 2022/06/30 | 1. simple linker.ld and start.s<br>2. Makefile structure |
  | 1 | UART & simple shell | 2022/07/15 | 1. hard-polling uart<br>2. mailbox to get board info |
  | 2 | filesystem(CPIO) & Device Tree | 2022/08/01 | 1. initial ramdisk with cpio<br>2. dtb handler |
  | 3 | exception & interrupt | 2023/03/20 | 1. run main program in el0<br>2. handle exception and interrupt to el1<br>3. load user program image<br>4. interrupt-trigger uart driver |