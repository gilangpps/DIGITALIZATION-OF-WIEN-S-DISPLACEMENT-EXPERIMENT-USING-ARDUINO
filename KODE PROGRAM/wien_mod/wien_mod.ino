#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Pustaka untuk LCD I2C
#include "Adafruit_TCS34725.h"

// Inisialisasi sensor TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

// Inisialisasi LCD dengan alamat 0x27, ukuran 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  lcd.init(16,2);         // Inisialisasi LCD
  lcd.backlight();    // Menyalakan lampu latar LCD

  // Menampilkan pesan awal di LCD
  lcd.setCursor(0, 0); 
  lcd.print("Initializing...");
  
  if (tcs.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("WIEN DISPLCMNT");
    delay(2000); // Menunggu sejenak sebelum melanjutkan
  } else {
    lcd.setCursor(0, 1);
    lcd.print("TCS34725 Failed");
    while (1); // Berhenti jika sensor tidak terdeteksi
  }
  
  lcd.clear(); // Menghapus layar LCD
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

  // Menampilkan nilai panjang gelombang dan suhu warna di LCD
  lcd.setCursor(0, 0);  // Baris pertama
  lcd.print("WL:");     // Menampilkan panjang gelombang
  lcd.print(wavelength, 0); // Menampilkan nilai panjang gelombang dalam nm
  lcd.print("nm ");

  lcd.setCursor(0, 1);  // Baris kedua
  lcd.print("Temp:");   
  lcd.print(colorTemp, 0); // Menampilkan nilai temperatur warna dalam Kelvin
  lcd.print("K ");
  
  // Delay untuk pembacaan tiap 1 detik
  delay(1000);
}
