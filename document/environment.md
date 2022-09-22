Environment Setup
---

#### Qemu: I built on wsl2. It may be easier in ubuntu VM.
- Install dependency
  ```sh
  sudo apt update
  sudo apt install build-essential
  sudo apt install zlib1g-dev
  sudo apt install pkg-config
  sudo apt install libglib2.0-dev  
  sudo apt install binutils-dev
  sudo apt install libboost-all-dev
  sudo apt install autoconf
  sudo apt install libtool
  sudo apt install libssl-dev
  sudo apt install libpixman-1-dev
  sudo apt install virtualenv
  sudo apt install flex
  sudo apt install bison
  sudo apt install libgcrypt20-dev
  ```
- Install ninja
  ```sh
  # make sure python version >= 3.6
  sudo apt install ninja-build
  ```
- Build Qemu
  - Follow the [website](https://www.qemu.org/download/) to build latest qemu.

### Tools in aarch64 linux environment
- Install tools
  ```sh
  sudo apt-get install gcc-aarch64-linux-gnu
  ```

### Debugger
- Install cross platform debugger
  ```sh
  sudo apt install gdb-multiarch
  ```
- Install gef plugin
  - It's not necessary.
  - Install by following the [website](https://github.com/hugsy/gef).
