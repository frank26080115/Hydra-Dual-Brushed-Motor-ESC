# Direct PWM Mode

Direct PWM mode is useful for when you just need to remotely drive the MOSFETs to provide voltage somehow. For example, if you want to control LEDs, maybe even a long RGB LED strip using a RC radio transmitter. There are three outputs on the ESC that can be independently controlled, so for a RGB LED strip, each colour can be individually controlled. There are other applications as well.

You do need to first understand [how to configure Hydra firmware (click here)](configuration.md)

First step is to configure the `vsplitmode` to `3`.

If you are using CRSF input mode, then `channel_mode` will control the 3rd output. In RC PWM pulse input mode, only the first two outputs can be used.

Then configure `dirpwm_1` and `dirpwm_2` and `dirpwm_3`, each one corresponds to an output. The values you can set are:

| value | meaning |
|-------|---------|
| 0     | disabled, floating |
| 1     | push-pull mode, complementary PWM is being used, both the low-side MOSFETs and high-side MOSFETs are used in coordination |
| 2     | high mode, only the high-side MOSFETs are used |
| 3     | open-drain mode, only the low-side MOSFETs are used |

(NOTE: if the ESC uses a fancy MOSFET driver with an EN pin, then mode 2 and 3 won't work, but these ESCs don't actually exist as far as I know)

A channel pulse input of 1500 microseconds, or the CRSF equivalent, will result in an output of 0, the output will be off. The absolute value of the normalized input is used, so that means 1000 microseconds is the same as 2000 microseconds.

Some caveats:

 * in RC PWM pulse input mode, only the first two outputs can be used, to use all three channels, you need to use CRSF input mode
 * `phasemap` is not being used and will have no effect, you need to figure out which output is which solder-pad on the ESC manually with a multimeter
 * arming is still required
 * `chanswap` will remain in effect, but will not affect the 3rd channel
 * current limiting, low-voltage limiting, and temperature limiting, are all still in effect if you have them enabled
 * `loadbal` is ignored and will have no effect
 * `tankmix` must be off, or else it will behave weird
