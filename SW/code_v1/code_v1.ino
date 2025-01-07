#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <rom/ets_sys.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>
#include "INA219.h"
#include <sstream>
#include <vector>

// INA var

INA219 INA(0x40);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BLE stuff

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEDescriptor *pDescr;
BLE2902 *pBLE2902;


bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

std::string my_received_message;

#define SERVICE_UUID        "73bb297a-630b-4bb0-86cb-07ea430f3221"
#define CHARACTERISTIC_UUID "d91aae5a-08e1-48e3-8985-3612800fca74"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

uint16_t delay_mare=5000;

class  MyCallBacks: public BLECharacteristicCallbacks
{                                                                                                
  bool commandValidation (int delay)
  {  
      return delay >= 5000 && delay <= 30000; 
  }

  void onWrite (BLECharacteristic *pCharacteristic) {
     my_received_message=pCharacteristic ->getValue();
      Serial.println(("received :"+my_received_message).c_str()) ; 

    std::vector<std::string> strings;
    std::istringstream strStream(my_received_message);
    std::string part;    
    while (std::getline(strStream, part, ' ')) {
        strings.push_back(part);
    }
      
    int delay=std::stoi(strings[1] );

     if ( strings[0] == "BLE_Command" && commandValidation (delay))
     { 

       delay_mare=delay;
       Serial.print("delay_mare BLE=");
       Serial.println("delay_mare");
     }
  }  
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESC stuff

gpio_num_t ESC_PINS_1[]= {GPIO_NUM_NC,GPIO_NUM_18,GPIO_NUM_13, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
gpio_num_t ESC_PINS_2[]= {GPIO_NUM_NC,GPIO_NUM_39,GPIO_NUM_35,GPIO_NUM_37,GPIO_NUM_40,GPIO_NUM_36,GPIO_NUM_41};
uint8_t i,j;
uint8_t my_delay=2;

void starea1(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[5], 0); 
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[1], 1);
        digitalWrite(lista_mea[4], 1);  
        ets_delay_us(my_delay);

}

void starea2(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[4], 0);
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[1], 1);
        digitalWrite(lista_mea[6], 1);
        ets_delay_us(my_delay);

}

void starea3(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[1], 0);
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[6], 1);
        digitalWrite(lista_mea[3], 1);
        ets_delay_us(my_delay);

}

void starea4(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[6], 0);
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[3], 1);
        digitalWrite(lista_mea[2], 1);
        ets_delay_us(my_delay);

}

void starea5(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[3], 0);
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[2], 1);
        digitalWrite(lista_mea[5], 1);
        ets_delay_us(my_delay);

}

void starea6(gpio_num_t  lista_mea[])
{

        digitalWrite(lista_mea[2], 0);
        ets_delay_us(my_delay);
        digitalWrite(lista_mea[5], 1);
        digitalWrite(lista_mea[4], 1);
        ets_delay_us(my_delay);
}

void initiator_ESC_rotatie_int (uint16_t delay_mare_intern=5000 )
{
	for( j=1;j<=10;j++)
	{
         starea1(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);
         starea2(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);
         starea3(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);
         starea4(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);
         starea5(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);
        starea6(ESC_PINS_1);
        ets_delay_us(delay_mare_intern - 2* my_delay);

    }
}


void initiator_ESC_rotatie_ext ()
{
	for( j=1;j<=10;j++)
	{
         starea1(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);
         starea2(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);
         starea3(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);
         starea4(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);
         starea5(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);
        starea6(ESC_PINS_1);
        ets_delay_us(delay_mare - 2* my_delay);

    }
}

