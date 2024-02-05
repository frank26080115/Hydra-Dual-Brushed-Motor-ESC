# Direct PWM Mode

Direct PWM mode is useful for when you just need to remotely drive the MOSFETs to provide voltage somehow. For example, if you want to control LEDs, maybe even a long RGB LED strip using a RC radio transmitter. There are three outputs on the ESC that can be independantly controlled, so for a RGB LED strip, each colour can be individually controlled. There are other applications as well.

You do need to first understand [how to configure Hydra firmware (click here)](configuration.md)

First step is to configure the `vsplitmode` to `3`.

Then, configure the `rc_mid` value to about `1000`, this should be the lowest throttle value your RC radio transmitter can send. If you are using CRSF, then just set it to `1000`.

Then configure the `rc_range` value to about `1000`, this added to `rc_mid` should be the highest throttle value your RC radio transmitter can send (about 2000). If you are using CRSF, then just set it to `1000`.

Then configure `rc_deadzone` to your desired value, but `0` is recommended.

If you are using CRSF input mode, then `channel_mode` will control the 3rd output. In RC PWM pulse input mode, only the first two outputs can be used.

Then configure `dirpwm_1` and `dirpwm_2` and `dirpwm_3`, each one corresponds to an output. The values you can set are:

| value | meaning |
|-------|---------|
| 0     | disabled, floating |
| 1     | push-pull mode, complementary PWM is being used, both the low-side MOSFETs and high-side MOSFETs are used in coordination |
| 2     | high mode, only the high-side MOSFETs are used |
| 3     | open-drain mode, only the low-side MOSFETs are used |

Some caveats:

 * in RC PWM pulse input mode, only the first two outputs can be used, to use all three channels, you need to use CRSF input mode
 * arming is still required, that's why the `rc_*` values need to be edited, so that arming happens at lowest throttle instead of middle throttle
 * `phasemap` is not being used and will have no effect, you need to figure out which output is which solder-pad on the ESC manually with a multimeter
 * `chanswap`, `flip1`, and `flip2` will remain in effect, I recommend these all be turned off
 * current limiting, low-voltage limiting, and temperature limiting, are all still in effect if you have them enabled
 * `loadbal` is ignored and will have no effect
