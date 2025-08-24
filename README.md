# Raspberry Pi Pico Dual CDC serial plus RESET

## Origins

I was looking to implement a project with two CDC serial channels
along with a RESET descriptor. After a bit of searching, I found that [tinyusb](https://github.com/hathach/tinyusb)
(included with pico-sdk) includes a [cdc_dual_ports](https://github.com/hathach/tinyusb/tree/master/examples/device/cdc_dual_ports)
example which sets up the right descriptors.

I took the [usb_descriptors.c](https://github.com/hathach/tinyusb/blob/master/examples/device/cdc_dual_ports/src/usb_descriptors.c) file and tweaked it to work in pico-sdk.

I also copied the [tusb_config.h](https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_stdio_usb/include/tusb_config.h) used in [pico-sdk](https://github.com/raspberrypi/pico-sdk)
and changed `CFG_TUD_CDC` from 1 to 2 (since we want 2 CDC channels).

### Adding the USB Reset interface

[picotool](https://github.com/raspberrypi/picotool.git) will use the USB Reset interface
if it's available. This will put the board into BOOTSEL mode without needing to push
the BOOTSEL button.

I basically needed to add a `TUD_RPI_RESET_DESCRIPTOR` to the descriptor table
and tweaked a few of the strings so that each CDC channel had a separate interface name.

### How to build

For the original Pi Pico (RP2040):
```
cmake -B build-pico
make -C build-pico
```
For the original Pi Pico W (RP2040):
```
cmake -DPICO_BOARD=pico_w -B build-pico-w
make -C build-pico-w
```

### How to flash

You can flash this a variety of ways. I've been using [picotool](https://github.com/raspberrypi/picotool.git)
since it allows me to put the board in BOOTSEL mode, load and run the program all from
a single command:
```
picotool load -x -f build/pico-dual-cdc.uf2
```
picotool will only be able to put the board in BOOTSEL mode if the image that's currently running has support for the RESET descriptor.

### Information

the example creates 2 CDC channels:
```
$ find_port.py -l
USB Serial Device 2e8a:000a with vendor 'Raspberry Pi' serial 'EEEEEEEEEEEEEEEE' intf 'stdio' found @/dev/ttyACM0
USB Serial Device 2e8a:000a with vendor 'Raspberry Pi' serial 'EEEEEEEEEEEEEEEE' intf 'data' found @/dev/ttyACM1
```
[find_port.py](https://github.com/dhylands/dotfiles/blob/master/bin/find_port.py) is a Python program I wrote
for showing USB serial ports that are detected. It requires you to `pip install pyserial`.

Connect a terminal emulator to each serial port (/dev/ttyACM0 and /dev/ttyACM1 in
this example). Anything you type will be echoed on the other one with upper/lower
case swapped.

By default, this program is setup to load and run from RAM, so it will disappear if
you unplug or otherwise reset your board. Because it's running from the RAM, the
flash is disabled and the unique ID comes back as all EE's (which is why the serial
number shows up that way).

### How to enable running from RAM

The Pi Pico has 264K of RAM and the Pico 2 has 520K of RAM. This is lots for running many programs.
This [article](https://kevinboone.me/pico_run_ram.html) gives some useful information about running from RAM.
This line:
```
pico_set_binary_type(${PROJECT} no_flash)
```
in the [CMakeLists.txt](https://github.com/dhylands/pico-multi-cdc/blob/original-working/CMakeLists.txt#L42) file is what makes this a "Run from RAM' program.

When you flash the "run from RAM" version, picotool indicates that it's loading to RAM:
```
$ picotool load -x -f build/usb_own_descriptors_reset.uf2
The device was asked to reboot into BOOTSEL mode so the command can be executed.

Loading into RAM:     [==============================]  100%

The device was rebooted to start the application.
```

### My typical setup

What I like to do is to use [pico-blink](https://github.com/dhylands/pico-blink) which is a modified version of [blink](https://github.com/raspberrypi/pico-examples/tree/master/blink)
from the [pico-examples](https://github.com/raspberrypi/pico-examples). `pico-blink` has the USB RESET feature enabled.

then I can do:
```
cd pico-blink
cmake -B build-pico
make -C build-pico flash-blink
```
and it will compile the blink example as well as flash it.

By having the blink with USB RESET enabled and loaded into flash, a unplug and
replug of the USB cable gets me back to something I can flash using picotool with.

Flashing to RAM means that if I screw things up, a simple power cycle is all it
takes to get back into business.
