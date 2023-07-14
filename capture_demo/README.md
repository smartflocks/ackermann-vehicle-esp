| Tested Targets | ESP32-S3 |
| -------------- | -------- |

# PWM capture

The capture module in MCPWM peripheral is designed to accurately log the time stamp on the hardware side when an event happens (compared to GPIO ISR which requires a software-based logging method). Each capture unit has three channels, which can be used together to capture IO events in parallel but in the capture component we only use one channel per group

This example shows how to use the make use of the pwm_capture component hardware features to decode the pulse width signals generated from a RC radio signal

Typical signals:

```
^
| 20% modulation +-+    +-+    +-+    +-+    +-+
|                | |    | |    | |    | |    | |
|                | |    | |    | |    | |    | |
|       ---------+ +----+ +----+ +----+ +----+ +----
| 40% modulation +--+   +--+   +--+   +--+   +--+
|                |  |   |  |   |  |   |  |   |  |
|                |  |   |  |   |  |   |  |   |  |
|       ---------+  +---+  +---+  +---+  +---+  +---
| 60% modulation +---+  +---+  +---+  +---+  +---+
|                |   |  |   |  |   |  |   |  |   |
|                |   |  |   |  |   |  |   |  |   |
|       ---------+   +--+   +--+   +--+   +--+   +--
| 80% modulation +----+ +----+ +----+ +----+ +----+
|                |    | |    | |    | |    | |    |
|                |    | |    | |    | |    | |    |
|       ---------+    +-+    +-+    +-+    +-+    +-
+--------------------------------------------------->
                Timeline
```

## How to Use Example

### Hardware Required

* An ESP development board that features the MCPWM peripheral
* An RC receiver module

Connection :

```
        +------+              +---------------------------------+
        |      |              |                                 |
        + PWM  +----=====>----+ GPIO13 (internal pull up)       |
        |      |              |                                 |
        |  VCC +--------------+ 5V                              |
        |      |              |                                 |
+-------+  GND +--------------+ GND                             |
+-------|      |              |                                 |
+-------|      |              |                                 |
        +------+              +---------------------------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project or use the VS Code flash and monitor 🔥

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects or using the [VS Code extention](https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/basic_use.md)

## Example Output

```
I (0) cpu_start: App cpu up.
I (189) cpu_start: Pro cpu start user code
I (189) cpu_start: cpu freq: 160000000 Hz
I (189) cpu_start: Application information:
I (192) cpu_start: Project name:     capture_demo
I (198) cpu_start: App version:      4f1d914-dirty
I (203) cpu_start: Compile time:     Jul 14 2023 09:47:14
I (209) cpu_start: ELF file SHA256:  17ec7739d13cad96...
I (215) cpu_start: ESP-IDF:          v5.1-dirty
I (220) cpu_start: Min chip rev:     v0.0
I (225) cpu_start: Max chip rev:     v0.99 
I (230) cpu_start: Chip rev:         v0.1
I (235) heap_init: Initializing. RAM available for dynamic allocation:
I (242) heap_init: At 3FC94768 len 00054FA8 (339 KiB): DRAM
I (248) heap_init: At 3FCE9710 len 00005724 (21 KiB): STACK/DRAM
I (255) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (261) heap_init: At 600FE010 len 00001FF0 (7 KiB): RTCRAM
I (268) spi_flash: detected chip: generic
I (272) spi_flash: flash io: dio
W (276) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (289) sleep: Configure to isolate all GPIO pins in sleep state
I (296) sleep: Enable automatic switching of GPIO sleep configuration
I (303) app_start: Starting scheduler on CPU0
I (308) app_start: Starting scheduler on CPU1
I (308) main_task: Started on CPU0
I (318) main_task: Calling app_main()
I (318) pwm_capture: Initialize capture timer
I (328) pwm_capture: Initialize capture channel
I (328) gpio: GPIO[13]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (338) pwm_capture: Register capture callback
I (348) pwm_capture: Enable capture channel
I (348) pwm_capture: Enable and start capture timer
I (358) pwm_capture: Timer enable and running
I (358) pwm_capture: Functions register in base
I (368) pwm_capture: Initialization ended
I (368) pwm_capture: Initialize capture timer
I (378) pwm_capture: Initialize capture channel
I (378) gpio: GPIO[14]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (388) pwm_capture: Register capture callback
I (398) pwm_capture: Enable capture channel
I (398) pwm_capture: Enable and start capture timer
I (408) pwm_capture: Timer enable and running
I (408) pwm_capture: Functions register in base
I (418) pwm_capture: Initialization ended
I (418) capture_demo: Current PWM A: 1508 
I (428) capture_demo: Current PWM B: 1500 
I (448) capture_demo: Current PWM A: 1508 
I (448) capture_demo: Current PWM B: 1500 
I (468) capture_demo: Current PWM A: 1508 
I (468) capture_demo: Current PWM B: 1500 
I (488) capture_demo: Current PWM A: 1507 
I (488) capture_demo: Current PWM B: 1500 
I (508) capture_demo: Current PWM A: 1508 

```

This example runs at 10Hz sampling rate. Measure data that out of the range is dropped and only valid measurement is printed out.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/smartflocks/ackermann-vehicle-esp/issues) on GitHub.