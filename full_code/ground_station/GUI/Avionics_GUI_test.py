#from serial import Serial
import socket
import time
import re
import os
import random
from serial import Serial
import serial.tools.list_ports as list_ports
import os

ports = list(list_ports.comports())
for i,p in enumerate(ports):
    print(f"{i}: {p.name}")

port_num = int(input("Which port:"))

PORT = ports[port_num].name

# MODIFY PORT AND BAUDRATE
#PORT = '/dev/cu.usbserial-0001' 
# PORT = '/dev/cu.SLAB_USBtoUART' #--> UNCOMMENT THIS
# PORT = '/dev/cu.usbmodem131488301'
BAUDRATE = 9600
MAX_COUNT = -31 #######Change this

measurement = 'avionics'
'''
s += String(XTSD::logTime)
String(MS::temp, 2) + "," + String(MS::pressure, 2) + "," + String(MS::altitude, 2) + ",";
String(BMP::temp1, 2) + "," + String(BMP::pressure1, 2) + "," + String(BMP::altitude1, 2) + ",";
String(ICM::accel_x, 2) + "," + String(ICM::accel_y, 2) + "," + String(ICM::accel_z, 2) + ",";
String(ICM::gyro_x, 2) + "," + String(ICM::gyro_y, 2) + "," + String(ICM::gyro_z, 2) + ",";
String(GPS::latitude, 6) + "," + String(GPS::longitude, 6) + "," + String(GPS::altitude, 2) + "," + String(GPS::SIV) + ",";
String(INA::bus_voltage, 3);
'''
# gps coordinates lat and long need to be named "latitude"/"lat" and "longitude"/"long" in order to display on geomap
# lat / long is for nosecone gps
# latitude and longitude is for computer gps
nosecone_keys = ["micros", "logtime", "temp1", "pressure1", "alt1", "temp2", "pressure2", "alt2", "accelx", "accely", "accelz","gyrox", "gyroy","gyroz", "lat", "long", "gps_alt", "SIV", "bus_voltage", "logsuccess"]
bodytube_keys = ["micros_bt", "logtime_bt", "temp_bt", "pressure_bt", "alt_bt", "pt0", "pt1", "pt2", "pt3", "loadcell_weight", "bus_voltage_bt", "logsuccess_bt"]
radio_keys = ["rssi","error","snr"]
additional_keys = ["accel_total", "latitude", "longitude", "geodesic_distance", "geodesic_bearing"]

# pt0 = ox
# pt1 = press
# pt2 = fuel
# pt3 = chamber

# just in case
def getTime():
    return time.time_ns()

start_time = str(getTime())
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# DO I NEED TO CHANGE SERVER ADDRESS PORT????
serverAddressPort = ('127.0.0.1', 20348)
ser = Serial("/dev/" + PORT, BAUDRATE, timeout=0.1)         # for Mac ONLY, append "/dev/" before port otherwise it won't be located

# Modify value of N for varying smoothness
N = 3
packet_counter = 0


while True:
    timestamp = str(getTime())

    # eventually change try catching to ignore bad inputs and preserve acceptable ones
    try:
        serial_in = str(ser.readline().decode("utf-8")) #--> UNCOMMENT THIS
        # print(serial_in)
        serial_in = serial_in.strip("\r\n")
        # print(serial_in)
    except:

        print("serial read issue: " + str(serial_in))
        serial_in = ""
    
    line = serial_in

    fields = ''

    try:
        args = line.split('\'')    # args[0] is empty, args[1] = data params, args[2] is radio params

        # nosecone_fields = args[1].split(',')
        # split() args[1] by | and the 0th will be nosecone, 1st will be bodytube

        nosecone_string = ""
        bodytube_string = ""
        bodytube_fields = ()
        fields = "" # combined fields used for nosecone and bodytube (can be None)

        try:
            # Case 1: body and nosecone tx
            data_params = args[1].split("|")
            nosecone_string = data_params[0]
            bodytube_string = data_params[1] # should throw index error if DNE

            bodytube_fields = bodytube_string.split(',')
            # combine all bodytubes data values together, separated by comma
            for key, val in zip(bodytube_keys, bodytube_fields):
                fields += f'{key}={val}'
                # if (key == 'bus_voltage_bt') : break
                fields += ','

        except:
            # Case 2: only nosecone transmitted
            nosecone_string = args[1]
            bodytube_fields = {}
        
        nosecone_fields = nosecone_string.split(',')
        # combine all nosecone data values together, separated by comma
        for key, val in zip(nosecone_keys, nosecone_fields):
            fields += f'{key}={val}'
            # if (key == 'bus_voltage') : break
            fields += ','




        radio_fields = args[2].split(',')
        rssi = radio_fields[1]
        error = radio_fields[3]
        snr = radio_fields[5]

        # print(rssi)
        # print(error)
        # print(snr)
        packet_counter = 0

        

        # manually add radio parameters
        fields += "rssi=" + rssi + ","
        fields += "error=" + error + ","
        fields += "snr=" + snr
    except:
        print("serial read issue")

    # create influx string
    influx_string = measurement + ' ' + fields + ' ' + timestamp
    print(influx_string)
    UDPClientSocket.sendto(influx_string.encode(), serverAddressPort)
    # time.sleep(0.35)
