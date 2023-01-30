# abrIoT DHT11 RS-485 modbus capable Temperature & Humidity Sensor

## Table of contents
* [Introduction](#introduction)
* [Technical parameters](#technical-parameters)
* [Technologies used](#technologies-used)
* [Dependencies](#dependencies)
* [Launch](#launch)
* [Examples of use](#examples-of-use)

## Introduction
A simple Arduino based Firmware for abrIoT DHT11 RS-485 modbus capable Temperature & Humidity Sensor.

## Technical parameters
| Description | Technical parameter |
| :--- | :--- |
| MCU | ATMEGA 328P (Arduino Nano) |
| Sensor | DHT-11 |
| Communication protocol | modbus RTU via RS-485 |
| Power supply | 6-24V AC/DC or USB |
| Power consumption | ca. 0.5W |

## Technologies used
* Arduino (C++)
* Modbus RTU communication via RS-485

## Dependencies
* Arduino modbus library
* Arduino RS-485 library
* Adafruit DHT-11 sensor library

## Launch
To launch the project, download the sketch and the dependencies as well. You may modify the project for personal usage and for contribution. Please note that, in accordance our license, **no commercial usage is allowed**.  

Please revise the variables like the timeout.  

Modbus regsiters:
* 1: Temperature * 100 - stored as integer (uint16)
* 2: Humidity * 100 - stored as integer (uint16)
* 3: Status code - integer (uint16)

## Examples of use
abrIoT sensor may be used for home automation projects and for development (prototyping) purposes. Since the sensor transfers the data via modbus, it may be linked to a PLC or HMI as well.


