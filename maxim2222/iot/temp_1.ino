// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCN75A
// This code is designed to work with the TCN75A_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Temperature?sku=TCN75A_I2CS#tabs-0-product_tabset-2

#include <Wire.h>
#include <SoftwareSerial.h>
// TCN75A I2C address is 0x48(72)
#include <SPI.h>
#include <SD.h>

#define Addr 0x48

SoftwareSerial Bluetooth(26, 27); /* RX, TX */
File myFile;

int delay_time = 500;

void setup()
{
  // Initialise I2C communication as Master
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  Bluetooth.begin(9600);
  
  if (!SD.begin(52)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select configuration register
  Wire.write(0x01);
  // 12-bit ADC resolution
  Wire.write(0x60);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);
}

void loop()
{
  unsigned int data[2];
  if (Bluetooth.available()) {
    String inp = Bluetooth.readString();
    delay_time = inp.toInt();
  }
  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 2 bytes of data
  Wire.requestFrom(Addr, 2);

  // Read 2 bytes of data
  // temp msb, temp lsb
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

  // Convert the data to 12-bits
  int temp = (((data[0] * 256) + (data[1] & 0xF0)) / 16);
  if(temp > 2047)
  {
    temp -= 4096;  
   }
  float cTemp = temp * 0.0625;
 // float fTemp = (cTemp * 1.8) + 32;

  // Output data to serial monitor
  delay(delay_time);
  Bluetooth.println(cTemp);
 myFile = SD.open("1.txt", FILE_WRITE);
   if (myFile) {
    myFile.println(cTemp);
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening 1.txt");
  }
  myFile.close();
  //delay(1000);
}
