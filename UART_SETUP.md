# UART Time Synchronization

Firmware now supports time synchronization via UART (9600 baud, 8N1).

## functionality
The microcontroller listens for a time string in the format `HH:MM:SS` (e.g., `12:30:45`) terminated by a newline. Upon receiving a valid time, it updates the internal clock.

## How to use
Connect the MSP430 LaunchPad to your computer. Identify the serial port (usually `/dev/ttyACM0` on Linux).

Run the following command in your terminal to sync the time:

```bash
date +%T > /dev/ttyACM0
```

- **`date +%T`**: Outputs the current time in `HH:MM:SS` format.
- **`> /dev/ttyACM0`**: Redirects the output to the serial port.

## Pin Configuration
- **RX**: P1.1 (Receives data from PC)
- **TX**: P1.2 (Not used for this feature, but initialized)
