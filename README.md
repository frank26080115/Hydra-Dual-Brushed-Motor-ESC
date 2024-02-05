Dual brushed motor ESC compatible with brushless motor ESCs

**This project is a work in progress, nothing is done yet!!!**

This is a replacement firmware for brushless motor ESCs that will convert them to being brushed motor ESCs that can independently drive two brushed motors. This is advantageous because brushless motor ESCs are usually smaller and cheaper, when compared to buying dedicated dual motor brushed motor ESCs.

This firmware must be installed after AM32 is installed on the ESC, as the installation needs to be done through the AM32 bootloader.

To control the ESC, RC pulse signals (PWM) are supported, and also the CRSF serial protocol is supported.

To configure the ESC, there is a command-line-interface (CLI) implemented, where various commands are used to manipulate stored settings.

(other features: current limiting, temperature limiting, low-battery limiting)

# Principle of Operation

Brushless motor ESCs have 6 MOSFETs on them to form 3 half-bridges. These drive the three windings inside a brushless motor. Each half-bridge is driven by a PWM signal, which can approximate a voltage.

![](doc/imgs/brushless_halfbridges.png)

Instead of connecting a single brushless motor, we connect two brushed motors. By changing the voltage (changing the PWM signal's duty cycle) of each half-bridge, we can control each of these two brushed motors independently.

![](doc/imgs/control_motor_directions.png)

Most ESCs only have one signal input for control. This hack can only be used with ESCs that have a second signal input, or ESCs that can take serial multi-channel-combined protocols.

# Caveats

Depending on which direction the two motors spin, the motors can get either up to full voltage, or half voltage. If the motors are used to drive a tank-style robot, then it can drive forward and backwards really fast with full battery voltage, but the speed when it turns will be slower as it can only use half of the battery voltage. There are three ways that the ESC can mix the voltage, explained by this diagram:

![](doc/imgs/operating_voltage_modes.png)

The completely linear half-voltage mode might make driving more predictable, but it is wasting potential performance. The other two modes will have a slight non-linear feeling as the robot begins to turn, but this can be resolved with another curve set on the transmitter.

Also, when used for brushless motors, each MOSFET is only turned on for a brief moment, all of the MOSFET share the same load. But when used for brushless motors, it is possible for one MOSFET to be on all of the time while another one is turned off all of the time, and the shared-common half-bridge passes more current through it than the other MOSFETs. For these reasons, the effective new current handling rating of the ESC is about 20% of the original ESC rating. If you purchased a ESC rated for 45A, then you should only use motors that require up to 9A each.

# Supported Hardware

The hardware that are fully supported are the ones I personally own and use

| Name | Brushless Rating | Brushed Rating<br />1x Motor | Brushed Rating<br />2x Motors | Max Voltage | URL |
|------|------------------|------------------------------|-------------------------------|-------------|-----|
| Repeat Robotics AM32 35A | 35A | 15A | 7A | 6S | https://repeat-robotics.com/buy/am32/ |
| Repeat Robotics AM32 NeutronRC | 70A | 30A | 15A | 8S | https://repeat-robotics.com/buy/neutronrc-70a-g071-beetle-weapon-esc/ |
| Repeat Robotics Dual Drive | ? | ? | ? | 6S | https://repeat-robotics.com/buy/repeat-am32-dual-brushless-drive-esc/ |
| Just Cuz Robotics Pariah 70A | 70A | 30A | 15A | 6S | https://justcuzrobotics.com/products/pariah-70a-am32-weapon-esc |
| Just Cuz Robotics 35A AM32 | 35A | 15A | 7A | 6S | https://justcuzrobotics.com/products/jcr-35a-am32 |
| Ranglebox Mars AM32 35A | 35A | 15A | 7A | 5S | https://ranglebox.com/shop/product/mars-am32-controller/ |
| SEQURE (must select AM32 option) | ? | ? | ? | ? | https://sequremall.com/collections/sqesc |

The EMAX Formula 45A ([official link](https://emax-usa.com/collections/electronic-speed-controller/products/emax-formula-series-45a-esc-support-blheli-32-2-5s), [store link](https://pyrodrone.com/collections/individual/products/emax-formula-series-45a-esc-support-blheli_32-2-5s)) will get full support because I own them and I do want to use them since they are small. This does not come with AM32 preinstalled and will require advanced tools to install AM32. (this EMAX model can only support CRSF input)

Other models potentially will work if they meet the following criteria:

 * Has the AM32 bootloader
 * Has enough exposed signal pads to support the desired input mode
 * Has the microcontroller STM32F051, STM32G071, and AT32F421

[Here's a full guide on using other hardware not on the list](doc/other-hardware-hacking.md)

To install, update, and configure Hydra firmware, you must use a USB-linker meant for communicating with ESCs. These USB-linkers are used with AM32, BLHeli_32, and BLHeli_S, for configuration and firmware updating. See this [wiki page](../../wiki/USB-Linker) for a list of USB-linkers you can use.

# Instructions
 
 * [Wiring](doc/wiring.md)
 * [Install Firmware](doc/install-firmware.md)
 * [Configuration](doc/configuration.md)

Once you've done all of the above, it's ready to use as any other dual ESC.

If arming is enabled (by default, it is enabled), then the motors won't start moving until the signal is indicating center throttle (center stick position), meaning stop. This is to prevent the motor from moving unexpectedly.

The ESC is always commanded in bi-directional mode. Braking mode is enabled by default and is prefered for better responsiveness.

During operation, if there is a LED, then the LED indicates the ESC's state:

| state            | LED single colour | LED multi-colour |
|------------------|-------------------|------------------|
| disarmed         | ![](doc/imgs/ledblinks/redsolid.png) | ![](doc/imgs/ledblinks/redsolid.png) |
| armed <br /> not moving | ![](doc/imgs/ledblinks/shortslow.png) | ![](doc/imgs/ledblinks/shortslowgreen.png) |
| armed and moving | ![](doc/imgs/ledblinks/shortfast.png) | ![](doc/imgs/ledblinks/shortfastgreen.png) |
| current/temperature limit reached | ![](doc/imgs/ledblinks/currentlimit_s.png) | ![](doc/imgs/ledblinks/currentlimit_m.png) |
| low battery | ![](doc/imgs/ledblinks/lowbatt_s.png) | ![](doc/imgs/ledblinks/lowbatt_m.png) |

(it is not possible to accidentally enter CLI mode, as it checks for input signal pulses)

There is no "arcade mode mixing" feature in this firmware, please do this from the transmitter side. I always recommend avoiding using any mixing modes on ESCs, it is much better to do it on the transmitter.

# Additional Advanced Topics

 * [Firmware Build Instructions](doc/build-instructions.md)
 * [Install AM32](doc/install-am32.md)
 * [Developer Commentary](doc/developer-commentary.md)
 * [Direct-PWM Mode](doc/direct-pwm-mode.md) for directly controlling the MOSFETs, useful for remotely controlling LEDs and other loads

# This still seems expensive

There is another hacky ESC project that can turn cheap BLHeli_S brushless ESCs into brushed motor ESCs. It's called [Greenjay](https://github.com/frank26080115/greenjay) firmware. Each individual ESC is small and inexpensive.
