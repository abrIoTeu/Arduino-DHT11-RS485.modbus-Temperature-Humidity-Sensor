// abrIoT DHT11 RS-485 modbus Temperature & Humidity Sensor Firmware
// Created by: Lantos, Attila
// Creation date: 26-01-2023
// Version: 1.0
// www.abriot.eu

// Libraries
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoModbus.h>

// Setup -> You may change this
#define MODBUSADDRESS 2
#define MODBUSBAUD 9600

int interval = 5000; // Sensor reading interval in milliseconds
int timeout = 1000; // Modbus polling and sensor reading timeout in milliseconds.

bool debug = true; // If true -> Serial monitor will be used to display messages

// DHT11 Sensor -> Please DO NOT change these
#define DHTPIN 3
#define DHTTYPE DHT11
DHT DHT(DHTPIN, DHTTYPE);

// Variables -> Please DO NOT change these
float temperature = 0;
float last_temperature = 0;
float humidity = 0;
float last_humidity = 0;
int temperature_int = 0;
int humidity_int = 0;
long last_time = 0;
long start_time = 0;
bool startup = true;
bool error = false;
int status = 0;
bool modbus = false;
String status_text[4] = {"OK", "Modbus error", "Sensor error", "Timeout error"};

// Setup
void setup() {  
  pinMode(DHTPIN, INPUT); // DHT11 Sensor pin setup
  setupGreeting();
  RS485.begin(9600);  
  DHT.begin();
  setupModbus();      
}

// Loop
void loop() {
    start_time = millis();
    while (millis() - start_time <= timeout) {
      ModbusRTUServer.poll();
      readSensor();      
    }                 
}

// Custom functions
void setupGreeting() {
    if (debug) {     
    Serial.begin(9600);
    Serial.println("DHT-11 Temperature & Humidity Sensor");
    Serial.println("==========");
  }  
}

void setupModbus() {
    if (ModbusRTUServer.begin(MODBUSADDRESS, MODBUSBAUD)) {        
    if (debug) {
      Serial.print("Modbus RTU Server started at address: ");
      Serial.print(MODBUSADDRESS);
      Serial.print(" with baudrate: ");
      Serial.println(MODBUSBAUD);
      Serial.println("----------");
    }      
    ModbusRTUServer.configureHoldingRegisters(1,3);
    // Register 1 = Temperature * 100
    // Regsiter 2 = Humidity * 100
    // Register 3 = Status code    
    modbus = true;
    setStatus(0);      
  } else {
    modbus = false;
    setStatus(1);    
    if (debug) {
      Serial.print("Modbus RTU Server startup FAILED at address: ");
      Serial.print(MODBUSADDRESS);
      Serial.print(" with baudrate: ");
      Serial.println(MODBUSBAUD);
      Serial.println("----------");            
    }    
  }
} 

void readSensor() {
  bool _changed = false;
  if (millis() - last_time >= interval || startup == true) {
      startup = false;
      temperature = DHT.readTemperature();
      humidity = DHT.readHumidity();
      if (temperature != last_temperature) {        
        _changed = true;
        temperature_int = (int) (temperature * 100);        
        ModbusRTUServer.holdingRegisterWrite(1, temperature_int);
        last_temperature = temperature;
      } else if (humidity != last_humidity) {
        _changed = true;
        humidity_int = (int) (humidity * 100);        
        ModbusRTUServer.holdingRegisterWrite(2, humidity_int);
        last_humidity = humidity;
      } else {
        _changed = false;
      }          
      if (_changed && debug) {
        Serial.print("Temperature: ");
        Serial.print(temperature);      
        Serial.print(" C | Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
        Serial.println("----------");
      }      
      last_time = millis();
    }
}

void setStatus(int _status) {
  status = _status;
  if (modbus) {
    ModbusRTUServer.holdingRegisterWrite(3, _status);
  }  
}