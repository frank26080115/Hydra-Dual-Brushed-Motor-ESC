# Windows

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

 * `ENABLE_CEREAL_TX` ability to transmit bytes over serial
 * `ENABLE_LED_BLINK` complex blinking behavior vs simple colours and simple blinks
 * `ENABLE_COMPILE_CLI` if the CLI is available, a ton of memory is used for `printf`

I have fanagled around with a bunch of build options to minimize the final flash size. Also, I have removed all instances of `malloc` and `new`.
