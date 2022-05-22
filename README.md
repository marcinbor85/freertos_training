# Environment

- install common tools
```bash
sudo apt-get install git make cmake libusb-1.0-0-dev putty
```

- install linux developer tools
```bash
sudo apt-get install gcc build-essential gcc-arm-none-eabi
```

- install arm developer tools
```bash
sudo apt-get install gcc-arm-none-eabi
```

- install stm32 flash tools
```bash
git clone https://github.com/stlink-org/stlink
cd stlink
cmake .
make
sudo make install
sudo cp lib/*.so* /lib32
sudo cp lib/*.so* /lib
```

# Download

- download repository
```
git clone --recursive https://github.com/marcinbor85/freertos_training.git
```

# Build

- compile and build
```bash
cd freertos_training
make -j
```

