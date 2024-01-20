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

(currently the build requires a clean build always, sorry!)
