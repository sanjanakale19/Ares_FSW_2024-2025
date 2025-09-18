#Rocket Project at UCLA: Ares Flight Software 2024-2025  
####Encompassing all code for Rocket Project 2024-2025 Avionics Systems.  

##**About**  
This code is intended to be used as a learning tool and knowledge transfer for future Avionics teams in Rocket Project at UCLA. Much of the code is system-specific and pinout-specific, so in order to utilize it for different PCBs, pinouts throughout the code base will need to be changed, as well as the serial communication protocol if it is different.  

Furthermore, available library code is not a substitute for proper electronics handling and knowledge. Code will not function in the way it is intended if parts are placed wrong on a PCB, if components are provided with the wrong voltage/current, or if PCB is not constructed with close attention to ESD, contact with metal, E-housing, and connector continuity. It's integral to ensure every part of the hardware functions as intended before making major changes to software. 

##**Dependencies**  
* Git
* Python3
* C/C++
* PlatformIO
* Telegraf
* Grafana
* VSCode
* Coolterm



## File System Organization  

**```full_code```** all finished code for bodytube, nosecone, ground station, SD dumping, GUI.  
**```sensor_tests```** individual tests for ADS1256, CAN, ground station.  
**```OLD_nosecone_groundstation```** deprecated/previous testing code for nosecone/ground station transmission. 


## Installation Instructions
System-specific instructions provided in the README.md files of each subdirectory within **`full_code`**. 

**General installation instructions:**  
1. Ensure that your chosen IDE is compatible with PlatformIO – VSCode is recommended for this purpose. After copying/cloning the repositories in VSCode, navigate to the sidebar **Extensions > PlatformIO IDE** and install. Wait for the pop-up dialog that initializes PlatformIO for the given project, and the compilation errors should disappear.  
2. Using your favorite python package manager (e.g homebrew/brew for Mac, pip for Windows, etc), ensure that at least the above are installed: (Python3, C/C++ GCC, Git)  
3. For best results, **before flashing any new code**, ensure that the hardware meets the criteria defined in the README for the subfolder/system. Criteria includes PCB continuity checks, hardware power-on check, test point voltage checks, etc.  

_Tip: only open one subdirectory at a time that contains a 'main.cpp' otherwise program may fail to compile (from duplicate definitions of main)_

##**PlatformIO .ini files**  
One quirk of PlatformIO is that it requires a `platformio.ini` file to be present within each subfolder to configure the hardware that the code will be compiled for. For example, different ESP32s (e.g. ESP32 breakout board versus ESP32-s3-devkitc) will need different parameters in this .ini file due to the MCUs being different.

####**PlatformIO .ini examples**  
---
For the ESP32-S3-DEVKITC:

	[env:esp32-s3-devkitc-1]  
	platform = espressif32  
	board = esp32-s3-devkitc-1  
	framework = arduino  
	build_flags =  
	   -DARDUINO_USB_MODE=1  
	   -DARDUINO_USB_CDC_ON_BOOT=1  
	monitor_speed = 115200  			# baud rate
	lib_deps = https://github.com/handmade0octopus/ESP32-TWAI-CAN  # directly adds github library from link

For these types of ESPs, ensure that `-DARDUINO USB_CDC_ON_BOOT` is set to 1, otherwise the ESP32 will not output anything to serial.

---

For the typical ESP32 Breakout Boards:

	[env:esp32doit-devkit-v1]
	platform = espressif32
	board = esp32doit-devkit-v1
	framework = arduino
	monitor_speed = 115200
	lib_deps = 
		https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library.git  


## **Full Code Filetree**
The essential FSW for the Ares Avionics 2024-2025 systems lives in `full_code`. The five subdirectories and contents as follow are for the essential telemetry hardware (Nosecone System and Bodytube System), and the data receiving hardware (Ground System). 

**Nosecone System** –> `nosecone_newestsys`  

**Bodytube System** –> `bodytube_flight ` 
 
**Ground System** –> `ground_station `, `sd_dump`, `GUI`

Additional Avionics hardware used on the 2024-2025 systems, including both the `Ad-Hoc Camera System` and the `Featherweight Altimeters COTS/backup System` are also included in the file tree with setup instructions.

**```full_code```**   
|
`bodytube_flight/src`  
|
–––––  `ADS1256.h`  
|
–––––  `ADS8688.h`  
|
–––––  `CANTX.h`  
|
–––––  `Globals.h`  
|
–––––  `HAL.h`  
|
–––––  `INA.h`  
|
–––––  `MS.h`  
|
–––––  `XTSD.h`  
|
–––––  `main.cpp`  
|
–––––  `README.md`  

|
`nosecone_newestsys/src`  
|
–––––  `BMP.h`  
|
–––––  `CANRX.h`  
|
–––––  `GPS.h`  
|
–––––  `Globals.h`  
|
–––––  `HAL.h`  
|
–––––  `ICM.h`  
|
–––––  `INA.h`  
|
–––––  `MS.h`  
|
–––––  `Radio.h`  
|
–––––  `XTSD.h`  
|
–––––  `main.cpp`  
|
–––––  `README.md`  

|
`ground_station/src`  
|
–––––  `main.cpp`  
|
–––––  `README.md`  

|
`sd_dump/src`  
|
–––––  `Globals.h`  
|
–––––  `HAL.h`  
|
–––––  `SD_functions.h`  
|
–––––  `main.cpp`  
|
–––––  `stream.h`  
|
–––––  `README.md`  

|
`GUI`  
|
–––––  `Avionics_GUI_test.py`  
|
–––––  `telegraf.conf`  
|
–––––  `README.md`  

|
`Ad-Hoc_Camera_HW`   
|
–––––  `README.md`  

|
`Featherweight_COTS`  
|
–––––  `README.md`  