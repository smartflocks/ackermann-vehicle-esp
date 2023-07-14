| Tested Targets | ESP32-S3 |
| -------------- | -------- |

# PWM Hook


The hook task works with the capture and the generator module avoid issues with as the 60 Seconds limit in the timestamp of a capture event in this example we connect a RC receiver with a rc servo and add filtering and logging to the capture event before sending the commands to the servo to better grasp the capture and the generator modules please see the demo app for each one.



## How to Use Example

### Hardware Required

* A development board with any Espressif SoC which features MCPWM peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A RC servo motor, e.g. [SG90](http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf)
* An RC receiver module

Connection :

```
       RC receiver                         ESP Board      
        +------+              +---------------------------------+
        |      |              |                                 |
        + PWM  +----=====>----+ GPIO13 (internal pull up)       |
        |      |              |                                 |
        |  VCC +--------------+ 5V                              |       Servo Motor      5V~6V
        |      |              |                                 |     +---------------+    ^
+-------+  GND +--------------+ GND            SERVO_PULSE_GPIO +-----+PWM        VCC +----+
+-------|      |              |                                 |     |               |
+-------|      |              |                             GND +-----+GND            |
        +------+              +---------------------------------+     +---------------+
```
### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project or use the VS Code flash and monitor 🔥

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects or using the [VS Code extention](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/basic_use.md)

## Example Output

```
I (0) cpu_start: App cpu up.
I (208) cpu_start: Pro cpu start user code
I (208) cpu_start: cpu freq: 160000000 Hz
I (208) cpu_start: Application information:
I (211) cpu_start: Project name:     hook_demo
I (216) cpu_start: App version:      da0caa5-dirty
I (221) cpu_start: Compile time:     Jul 14 2023 21:00:27
I (228) cpu_start: ELF file SHA256:  ce2e608bef4e78b7...
I (233) cpu_start: ESP-IDF:          v5.1-dirty
I (239) cpu_start: Min chip rev:     v0.0
I (243) cpu_start: Max chip rev:     v0.99 
I (248) cpu_start: Chip rev:         v0.1
I (253) heap_init: Initializing. RAM available for dynamic allocation:
I (260) heap_init: At 3FC94558 len 000551B8 (340 KiB): DRAM
I (266) heap_init: At 3FCE9710 len 00005724 (21 KiB): STACK/DRAM
I (273) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (279) heap_init: At 600FE010 len 00001FF0 (7 KiB): RTCRAM
I (287) spi_flash: detected chip: generic
I (290) spi_flash: flash io: dio
I (295) app_start: Starting scheduler on CPU0
I (299) app_start: Starting scheduler on CPU1
I (299) main_task: Started on CPU0
I (309) main_task: Calling app_main()
I (309) pwm_generator: Connect timer and operator
I (319) pwm_generator: Create comparator and generator from the operator
I (319) pwm_generator: Connect timer and operator
I (329) gpio: GPIO[1]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (329) pwm_generator: Create comparator and generator from the operator
I (339) pwm_generator: Set generator action on timer and compare event
I (349) gpio: GPIO[2]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (359) pwm_generator: Enable and start timer
I (359) pwm_generator: Set generator action on timer and compare event
I (369) pwm_generator: Timer enable and running
I (379) pwm_generator: Enable and start timer
I (379) pwm_generator: Functions register in base
I (389) pwm_generator: Timer enable and running
I (389) pwm_generator: Initialization ended
I (399) pwm_generator: Functions register in base
I (399) pwm_capture: Initialize capture timer
I (409) pwm_generator: Initialization ended
I (409) pwm_capture: Initialize capture channel
I (419) pwm_capture: Initialize capture timer
I (419) gpio: GPIO[13]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (429) pwm_capture: Initialize capture channel
I (439) pwm_capture: Register capture callback
I (439) gpio: GPIO[14]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (449) pwm_capture: Enable capture channel
I (459) pwm_capture: Register capture callback
I (459) pwm_capture: Enable and start capture timer
I (469) pwm_capture: Enable capture channel
I (469) pwm_capture: Timer enable and running
I (479) pwm_capture: Enable and start capture timer
I (479) pwm_capture: Functions register in base
I (489) pwm_capture: Timer enable and running
I (489) pwm_capture: Initialization ended
I (499) pwm_capture: Functions register in base
I (509) pwm_capture: Initialization ended
I (509) hook_demo: Ellapsed time: 12 
I (539) hook_demo: Ellapsed time: 42 
I (559) hook_demo: Ellapsed time: 57 
I (579) hook_demo: Ellapsed time: 72 
I (599) hook_demo: Ellapsed time: 102 
I (619) hook_demo: Ellapsed time: 117 
I (639) hook_demo: Ellapsed time: 132 
I (659) hook_demo: Ellapsed time: 162 
I (679) hook_demo: Ellapsed time: 177 
I (699) hook_demo: Ellapsed time: 192 
I (719) hook_demo: Ellapsed time: 222 
I (739) hook_demo: Ellapsed time: 237 
I (759) hook_demo: Ellapsed time: 252 
I (779) hook_demo: Ellapsed time: 282 
I (799) hook_demo: Ellapsed time: 297 
I (819) hook_demo: Ellapsed time: 312 
I (839) hook_demo: Ellapsed time: 342 
I (859) hook_demo: Ellapsed time: 357 
I (879) hook_demo: Ellapsed time: 372 
I (899) hook_demo: Ellapsed time: 402 
I (919) hook_demo: Ellapsed time: 417 
I (939) hook_demo: Ellapsed time: 432 
I (959) hook_demo: Ellapsed time: 462 
I (979) hook_demo: Ellapsed time: 477 
I (999) hook_demo: Ellapsed time: 492 
I (1019) hook_demo: Ellapsed time: 522 

```

This example runs at 10Hz sampling rate. Measure data that out of the range is dropped and only valid measurement is printed out.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/smartflocks/ackermann-vehicle-esp/issues) on GitHub.