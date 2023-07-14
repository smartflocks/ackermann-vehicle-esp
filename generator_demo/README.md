| Tested Targets | ESP32-S3 |
| -------------- | -------- |

# MCPWM RC Servo Control Example

This example illustrates how to drive a typical [RC Servo](https://en.wikipedia.org/wiki/Servo_%28radio_control%29) by sending a PWM signal using the pwm generator component without using the MCPWM driver directly. The PWM pulse has a frequency of 50Hz (period of 20ms), and the active-high time (which controls the rotation) ranges from 0.5s to 2.5ms with 1.5ms always being center of range.

Control Signal:

```

  Minimum 0.5ms  +-+                +-+                +-+         ┌--▲-┐ 
                 | |                | |                | |         |●   | Maximum Left
                 | |                | |                | |         └----┘
        ---------+ +----------------+ +----------------+ +--------
  Neutral 1.5ms  +---+              +---+              +---+       ┌----┐ 
                 |   |              |   |              |   |       |●   ► Center Position
                 |   |              |   |              |   |       └----┘
        ---------+   +--------------+   +--------------+   +------
  Maximum 2.5ms  +-----+            +-----+            +----+      ┌----┐ 
                 |     |            |     |            |    |      |●   | Maximum Right
                 |     |            |     |            |    |      └--▼-┘
        ---------+     +------------+     +------------+    +-----
                  <----------------->
                          20ms
```

## How to Use Example

### Hardware Required

* A development board with any Espressif SoC which features MCPWM peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A RC servo motor, e.g. [SG90](http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf)

Connection :

```
      ESP Board              Servo Motor     5V~6V
+-------------------+     +---------------+    ^
|  SERVO_PULSE_GPIO +-----+PWM        VCC +----+
|                   |     |               |
|               GND +-----+GND            |
+-------------------+     +---------------+
```

Note that, some kind of servo might need a higher current supply than the development board usually can provide. It's recommended to power the servo separately.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project or use the VS Code flash and monitor 🔥

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects or using the [VS Code extention](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/basic_use.md)


## Example Output

Run the example, you will see the following output log:

```
...
I (0) cpu_start: App cpu up.
I (190) cpu_start: Pro cpu start user code
I (190) cpu_start: cpu freq: 160000000 Hz
I (191) cpu_start: Application information:
I (193) cpu_start: Project name:     generator_demo
I (199) cpu_start: App version:      4f1d914-dirty
I (204) cpu_start: Compile time:     Jul 14 2023 09:43:27
I (210) cpu_start: ELF file SHA256:  5a8248f82ad2153d...
I (216) cpu_start: ESP-IDF:          v5.1-dirty
I (222) cpu_start: Min chip rev:     v0.0
I (226) cpu_start: Max chip rev:     v0.99 
I (231) cpu_start: Chip rev:         v0.1
I (236) heap_init: Initializing. RAM available for dynamic allocation:
I (243) heap_init: At 3FC94468 len 000552A8 (340 KiB): DRAM
I (249) heap_init: At 3FCE9710 len 00005724 (21 KiB): STACK/DRAM
I (256) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (262) heap_init: At 600FE010 len 00001FF0 (7 KiB): RTCRAM
I (270) spi_flash: detected chip: generic
I (273) spi_flash: flash io: dio
W (277) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (290) sleep: Configure to isolate all GPIO pins in sleep state
I (297) sleep: Enable automatic switching of GPIO sleep configuration
I (304) app_start: Starting scheduler on CPU0
I (309) app_start: Starting scheduler on CPU1
I (309) main_task: Started on CPU0
I (319) main_task: Calling app_main()
I (319) example: Create timer and operator
I (329) pwm_generator: Connect timer and operator
I (329) pwm_generator: Create comparator and generator from the operator
I (339) gpio: GPIO[1]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (349) pwm_generator: Set generator action on timer and compare event
I (359) pwm_generator: Enable and start timer
I (359) pwm_generator: Timer enable and running
I (369) pwm_generator: Functions register in base
I (369) pwm_generator: Initialization ended
I (379) pwm_generator: Connect timer and operator
I (379) pwm_generator: Create comparator and generator from the operator
I (389) gpio: GPIO[2]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (399) pwm_generator: Set generator action on timer and compare event
I (409) pwm_generator: Enable and start timer
I (409) pwm_generator: Timer enable and running
I (419) pwm_generator: Functions register in base
I (419) pwm_generator: Initialization ended
I (429) example: Starting loop
I (429) example: Dutycycle A: 1500 
I (439) example: Dutycycle B: 1500 
I (459) example: Dutycycle A: 1511 
I (459) example: Dutycycle B: 1488 
I (479) example: Dutycycle A: 1522 
I (479) example: Dutycycle B: 1477 
I (499) example: Dutycycle A: 1533 
I (499) example: Dutycycle B: 1466 
I (519) example: Dutycycle A: 1544 
I (519) example: Dutycycle B: 1455 
I (539) example: Dutycycle A: 1555 
I (539) example: Dutycycle B: 1444 
I (559) example: Dutycycle A: 1566 
I (559) example: Dutycycle B: 1433 
I (579) example: Dutycycle A: 1577 
I (579) example: Dutycycle B: 1422 
I (599) example: Dutycycle A: 1588 
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/smartflocks/ackermann-vehicle-esp/issues) on GitHub.
