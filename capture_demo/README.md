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
I (0) cpu_start: Starting scheduler on APP CPU.
I (304) example: Create capture queue
I (304) example: Install capture timer
I (304) example: Install capture channel
I (314) gpio: GPIO[2]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (324) example: Register capture callback
I (324) example: Create a timer to trig HC_SR04 periodically
I (334) example: Configure Trig pin
I (334) gpio: GPIO[0]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (344) example: Enable and start capture timer
I (434) example: Pulse width: 189.02us, Measured distance: 3.26cm
I (534) example: Pulse width: 189.02us, Measured distance: 3.26cm
I (634) example: Pulse width: 189.01us, Measured distance: 3.26cm
I (734) example: Pulse width: 188.98us, Measured distance: 3.26cm
I (834) example: Pulse width: 188.99us, Measured distance: 3.26cm
```

This example runs at 10Hz sampling rate. Measure data that out of the range is dropped and only valid measurement is printed out.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/smartflocks/ackermann-vehicle-esp/issues) on GitHub.