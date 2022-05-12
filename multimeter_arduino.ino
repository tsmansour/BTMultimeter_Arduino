
#include <ArduinoBLE.h>
#include <stdlib.h>
//#include <ArduinoUniqueID.h>

BLEService multimeterService("1101");
BLECharacteristic latestValue("2101", BLERead|BLENotify, 4);
BLEIntCharacteristic activeSensor("2110", BLERead|BLEWrite);
unsigned char bytes[4];
uint32_t current_sensor = 0;


// 0 = voltage
// 1 = Current
// 2 = Resistance

// Set value to true or false to control if random data is used or not
bool generate_random = false;


void setup()
{
  // put your setup code here, to run once:
  //Serial.begin(9600);
  //while(!Serial);
  //attachInterrupt(digitalPinToInterrupt(2), buttonPress, RISING);
  
  // begin initialization
  if (!BLE.begin())
  {
    //Serial.println("starting Bluetooth® Low Energy module failed!");
    while(1);
  }
  BLE.setLocalName("BMM BLE1");
  BLE.setDeviceName("BMM BLE1");
  BLE.setAdvertisedService(multimeterService);
  multimeterService.addCharacteristic(latestValue);
  multimeterService.addCharacteristic(activeSensor);
  BLE.addService(multimeterService);


  activeSensor.writeValue(0);


  BLE.advertise();
  //Serial.println("Bluetooth device active, waiting for connections...");
}

void loop()
{
  // put your main code here, to run repeatedly:

  BLEDevice central = BLE.central();

  if(central)
  {
    int output_number = 30;
    int analog_value;
    
    
    //Serial.print("Connected to central: ");
    //Serial.println(central.address());
    
    int lastPress = millis();

    while(central.connected())
    {
      activeSensor.readValue(current_sensor);
      //Serial.println("Current Sensor");
      //Serial.println(current_sensor);
      

      if(generate_random){
        int random_number = rand();
        random_number = random_number %2;
        switch(random_number){
          case 0:
            output_number += 1;
            break;
          case 1:
            output_number -= 1;
            break;
        }

        if(current_sensor == 0){
          bytes[2] = 0x02;
        }
        if(current_sensor == 1){
          bytes[2] = 0x03;
        }
        if(current_sensor == 2){
          bytes[2] = 0x04;
        }
        analog_value = output_number * 1000;
        
      }else{
        //Serial.println("Analog Read!");
        analog_value = analogRead(current_sensor);
        //Serial.println(analog_value);
        
  
        if(current_sensor == 0){
          analog_value = map(analog_value, 0, 1023, 0, 25000);
          analog_value = analog_value*(3.3/5);
          bytes[2] = 0x02;
        }
        if(current_sensor == 1){
          analog_value = map(analog_value, 0, 1023, -30000, 30000);
          analog_value = analog_value*(5/3.3);
          bytes[2] = 0x03;
        }
        if(current_sensor == 2){
          int sensor_value = map(analog_value, 0, 1023, 0, 3300);
          //Serial.println(sensor_value);
          if(sensor_value != 0){
            analog_value = (10000 * (3300 - sensor_value))/sensor_value;
          }else{
            analog_value = -1;
          }
          
          bytes[2] = 0x04;
        }
      }
      bytes[3] = 0x00;
      
      //Serial.println("Still Connected!");

      //Serial.println(current_sensor);

      

      //Serial.println("Converted Analog Read!");
      //Serial.println(analog_value);
      

      bytes[1] = highByte(analog_value);
      bytes[0] = lowByte(analog_value);
      //Serial.println(bytes[0]);
      //Serial.println(bytes[1]);
      //Serial.println(bytes[2]);
            
      latestValue.writeValue(bytes, 4);
      delay(100);

      if(analogRead(7) > 500){
        
        //Serial.println("BUTTON PRESS");
        if(millis() - lastPress > 600){
          if(current_sensor == 0){
            current_sensor = 1;
          }else if(current_sensor == 1){
            current_sensor = 2;
          }else if(current_sensor == 2){
            current_sensor = 0;
          }
          lastPress = millis();
          activeSensor.writeValue(current_sensor);
        }
      }
    }

    //Serial.print("Disconnected from central: ");
    //Serial.println(central.address());

    BLE.advertise();
    //Serial.println("Bluetooth device active, waiting for connections...");
  }
  

  

}
