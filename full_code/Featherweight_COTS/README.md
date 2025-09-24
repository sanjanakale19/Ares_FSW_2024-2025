# Rocket Project at UCLA: Featherweight COTS system  
Setup and usage for the Featherweight Altimeters GPS and Apogee Tracker, that is used as the COTS (commercial off-the-shelf) system as a backup for the 2024-2025 custom Avionics system.


## Setup   
Setup is very simple for the Featherweight. 

The Featherweight consists of two systems - the Tracker (system that is put on the rocket or vehicle) and the Ground station (handheld receiving system, connects to a phone).

Ensure the tracker is securely mounted and all wiring, specifically battery wiring, is ziptied or secured to have no strain on it. Last year's system soldered wires directly to the tracker module which then connected to the battery, and this was much more secure than batteries connected to the JST socket on the tracker.

## Battery
The Featherweight tracker and ground station both require a 3.7V battery to operate (or a 1S battery cell). Ensure that both systems (especially the tracker), are powered by a battery with at least 400mAh of battery life (many small batteries in the lab are only 150-200mAh), or an even greater charge depending on how long the tracker is required to be on. Fully charge all batteries before any tests or launch attempts.

The tracker module as well as the ground station have an ON/OFF switch, and the tracker switch must be physically turned on before any final integration/sealing of the nosecone. However, to ensure battery life is retained for as long as possible, it is strongly advised to use a pullpin in series with the Featherweight tracker battery. 

## Connectivity: Antennas and Bluetooth
The Featherweight tracker and ground station both have an RP-SMA Female antenna port, in which an RP-SMA Male 915MHz antenna must be attached. For best antenna connectivity, the tracker module's antenna must be pointed along the axis of the launch vehicle; ideally, position the tracker so the antenna points upwards. Avoid bending the antenna as it will weaken the signal.

For best connectivity, ensure that there is no metal in the plane of the antenna or the module, and limit the amount of metal near the module overall (i.e position tracker far from metal bulkheads, metal nosecone tips, etc).

To verify antenna performance, a VNA should be used to measure the VSWR (voltage standing-wave ratio) versus frequency of both antennas. A good VSWR would be a value of around 1-1.3 in the frequency band that the antenna receives/transmits in (in this case, 910-920MHz).

Bluetooth connection is mainly in reference to the handheld ground station and your phone, but the tracker also connects to Bluetooth when pairing. Once the tracker is far enough away from the phone, it will disconnect from Bluetooth and display no tracker Bluetooth connection in the app, but as long as the LoRa between the tracker and ground station is active, and the ground station is connected to the phone, tracking will work fine.

## Usage
Upon turning the ON/OFF switch of the tracker to 'ON', a red/green light will flash once to indicate power. The ground station also has a light to indicate power, as well as data packets being received from the tracker module. 

To interface with the Featherweight ground station, download the 'FeatherweightUI' app on your phone (this is the newest version of the app and has a better UI than the previous one). 

Navigate to the **Devices** column, and hit the "New Scan" button while both tracker (remote altimeter module) and receiver (ground station) are on and in range and the phone will discover them. Once each device is found by the phone, it should have a green 'BLE Connected' message for that device. If both antennas are connected, there will be a green ‘LoRa OK’ text to indicate tracker and receiver are paired. 

See tracker and receiver stats, as well as altitude information in the **Tracker** column. If it shows bluetooth to be disconnected from tracker when you are OUT OF RANGE, this is ok - this means the PHONE bluetooth cannot connect to the tracker, which is expected. As long as the Lora signal is working between the tracker + receiver, and RECEIVER bluetooth is also connected (in close range to your phone), communication will be fine.

The **Flights** column will show you previous flight information as well as statistics from the latest flight, including the apogee, a GPS map, H/V velocity, and can even generate graphs over time with these parameters. You can also export the data from your phone to csv files to analyze.


## Potential Issues
In the past years of Rocket Project, the Featherweight has been the most reliable part of our avionics system. However, we've had some launch/test attempts in which the Featherweight has failed us, and it's always been because of the same reason – if other collegiate teams are also using their Featherweights at the same time. For whatever reason, even if other teams have not paired with our Featherweight tracker module in the past, them trying to connect to their tracker sometimes causes us to lose connection with ours. 

I'm not sure what the solution to this is other than making sure all other teams are not trying to access a Featherweight/have the app open at the same time as our launches/tests.
