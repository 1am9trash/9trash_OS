Lab 1
---

## Useful reference
- [ARM Peripherals](https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf)

## Uart
- Bus address (Device base): `0x7ennnnnn`
- Physical address: `0x3fnnnnnn`

## Mailbox
- Mailbox tag buffer
  - 0-3: tag id
  - 4-7: value buffer size, which is maximum of request and response value buffer's length
  - 8-11: bit 31 for request(0) or response(1), other bits for length
  - 12-19: value buffer
- [Mailbox Structure](https://jsandler18.github.io/extra/prop-channel.html)
- [Mailbox Interface](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface)

## Makefile
```sh
make run    # run code with qemu
make debug  # run code with gdb
make build  # build kernel8.img, may not be called directed
make clean  # delete build folder
```