#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Inisialisasi sensor TCS34725 dengan waktu integrasi dan gain
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  if (tcs.begin()) {
    Serial.println("TCS34725 detected");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // Berhenti jika sensor tidak terdeteksi
  }
}

// Fungsi untuk menghitung panjang gelombang (nm) dari temperatur warna (Kelvin)
float calculateWavelength(float colorTemp) {
  // Menggunakan konstanta Wien yang lebih akurat
  return 2.8977719e6 / colorTemp;
}

void loop() {
  uint16_t r, g, b, c;
  float colorTemp, lux, wavelength;
  
  // Membaca nilai dari sensor TCS34725
  tcs.getRawData(&r, &g, &b, &c);

  // Menghitung temperatur warna dan lux menggunakan fungsi bawaan
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

  // Jika temperatur warna valid (bukan NaN atau 0)
  if (!isnan(colorTemp) && colorTemp > 0) {
    // Menghitung panjang gelombang dari temperatur warna
    wavelength = calculateWavelength(colorTemp);
  } else {
    // Jika data tidak valid, panjang gelombang tidak dihitung
    wavelength = 0;
  }

  // Mengirim data RGB, lux, dan panjang gelombang (nm) ke serial dengan format yang sesuai
  Serial.print(r); Serial.print(",");
  Serial.print(g); Serial.print(",");
  Serial.print(b); Serial.print(",");
  Serial.print(lux); Serial.print(",");
  Serial.println(wavelength);  // Mengirimkan data panjang gelombang terakhir

  // Delay untuk pembacaan tiap 1 detik
  delay(1000);
}
