
#include <ArduinoBLE.h>
#include <stdlib.h>
//#include <ArduinoUniqueID.h>

BLEService multimeterService("1101");
BLECharacteristic latestValue("2101", BLERead|BLENotify, 8);
BLEIntCharacteristic activeSensor("2110", BLERead|BLEWrite);
unsigned char bytes[8];
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


  pinMode(LED_BUILTIN, OUTPUT);
  
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
    int output_number1 = 30;
    int output_number2 = 30;
    int output_number3 = 30;
    
    
    //Serial.print("Connected to central: ");
    //Serial.println(central.address());

    while(central.connected())
    {
      digitalWrite(LED_BUILTIN, HIGH);
      activeSensor.readValue(current_sensor);
      //Serial.println("Current Sensor");
      //Serial.println(current_sensor);
      

      if(generate_random){
        int random_number1 = rand() % 2;
        int random_number2 = rand() % 2;
        int random_number3 = rand() % 2;
        switch(random_number1){
          case 0:
            output_number1 += 1;
            break;
          case 1:
            output_number1 -= 1;
            break;
        }
        switch(random_number2){
          case 0:
            output_number2 += 1;
            break;
          case 1:
            output_number2 -= 1;
            break;
        }
        switch(random_number3){
          case 0:
            output_number3 += 1;
            break;
          case 1:
            output_number3 -= 1;
            break;
        }

        int result_output1 = output_number1 * 1000;
        int result_output2 = output_number2 * 1000;
        int result_output3 = output_number3 * 1000;

        bytes[5] = highByte(result_output3);
        bytes[4] = lowByte(result_output3);
        bytes[3] = highByte(result_output2);
        bytes[2] = lowByte(result_output2);
        bytes[1] = highByte(result_output1);
        bytes[0] = lowByte(result_output1);

        
        
      }else{
        //Serial.println("Analog Read!");
        
        //Serial.println(analog_value);
        
        // Voltage
        int voltage_value = analogRead(0);
        voltage_value = map(voltage_value, 0, 1023, 0, 25000);
        voltage_value = voltage_value*(3.3/5);
        
        // Current
        int current_value = analogRead(1);
        current_value = map(current_value, 0, 1023, -30000, 30000);
        current_value = current_value*(5/3.3);

        // Resistance
        int resistance_value = analogRead(2);
        int sensor_value = map(resistance_value, 0, 1023, 0, 3300);
        if(sensor_value != 0){
          resistance_value = (10000 * (3300 - sensor_value))/sensor_value;
        }else{
          resistance_value = -1;
        }


        bytes[5] = highByte(resistance_value);
        bytes[4] = lowByte(resistance_value);
        bytes[3] = highByte(current_value);
        bytes[2] = lowByte(current_value);
        bytes[1] = highByte(voltage_value);
        bytes[0] = lowByte(voltage_value);
          
      }
      
      //Serial.println("Still Connected!");

      //Serial.println(current_sensor);

      

      //Serial.println("Converted Analog Read!");
      //Serial.println(analog_value);

      bytes[7] = 0x0;
      bytes[6] = 0x0;
      
            
      latestValue.writeValue(bytes, 8);
      delay(100);

    }
    digitalWrite(LED_BUILTIN, LOW);

    //Serial.print("Disconnected from central: ");
    //Serial.println(central.address());

    BLE.advertise();
    //Serial.println("Bluetooth device active, waiting for connections...");
  }
  

  

}
