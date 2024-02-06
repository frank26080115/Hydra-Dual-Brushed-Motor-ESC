# Sensor Calibration

Please understand that not all ESCs have voltage sensing or current sensing. If one of these sensors is missing, it might read 0, it might also read crazy noisy values.

You need to do these calibrations in order to use the current limiting feature, and low-battery limiting feature.

Prerequisites:

 * Hydra firmware is installed
 * you [understand how to use CLI mode](configuration.md)
 * you have saved a copy of your original configuration values
 * you already figured out which phase is the common-shared phase
 * a motor (or another load) is connected between the common-shared phase and one of the other two phases
 * you have a multimeter, and understand how to measure voltage and current

## Voltage Sensor Calibration

Calibrating the voltage sensing is easy. Use the command `tunevoltage` in CLI mode to enable the output of the sensor.

Measure the voltage you are using to power the ESC with a multimeter.

On you screen, you will start seeing data that looks like `[32400] raw 1234 , calc 13324 , vdiv 110`

The number in the `[...]` is just a millisecond timestamp, ignore it.

The `raw` value is the ADC raw reading, ignore it.

The `calc` value is what is most important. If you are powering the ESC with 12 volts, then this number should read 12000. The unit is millivolts.

The `vdiv` value is what we need to adjust.

With your keyboard, pressing keys into the serial terminal will adjust the `vdiv` value. Adjust it until the `calc` value is as close to what it is supposed to be as possible. Remember the `vdiv` value as you do this.

| key stroke | action |
|------------|--------|
| `,`        | adjust **down** by 1 |
| `.`        | adjust **up** by 1 |
| `;`        | adjust **down** by 10 |
| `'`        | adjust **up** by 10 |
| `ESC` or `ENTER` <br /> or `BACKSPACE` or `DELETE` <br /> or `x` | stop and cancel |
| `s`        | stop and save result |

Once you've stopped, the final `vdiv` value will be displayed, this is the number that should be set as `voltdiv` in the configuration. If you've pressed the `s` key, the setting should have been saved for you.

## Electrical Current Sensor Calibration

Wire up your multimeter to measure the electrical current. Please select the correct measurement range, and connect the probes properly. Typically, a multimeter has a different connector for measuring high currents.

![](imgs/measurecurrent.png)

There are other ways of doing this wiring, such as using a power supply with a current read-out, or using a battery watt-meter in between the battery and ESC.

During the test, you can turn on or turn off the common-shared phase. The other two phases are always grounded. This allows you to attach a test load to the ESC. This could be a low-resistance-high-power resistor, or a constant-current dummy load, or a motor. Just don't blow up the ESC.

Use the command `tunecurrent` in CLI mode to enable the output of the sensor.

On you screen, you will start seeing data that looks like `[32400] raw 1234 , calc 13324 , offset 0 , scale 20`

The number in the `[...]` is just a millisecond timestamp, ignore it.

The `raw` value is the ADC raw reading, ignore it.

The `calc` value is what is most important. If there is 2A of current being drawn (read the as you do this), then this number should read 2000. The unit is milliamps.

The `offset` value and `scale` value are the calibration parameters that we need to adjust.

With your keyboard, pressing keys into the serial terminal will adjust the calibration parameters. Adjust them until the `calc` value is as close to what it is supposed to be as possible (read your meter as you do this).

Before applying the load, adjust the `offset` such that the `calc` value is close to zero when there is no load, then move on to applying the load and adjusting `scale`.

| key stroke | action |
|------------|--------|
| `,`        | adjust `offset` **down** by 1 |
| `.`        | adjust `offset` **up** by 1 |
| `<`        | adjust `offset` **down** by 10 |
| `>`        | adjust `offset` **up** by 10 |
| `;`        | adjust `scale` **down** by 1 |
| `'`        | adjust `scale` **up** by 1 |
| `:`        | adjust `scale` **down** by 10 |
| `"`        | adjust `scale` **up** by 10 |
| `1` - '9'  | turn ON test load with selected power level |
| `0`        | turn OFF test load |
| `ESC` or `ENTER` <br /> or `BACKSPACE` or `DELETE` <br /> or `x` | stop and cancel |
| `s`        | stop and save result |

Once you've stopped, the final calibration parameters will be displayed, these are the number that should be set as `curroffset` and `currscale` in the configuration. If you've pressed the `s` key, the setting should have been saved for you.

## Current Limiting Tuning

The current limit value is set in milliamps. If you want the current limit to be set at 10A, then the value `currlim` should be configured to 10000.

The default settings should have a `curlimkp` (current limit PID loop's kP) value of around 100 by default. If the current draw is 1A over the limit, this should cut the power completely in under half of a second. It's not a hard sharp power cut, the PID loop will gradually lower the voltage during this time. Simulation shows that the current should reach a steady state in around 120 milliseconds, but at a current slightly higher than the specified limit (for example, a current limit set at 10A will settle at 10.5A).

If you want to operate at the current limit frequently or for long durations, then lower `curlimkp` and also have a slightly lower `currlim`.
