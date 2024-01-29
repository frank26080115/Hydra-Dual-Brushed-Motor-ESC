The CLI (command line interface) is how the user can configure the ESC, and also perform some simple tests.

# Getting into the CLI

## Required: USB Linker

To install, update, and configure Hydra firmware, you must use a USB-linker meant for communicating with ESCs.

See this [wiki page](../../../wiki/USB-Linker) for a list of linkers you can use. That page also has links to the appropriate USB drivers that you might need to install.

## Required: Serial Terminal App

You need a simple serial terminal app of some sort installed on your computer.

For Windows, some of the popular choices: [Tera Term](https://tera-term.en.softonic.com/), [PuTTY](https://putty.org/), [RealTerm](https://realterm.sourceforge.io/), Arduino IDE's built-in serial monitor, some of the extensions available in VSCode

For Linux, perhaps try: `Minicom`, `picocom`, `screen`, `gtkterm`

Please remember to configure the serial port for 19200 baud rate, 8 bits, no parity.

SparkFun Electronics has a [tutorial about serial terminals](https://learn.sparkfun.com/tutorials/terminal-basics/real-term-windows)

## Connecting the USB linker

These steps are different from installing the firmware. Please do not confuse the two different instructions.

 1. Make sure the ESC is disconnected from everything. Do not connect the USB linker to it just yet. Keep the power off.

 2. Power on the ESC. The LED should indicate that it is disarmed.

 3. Wait 10 seconds. After 10 seconds have passed, the LED blinking should have changed to a different pattern.

 4. Connect the USB linker to the computer

 5. Connect the USB linker to the ESC

 6. Run the serial terminal app

Send the enter keystroke into the terminal a few times and you should see `>` appear on your screen. This means you are ready to enter in a command.

NOTE: arrow keys will never work, the delete or backspace key will simply cancel the entire command

NOTE: the CLI will never exit until you reboot the entire ESC with the USB linker disconnected

# Using the CLI

## Reading current settings

The command is `list`

It will output a giant list of all of the settings.

You can copy this block of text into a text file for safekeeping.

(there's a section later describing each of the setting items)

## Changing a setting

The command looks like `<setting-name> <value>`, for example, `inputmode 1`.

A confirmation will shown if you are successful.

If you pasted in multiple lines, it will change multiple settings all at once. So you may paste in the text file you saved earlier.

WARNING: please don't go crazy with the values. The CLI is only smart enough to know if you forgot to include a value, or if you made a spelling mistake, but it cannot actually check if your value is within the correct range.

NOTE: for settings that are either true or false, please use `1` or `0`

(there's a section later describing each of the setting items)

## Factory Reset

The command is `factoryreset`

This will change every setting back to their default values.

## Reboot

The command is `reboot`

This will cause the ESC to enter into the AM32 bootloader.

# Available Settings and What They Mean

TODO write this section

# Advanced Commands

TODO write this section
