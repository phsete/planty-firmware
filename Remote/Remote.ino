
/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2023 Philipp Tebbe
 *  
 *  https://github.com/phsete/planty-firmware
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************
 *
 *  This script uses the HomeSpan Library by Gregg E. Berman and is based on the 
 *  RemoteSensor example.
 *  Check it out at https://github.com/HomeSpan/HomeSpan !
 *  For this script to work you have to install the library first. For instructions
 *  please have alook at the stated github link.
 *
 ********************************************************************************/

#include "HomeSpan.h"

#define SAMPLE_TIME   30000     // Time between samples (in milliseconds)

SpanPoint *mainDevice;

void setup() {

  setCpuFrequencyMhz(80);       // reduce CPU frequency to save battery power  
  
#if defined(DIAGNOSTIC_MODE)  
  homeSpan.setLogLevel(1);
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Starting Remote Sensor.  MAC Address of this device = %s\n",WiFi.macAddress().c_str());
#endif

  // In the line below, replace the MAC Address with that of your MAIN HOMESPAN DEVICE

  mainDevice=new SpanPoint("24:6F:28:22:A3:1C",sizeof(String),0);    // create a SpanPoint with send size=sizeof(float) and receive size=0     
}

void loop() {
  int soilMoistureValue = analogRead(33);
  int batteryValue = analogRead(32);
  String dataToSend = "|"; // SOILMOISTURE|BATTERY - Werte zwischen 0 und 4095
  dataToSend = soilMoistureValue + dataToSend;
  dataToSend = dataToSend + batteryValue;

  boolean success = mainDevice->send(&dataToSend);                 // this will show as success as long as the MAIN DEVICE is running
#if defined(DIAGNOSTIC_MODE)  
  Serial.printf("Send %s\n",success?"Succeded":"Failed");
#endif
  esp_deep_sleep(SAMPLE_TIME*1000);     // enter deep sleep mode -- reboot after resuming
}
