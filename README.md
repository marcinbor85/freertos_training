# Environment

- update database
```bash
sudo apt update
```

- install developer tools
```bash
sudo apt install gcc arm-gcc-none-eabi git -y
```

- install stm32 flash tools
```bash
sudo apt install st-flash -y
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

