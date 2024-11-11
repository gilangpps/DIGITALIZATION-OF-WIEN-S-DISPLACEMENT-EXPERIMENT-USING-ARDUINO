import serial
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import tkinter as tk
from tkinter import Label, Frame
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Inisialisasi koneksi serial (sesuaikan port sesuai dengan Arduino Anda)
ser = serial.Serial('COM3', 9600, timeout=1)

# Inisialisasi array untuk menyimpan data
wavelengths = []
intensities = []
temps = []

# Konstanta Wien yang lebih akurat
WIEN_CONSTANT = 2.8977719e6  # dalam nm.K

# Fungsi Wien untuk menghitung lambda max
def wien_lambda_max(T):
    return WIEN_CONSTANT / T  # Menghitung panjang gelombang maksimum (nm)

# Fungsi untuk menghitung beberapa kurva Wien
def plot_wien_curves(ax):
    temperatures = [3000, 4000, 5000, 6000]  # Kelvin
    wavelengths = np.linspace(0.1, 3, 1000) * 1e3  # Ubah dari μm ke nm
    for T in temperatures:
        intensities = (1 / wavelengths**5) / (np.exp(1.4388e7 / (wavelengths * T)) - 1)
        ax.plot(wavelengths, intensities / np.max(intensities), label=f'{T}K')

# Inisialisasi GUI menggunakan Tkinter
root = tk.Tk()
root.title("Real-Time Light Intensity Monitor")

# Membuat frame utama untuk menampung label dan plot
frame = Frame(root)
frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

# Membuat label untuk menampilkan data
label_wavelength = Label(frame, text="Wavelength: -- nm", font=("Arial", 12))
label_wavelength.pack()

label_intensity = Label(frame, text="Intensity: -- Lux", font=("Arial", 12))
label_intensity.pack()

label_temperature = Label(frame, text="Temperature: -- K", font=("Arial", 12))
label_temperature.pack()

# Membuat figure Matplotlib
fig, ax = plt.subplots()
plot_wien_curves(ax)
ax.set_xlabel("Wavelength / nm")
ax.set_ylabel("Intensity (arb. units)")
ax.set_title("Light Intensity vs Wavelength")
ax.set_facecolor("#d3d3d3")
ax.legend()

# Memasukkan plot ke dalam Tkinter GUI
canvas = FigureCanvasTkAgg(fig, master=frame)  # canvas Matplotlib ke dalam Tkinter
canvas.draw()
canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

# Inisialisasi plot awal
line, = ax.plot(wavelengths, intensities, 'r-', lw=2)

# Fungsi untuk memperbarui GUI
def update_gui(wavelength, intensity, temp):
    label_wavelength.config(text=f"Wavelength: {wavelength:.2f} nm")
    label_intensity.config(text=f"Intensity: {intensity:.2f} Lux")
    label_temperature.config(text=f"Temperature: {temp:.2f} K")
    root.update()

# Fungsi untuk memperbarui grafik secara real-time
def update_plot(wavelength, intensity, temp):
    wavelengths.append(wavelength)
    intensities.append(intensity)
    temps.append(temp)

    line.set_xdata(wavelengths)
    line.set_ydata(intensities)

    ax.relim()
    ax.autoscale_view()
    canvas.draw()  # Perbarui canvas Matplotlib

# Fungsi untuk membaca data dari serial
def read_serial_data():
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').strip()
        if data:
            try:
                r, g, b, lux, wavelength = map(float, data.split(','))
                temp_measured = (WIEN_CONSTANT / wavelength)  # Menghitung suhu
                print(f"Wavelength: {wavelength} nm, Intensity: {lux} Lux, Temp: {temp_measured:.2f} K")

                # Memperbarui GUI dan grafik dengan data baru
                update_gui(wavelength, lux, temp_measured)
                update_plot(wavelength, lux, temp_measured)
            except ValueError:
                pass  # Jika parsing gagal, lanjutkan loop

    root.after(100, read_serial_data)  # Memanggil fungsi ini lagi setelah 100ms

# Fungsi untuk menyimpan data ke file Excel
def save_data_to_excel():
    df = pd.DataFrame({
        'Wavelength (nm)': wavelengths,
        'Intensity (Lux)': intensities,
        'Temperature (K)': temps
    })
    df.to_excel('light_intensity_data.xlsx', index=False)
    print("Data disimpan dalam format Excel sebagai 'light_intensity_data.xlsx'.")

# Mulai pembacaan serial dan GUI
root.after(100, read_serial_data)  # Memulai pembacaan data
root.mainloop()
