// abrIoT DHT11 RS-485 modbus Temperature & Humidity Sensor Firmware
// Created by: Lantos, Attila
// Creation date: 26-01-2023
// Version: 1.3
// www.abriot.eu
// Board: ESP8266

// Dependencies:
//  - Aruduino Modbus & RS485 libraries
//  - Adafruit DHT-11 library
//  - Wire (I2C)
//  - Adafruit SSD1306 library (OLED)
//  - Adafruit GFX library (OLED)

// Libraries
#include <dummy.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoModbus.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Setup -> You may change this
#define BAUD 9600 // Serial baudrate
#define MODBUSADDRESS 2 // Sensor device modbus RTU address
#define MODBUSBAUD 9600 // Default modbus RTU baud rate
float correction = -1.6; // Temperature correction due to calibration
int interval = 5000; // Sensor reading interval in milliseconds
int timeout = 1000; // Modbus polling and sensor reading timeout in milliseconds.
bool debug = false; // If true -> Serial monitor will be used to display messages
bool oled = true; // If true -> OLED display will be used

// DHT11 Sensor -> Please DO NOT change these
#define DHTPIN 2 // DHT11 sensor pin number (Default is GPIO0 for the ESP8266 module)
#define DHTTYPE DHT11
DHT DHT(DHTPIN, DHTTYPE);

// OLED display -> Please DO NOT change these
#define SCREEN_WIDTH 128 // OLED display width in pixels
#define SCREEN_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
// String status_text[4] = {"OK", "Modbus error", "Sensor error", "Timeout error"};

// Setup
void setup() {  
  pinMode(DHTPIN, INPUT); // DHT11 Sensor pin setup
  DHT.begin();
  setupOLED();
  setupGreeting();     
  setupModbus();              
}

// Loop
void loop() {
    start_time = millis();   
    while (millis() - start_time < timeout) {
      // ModbusRTUServer.poll();
      readSensor();       
      if (millis() - start_time >= timeout) {
        setStatus(3);
      }
    }
    ModbusRTUServer.poll();
}

// Custom functions
void setupGreeting() {
    long _start_time = millis();
    if (debug) {     
      Serial.begin(BAUD);
      Serial.println("abrIoT DHT-11 Temperature & Humidity Sensor");
      Serial.println("==========");
    if (oled) {
      display.clearDisplay();
      display.setCursor(20, 0);
      display.setTextSize(1);      
      display.setTextColor(WHITE);
      display.println("abrIoT DHT-11 Temperature & Humidity Sensor");
      display.println("==========");
      display.display();
      if (millis () - _start_time >= 3000) {
        display.clearDisplay();
        display.display();
      }
    }
  }  
}

void setupOLED() {
  if (oled) {  
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        if (debug) {
          Serial.println("SSD1306 allocation failed!");
          Serial.println("---------");
          return;
        }
      }      
      display.cp437(true);
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
      temperature = DHT.readTemperature() + correction;
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
      } else if (isnan(humidity) || isnan(temperature)) { // Testing for NaN condition      
        setStatus(2);
        _changed = false;
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
      if (_changed && oled) {
        display.clearDisplay();
        display.setCursor(0, 10);
        display.setTextColor(WHITE);
        display.setTextSize(3);               
        display.print(temperature);      
        display.println(" C");
        display.setCursor(25, 40);
        display.setTextSize(2);        
        display.print(humidity);
        display.print(" %");
        display.display();        
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