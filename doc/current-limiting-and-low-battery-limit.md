Prerequisites:

 * Hydra firmware is installed
 * you [understand how to use CLI mode](configuration.md)
 * you [have performed sensor calibration](sensor-calibration.md)

## Current Limit

The current limit value is set in milliamps. If you want the current limit to be set at 10A, then the value `currlim` should be configured to 10000.

The default settings should have a `curlimkp` (current limit PID loop's kP) value of around 100 by default. If the current draw is 1A over the limit, this should cut the power completely in under half of a second. It's not a hard sharp power cut, the PID loop will gradually lower the voltage during this time. Simulation shows that the current should reach a steady state in around 120 milliseconds, but at a current slightly higher than the specified limit (for example, a current limit set at 10A will settle at 10.5A).

If you want to operate at the current limit frequently or for long durations, then lower `curlimkp` and also have a slightly lower `currlim`.

## Low Battery Voltage Limit

The ESC can be configured to detect low battery in different ways, and react in different ways.

#### Absolute Voltage Limiting

This means the voltage limiting is active when the detected voltage is below a voltage threshold you set. The threshold value is the `voltlim` value specified in millivolts. To use this method, keep the value of `cellmaxvolt` at `0`.

#### Per-Cell Voltage Limiting

This means the ESC will try and guess how many battery cells are being used, and automatically determine the low battery voltage limit. You need to specify the limit as `voltlim` in millivolts-per-cell, and also specify each cell's maximum voltage with `cellmaxvolt`. For example, regular Li-ion and Li-poly batteries should use `cellmaxvolt` set to 4200, and Li-HV batteries should use `cellmaxvolt` set at 4350. `voltlim` should be set at about 3000, but you can raise or lower this according to preference.

WARNING: This automatic-cell-counting feature only works well if your battery is not already dead. For example, if you used a 6S battery that's so dead that it is at 18V, the firmware might think it is a 5S battery instead, since a full 5S battery is 21V.

It is recommended to use the absolute voltage limit method instead.

#### Immediate Stop

Set `lowbattstretch` to `0` and the ESC will completely stop if the measured voltage goes below the voltage limit.

#### Slow Down Gradually

Set `lowbattstretch` to a very high number, such as 1000. For example, if the voltage limit is at 6000 millivolts, and the `lowbattstretch` is set to 1000, then it means when the ESC measures 6000 millivolts from the battery, it will begin to slow down, and when battery voltage reaches 5000 millivolts, the ESC will have come to a complete stop. If you want to use this feature, you will want a slightly higher voltage limit, so that the motors slow down earlier.

This is useful in a competitive environment, where you want to sacrifice the health of your battery to win. This is also how RC boat ESCs typically behave, it makes the boat slower but you'll be able to safely bring home the boat.

## Temperature Limit

This feature is used if `templim` is set in degrees-Celcius. It is not very useful because it is using the temperature of the microcontroller, not the temperature of the MOSFETs. Setting `templim` to about 80 might be an acceptable value.
