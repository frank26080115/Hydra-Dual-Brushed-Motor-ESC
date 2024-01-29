# Windows

(for some reason that I can't figure out, builds on Windows seem to generate a smaller firmware binary than when built on Ubuntu with the same version of `arm-none-eabi-gcc`)

## Installing ARM Toolchain

Download installer from https://developer.arm.com/downloads/-/gnu-rm

Install it to the path `C:\ProgramFiles\GNUArmEmbeddedToolchain\10.2021.10` (the version might be different), check the checkbox "Add path to environmental variable"

(if it is not added to the system `PATH`, then add `C:\ProgramFiles\GNUArmEmbeddedToolchain\10.2021.10\bin` to your system `PATH` manually)

## Installing Make

If you don't already have a copy of `make`, then the minimal way to get it is to download it from https://packages.msys2.org/package/make where it says `File:
https:// ...`

Extract it, rename the `usr` directory to `msys-make`, and place it in `C:\ProgramFiles\msys-make`, and add `C:\ProgramFiles\msys-make\bin` to your system `PATH`

# Ubuntu  (or Ubuntu for Windows)

Make sure you have the tools installed with `sudo apt-get install build-essentials gcc-arm-embedded`

# Running a Build

Open a new terminal or command line prompt. (a new instance makes sure that the latest changes to `PATH` is effective)

Use `cd` to navigate to the code repository.

Use the command `make clean` first. Then use the command `make`, which should generate all of the firmware files.

For a single target, the command looks like `make -s MCU_TYPE=<MCU> TARGET=<TARGETNAME> binary` , for example, `make -s MCU_TYPE=F051 TARGET=REPEAT_DRIVE_F051 binary`

Batch build will be slow as all the files need to be completely rebuilt for every ESC target. Individual targets can be rebuilt quickly without starting from a clean state.

# Modifying a Build

Compiler preprocessor defines that can be enabled or disabled to save memory:

 * `ENABLE_LED_BLINK` complex blinking behavior vs simple colours and simple blinks
 * `ENABLE_COMPILE_CLI` if the CLI is available, a ton of memory is used for `printf`

I have fanagled around with a bunch of build options to minimize the final flash size. Also, I have removed all instances of `malloc` and `new`.

The targets named `STM32F051DISCO` or `STM32G071NUCLEO` are meant for the official ST development boards with built-in debuggers. The firmware will instantiate a dedicated UART for debugging-printf and CLI. The AM32 bootloader is not required for the development boards as the linker script has a different flash starting address.

Helpful debugging constants:

 * `DEBUG_PRINT` will activate `dbg_printf` statements throughout the code
 * `DEBUG_EVENTCNT` will activate code that tracks how many times a particular event has occured. Get these counts using `uint32_t dbg_evntcnt_get(uint8_t id)` where `id` is one of the `DBGEVNTID_` enum items. See `debug_tools.h` for a list
 * `DEBUG_PINTOGGLE` will cause some pins to change state on certain events, useful for debugging timing using a logic analyzer attached to those pins.

# Using IDE

There is a `.cproject` file available to build the development board firmwares, this can be opened with STM32CubeIDE. This is useful for using the ST-Link debugger to debug the code. It is not meant for building actual ESC firmware.

There are some `*.launch_` files available inside the `src-mcu` directory. Rename these to `*.launch` and then they can be imported into STM32CubeIDE as debugger configurations.
