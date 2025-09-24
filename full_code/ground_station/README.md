# Rocket Project at UCLA: Ground Station PCB


## Mechanical choices


## Serial connection


## Power choices
reg + power switch + test points

## Radio connectivity
1. LoRa module + wiring + TXEN, RXEN 
2. layout requirements + power requirements
3. antenna requirements + how to measure antenna  


## Ground Receiving
The main functionality of the Ground PCB, receiving radio packets from the Nosecone system using a LoRa radio module. Code usage information found under `./ground_receive`.


## SD dump
Secondary functionality of the Ground PCB, to easily see file information and print file data from SD card breakout. Code usage information found under `./sd_dump`.


## GUI/Grafana setup
A visualization for the data received from the 'Ground Receiving' code, rather than interpreting values directly from serial prints.


## General + Component-specific Troubleshooting 

