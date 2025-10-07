Arduino Lite Scope is a lightweight oscilloscope project that captures analog signals with an Arduino and visualizes them in real time on a PC using Python.

The system combines:

- Arduino firmware — samples an analog input (e.g., a sine wave or sensor signal) using the onboard ADC, buffers the data, and sends it to the PC via serial.
- Python script — receives the samples, converts them to voltages, and plots them in a simple GUI window for real-time visualization.



The Arduino continuously captures 800 ADC samples at an effective rate of approximately 539 kS/s, achieved by tweaking the vendor’s analog read library for faster sampling.

- Once the buffer is full, it sends a "start" marker followed by the sample data through the serial port.
- The Python application listens for this marker, reads the samples, converts them to voltages, and plots the waveform.
