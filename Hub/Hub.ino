
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

#include "HomeSpan.h"         // include the HomeSpan library

struct BatteryMeasurement : Service::BatteryService {
  SpanPoint *remoteSensor;
  SpanCharacteristic *batteryLevel;
  SpanCharacteristic *chargingState;
  SpanCharacteristic *statusLowBattery;
  const char *name;
  String receivedData;
  float *batteryReceived;
  bool *newBatteryAvailable;

  BatteryMeasurement(const char *name, SpanPoint *remoteSensor, bool *newBatteryAvailable, float *batteryReceived) : Service::BatteryService() {
    batteryLevel = new Characteristic::BatteryLevel();
    chargingState = new Characteristic::ChargingState();
    statusLowBattery = new Characteristic::StatusLowBattery();

    this->name=name;
    this->remoteSensor=remoteSensor;
    this->newBatteryAvailable=newBatteryAvailable;
    this->batteryReceived=batteryReceived;
  }

  void loop() {
    if(*newBatteryAvailable){      // if there is data from the remote sensor
      LOG1("BATT: %d\n", *batteryReceived);
      batteryLevel->setVal(*batteryReceived/4095*100);            // update temperature
       
      LOG1("Sensor %s update: Battery=%0.2f\n",name,*batteryReceived/4095*100);
      *newBatteryAvailable = false;
    }
  }
};

struct SoilMoisture : Service::HumiditySensor {
  SpanPoint *remoteSensor;
  SpanCharacteristic *soilMoistureLevel;
  const char *name;
  float soilMoistureValue;
  String receivedData;
  float *batteryReceived;
  bool *newBatteryAvailable;
 
  SoilMoisture(const char *name, SpanPoint *remoteSensor, bool *newBatteryAvailable, float *batteryReceived) : Service::HumiditySensor() {
    soilMoistureLevel = new Characteristic::CurrentRelativeHumidity();

    this->name=name;
    this->remoteSensor=remoteSensor;
    this->newBatteryAvailable=newBatteryAvailable;
    this->batteryReceived=batteryReceived;
  }

  void loop() {
    if(remoteSensor->get(&receivedData)){      // if there is data from the remote sensor
      LOG1("received: %s\n", receivedData.c_str());
      int index = receivedData.indexOf('|');
      soilMoistureValue = receivedData.substring(0, index).toInt();
      *batteryReceived = receivedData.substring(index+1, receivedData.length()).toInt();
      *newBatteryAvailable = true;
      LOG1("SOIL: %d\n", soilMoistureValue);
      soilMoistureLevel->setVal(soilMoistureValue/4095*100);            // update temperature
        
      LOG1("Sensor %s update: Soil-Moisture=%0.2f\n",name,soilMoistureValue/4095*100);
    }
  }
};

struct RemoteSensor {
  SpanPoint *remoteSensor;
  float batteryReceived;
  bool newBatteryAvailable = false;
  RemoteSensor(const char *name, const char *macAddress) {
    new SpanAccessory();
    remoteSensor=new SpanPoint(macAddress,0,sizeof(String));
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(name);
    new SoilMoisture(name, remoteSensor, &newBatteryAvailable, &batteryReceived);
    new BatteryMeasurement(name, remoteSensor, &newBatteryAvailable, &batteryReceived);
  }
};

void setup() {     
 
  Serial.begin(115200);       // start the Serial interface

  homeSpan.setLogLevel(1);

  homeSpan.begin(Category::Bridges,"Sensor Hub");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 

  new RemoteSensor("Planty 1","84:0d:8e:07:e3:4c");
  
} // end of setup()

void loop(){

  homeSpan.poll();

} // end of loop()