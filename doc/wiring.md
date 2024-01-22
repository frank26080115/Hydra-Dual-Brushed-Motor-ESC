# Wiring the Inputs

## For RC Pulse Inputs

The first channel input will always be the main input signal. That usually means the signal wire that came pre-soldered with the ESC, or the traditional signal input solder-pad on the ESC.

For the second channel input, ideally there is a telemetry solder-pad that can be used.

If there is no telemetry solder-pad, then use either the SWDIO pad or the SWCLK pad.

## For CRSF Inputs

If the ESC is using `GPIO PA2` as the main signal input, then the CRSF input is the main signal input. Connect it to your receiver's CRSF output (TX) signal.

If the ESC is NOT using `GPIO PA2` as the main signal input, then the CRSF input can use the telemetry solder-pad.

If there is no telemetry solder-pad, then use the SWCLK pad.

## Configuring Input Modes

The command in CLI is `inputmode <num>` where `<num>` is a number. For example, the command will look like `inputmode 1` to change the input mode to be CRSF.

Here's a table of modes and their numbers:

| MODE             | NUM |
|------------------|-----|
| RC Pulse         |   0 |
| CRSF             |   1 |
| RC using SWDIO   |   2 |
| RC using SWCLK   |   3 |
| CRSF using SWCLK |   4 |

## Wiring the Motors