void GPIO_INITIALISATION()
{

	for( i=1;i<=6;i++)
	{
		gpio_set_direction(ESC_PINS_1[i], GPIO_MODE_OUTPUT);
		gpio_set_pull_mode(ESC_PINS_1[i],GPIO_PULLDOWN_ONLY);
		gpio_set_level(ESC_PINS_1[i],0);


		gpio_set_direction(ESC_PINS_2[i], GPIO_MODE_OUTPUT);
		gpio_set_pull_mode(ESC_PINS_2[i],GPIO_PULLDOWN_ONLY);
		gpio_set_level(ESC_PINS_2[i],0);

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ESC_setup()
{               
  GPIO_INITIALISATION();

}
void BLE_setup()
{

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );                   
   
BLECharacteristic* pCharacteristic1 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );  
                                     
  pCharacteristic1->setCallbacks(new MyCallBacks()); 


  pDescr = new BLEDescriptor((uint16_t)0x2901);
  pDescr->setValue("A very interesting variable");
  pCharacteristic->addDescriptor(pDescr);
  
  pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);
  pCharacteristic->addDescriptor(pBLE2902);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

}
void INA_setup()
{
Serial.print("INA219_LIB_VERSION: ");
Serial.println(INA219_LIB_VERSION);

  Wire.begin(GPIO_NUM_1, GPIO_NUM_2);
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }

  INA.setMaxCurrentShunt(10, 0.001);
  INA.setGain(1);
  INA.setShuntResolution(12);
  INA.setShuntSamples(5);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////````````````````````````````````````

void ESC_task(void *pvParameters)
{
    
  while(1) {

for(i=0; i<10; i++)
{
  initiator_ESC_rotatie_ext();
  Serial.print("i= ");
  Serial.print(i);
}

  Serial.println (" delay_mare=");
  Serial.println (delay_mare);

UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

Serial.println(("ESC TASK: "+ std::to_string(uxHighWaterMark)).c_str()) ; 
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLE_task(void *pvParameters)
{
  
  while(1) {
UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

Serial.println(("BLE TASK: "+ std::to_string(uxHighWaterMark)).c_str()) ; 

    // notify changed value
    if (deviceConnected) {
        pCharacteristic->setValue(value);
        pCharacteristic->notify();
        value++;
        vTaskDelay(pdMS_TO_TICKS(1000));;
    }

     vTaskDelay(pdMS_TO_TICKS(1000));
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        vTaskDelay(pdMS_TO_TICKS(500));; // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void INA_task(void *pvParameters)
{

while(1) {

   //  these two can be read without further configuration.
  Serial.print(INA.getBusVoltage(), 3);
  Serial.print("\t");
  Serial.print(INA.getShuntVoltage_mV(), 3);
  Serial.print("\t");
  Serial.print(" curent in mA: ");
  Serial.print("\t");
  Serial.print( 0.001f * INA.getShuntVoltage_mV()  );

  Serial.println();

  vTaskDelay(pdMS_TO_TICKS(1000));

  UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

Serial.println(("INA TASK: "+ std::to_string(uxHighWaterMark)).c_str()) ; 
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{

Serial.begin(115200);


ESC_setup();
BLE_setup();
INA_setup();

 BaseType_t esc = xTaskCreatePinnedToCore(
     ESC_task,    // Function that should be called
    "ESC_task LED",   // Name of the task (for debugging)
    10000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    0,               // Task priority
    NULL ,            // Task handle
    1                   // core
  );

 BaseType_t ble=  xTaskCreatePinnedToCore(
    BLE_task,    // Function that should be called
    "BLE_task LED",   // Name of the task (for debugging)
    10000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    0,               // Task priority
    NULL,             // Task handle
    0                 // core 
  );

 BaseType_t  ina= xTaskCreatePinnedToCore(
    INA_task,    // Function that should be called
    "INA_task LED",   // Name of the task (for debugging)
    10000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    0,               // Task priority
    NULL,             // Task handle
    0                   // core
  );

  Serial.println(("ESC TASK: "+ std::to_string(esc)).c_str()) ; 
  Serial.println(("BLE TASK: "+ std::to_string(ble)).c_str()) ; 
  Serial.println(("INA TASK: "+ std::to_string(ina)).c_str()) ; 
 
}

void loop()
{

vTaskDelay(pdMS_TO_TICKS(100)); 

  }