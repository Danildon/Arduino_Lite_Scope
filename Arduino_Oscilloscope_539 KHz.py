import serial
import time
import numpy as np
import matplotlib.pyplot as plt
import tkinter as tk
import sys

# ========================= Configuration =========================
PORT = 'COM4'
BAUDRATE = 115200
NUM_SAMPLES = 800
FS = 539_000                 # Sampling frequency in Hz
TS = 1 / FS                  # Sampling interval in seconds
ADC_SCALE = 0.0048875855     # ADC to voltage conversion factor

# ========================= Serial Setup =========================
print(f"# Connecting on port {PORT} at {BAUDRATE}")
arduino = serial.Serial(PORT, BAUDRATE)

# Reset Arduino to start clean
arduino.setDTR(1)
time.sleep(0.25)
arduino.setDTR(0)

# ========================= Data Acquisition =========================
def read_voltage():
    """Read NUM_SAMPLES from Arduino and convert to voltages."""
    data_raw = []
    voltage_data = []
    time_array = np.arange(3, NUM_SAMPLES + 1) * TS

    # Wait until data is available
    while arduino.in_waiting == 0:
        pass

    # Wait for sync signal
    while arduino.readline() != b'start\r\n':
        pass

    # Read raw ADC data
    for _ in range(NUM_SAMPLES):
        line = arduino.readline()
        data_raw.append(line)

    # Convert to voltage
    for line in data_raw:
        try:
            value = int(line)
        except (ValueError, TypeError):
            value = 500  # default if conversion fails
        voltage = ADC_SCALE * value
        voltage_data.append(voltage)

    return voltage_data, time_array

# ========================= Plotting =========================
def plot_data():
    """Read data from Arduino and plot in real-time."""
    plt.close('all')
    plt.ylim(-0.001, 5.001)
    plt.title('Arduino Scope')
    plt.xlabel('Time [s]')
    plt.ylabel('Voltage [V]')
    plt.grid(True)

    ax = plt.subplot(111)
    voltage, time_array = read_voltage()
    ax.plot(time_array, voltage)
    plt.show()

# ========================= GUI =========================
def quit_app():
    """Quit the application safely."""
    plt.close('all')
    root.quit()
    sys.exit()

def start_acquisition():
    """Start real-time acquisition loop."""
    while True:
        plot_data()

# Create GUI window
root = tk.Tk()
root.title("Arduino Scope")
root.geometry('300x50+500+10')

# Start button
tk.Button(root, text='START', command=start_acquisition).pack(pady=5)

# Handle window close
root.protocol('WM_DELETE_WINDOW', quit_app)

# Run GUI event loop
root.mainloop()
