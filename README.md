Dual brushed motor ESC compatible with brushless motor ESCs

**This project is a work in progress, nothing is done yet!!!**

This is a replacement firmware for brushless motor ESCs that will convert them to being brushed motor ESCs that can independently drive two brushed motors. This is advantageous because brushless motor ESCs are usually smaller and cheaper, when compared to buying dedicated dual motor brushed motor ESCs.

This firmware must be installed after AM32 is installed on the ESC, as the installation needs to be done through the AM32 bootloader.

To control the ESC, RC pulse signals (PWM) are supported, and also the CRSF serial protocol is supported.

To configure the ESC, there is a command-line-interface (CLI) implemented, where various commands are used to manipulate stored settings.

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

| Name | Brushless Rating | Brushed Rating 1 Motor | Brushed Rating 2 Motors | Max Voltage | URL |
|------|------------------|------------------------|-------------------------|-------------|-----|
| Repeat Robotics AM32 35A | 35A | 15A | 7A | 6S | https://repeat-robotics.com/buy/am32/ |
| Repeat Robotics AM32 NeutronRC 70A | 70A | 30A | 15A | 8S | https://repeat-robotics.com/buy/neutronrc-70a-g071-beetle-weapon-esc/ |
| Just Cuz Robotics Pariah 70A AM32 | 70A | 30A | 15A | 6S | https://justcuzrobotics.com/products/pariah-70a-am32-weapon-esc |
| SEQURE 70A with AM32 | 70A | 30A | 15A | 6S | https://www.amazon.com/SEQURE-Brushless-Electric-Controller-Airplanes/dp/B0CN6NB8CB/ |

The EMAX Formula 45A ([official link](https://emax-usa.com/collections/electronic-speed-controller/products/emax-formula-series-45a-esc-support-blheli-32-2-5s), [store link](https://pyrodrone.com/collections/individual/products/emax-formula-series-45a-esc-support-blheli_32-2-5s)) will get full support but only with CRSF input mode, just because I own them and I do want to use them since they are small. This does not come with AM32 preinstalled and will require advanced tools to install AM32.

Other models potentially will work if they meet the following criteria:

 * Has the AM32 bootloader
 * Has enough exposed signal pads to support the desired input mode
 * Has the microcontroller STM32F051 or STM32G071

To install, update, and configure Hydra firmware, you must use a USB-linker meant for communicating with ESCs. See this [wiki page](../../wiki/USB-Linker) for a list of linkers you can use.

# Instructions

 * [Install Firmware](doc/install-firmware.md)
 * [Configuration](doc/configuration.md)
 * [Build Instructions](doc/build-instructions.md)
 * [Install AM32](doc/install-am32.md)