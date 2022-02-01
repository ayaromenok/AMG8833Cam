/*
  Using the Panasonic Grid-EYE Sensor
  By: Nick Poole
  SparkFun Electronics
  Date: January 12th, 2018
  
  MIT License: Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software without 
  restriction, including without limitation the rights to use, copy, modify, merge, publish, 
  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14568
  
  This example implements a rudimentary heat camera in the serial terminal, using a matrix of ascii
  characters to represent the temperature of each pixel in the device. Start your terminal at 115200 
  and make the window as small as possible for best effect.
  
  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
*/

#include <SparkFun_GridEYE_Arduino_Library.h>
#include <Wire.h>

// Use these values (in degrees C) to adjust the contrast
#define HOT 50
#define COLD 10

// This table can be of type int because we map the pixel 
// temperature to 0-3. Temperatures are reported by the 
// library as floats
int pixelTable[64];

GridEYE grideye;

void setup() {

  // Start your preferred I2C object 
  Wire.begin();
  // Library assumes "Wire" for I2C but you can pass something else with begin() if you like
  grideye.begin();
  void setFramerate1FPS();
  //void setFramerate10FPS();

  // Pour a bowl of serial
  Serial.begin(9600);

}

void loop() {
// float getPixelTemperature(unsigned char pixelAddr);
// int16_t getPixelTemperatureRaw(unsigned char pixelAddr);
// float getDeviceTemperature();
// int16_t getDeviceTemperatureRaw();
  for(unsigned char i = 0; i < 8; i++){
    for(unsigned char j = 0; j < 8; j++){
      //Serial.print(grideye.getPixelTemperature(i+j*8));
      Serial.print(grideye.getPixelTemperatureRaw(i+j*8),HEX);
      //Serial.print(' ');
    }//for j
    //Serial.println();
  }//for i
  Serial.println("");
  delay(1000);

}
