# Intro
This is the senior design project created by Angel Cruz, Jose Guerra Fajardo, and Miguel Angel Isasi Cedillo. 

# Objective
The objective of this project was to create an Uninterruptible Power Supply (UPS) System. The UPS system serves two purposes, to provide backup power during main grid power outages, and to help save elactrical consumption costs through a power saving mode called "peak hours" mode. 

The UPS system has been designed to be implemented in a hospital Intensive Care Unit (ICU) room. The UPS system will be able to accept up to two 120V AC inputs and will have an internal 12V 150Ah lithium battery which will be capable of supplying up to 200W for 4 hours under full load. The battery will be able to be fully charged in 7.5 hours via an internal 20A battery charger. 

The peak hours mode will serve as a way to help reduce consumer electricity costs by allowing the UPS load to be powerd by the internal battery during the hours of the day that electricity costs are the highest.

# Specifications
* (2) 120V AC inputs, Fused
* (2) 120V AC Outputs, Fused
* 250 watt load capacity
* 4.5 Hours Approximate Backup Time at Full Load
* 7.5 Hours Approximate Battery Charge Time (from fully discharged)
* Peak Hours Mode
* Pure Sine Wave Output
* Online UPS Topology

# BOM
The components used for the UPS system are listed in an excel file. The main components of the UPS system consisted of a step-down transformer, custom designed AC/DC rectifier, lithium battery, battery charger, inverter, and Arduino Mega 2560. PCB files are also available for the custom AC/DC rectifier. 

# AC/DC Rectifier
The rectifier for this UPS was custom designed to be able to support the required current. Included with the rectifier is a linear voltage regulator curcuit which was set to output 20A at 15.5V. 
In order to be able to supply enough current for full 300W output, two rectifier boards had to be utilized. One rectifier supplies just the battery charger and the other supplies the inverter (see provided overall UPS schematics). Each rectifier board was tested up to 20A output. 

The transistors were mounted onto a dedicated heatsink block with fans for cooling.

IMPORTANT: when ordering PCB, make sure outer layer copper thickness is set to 2oz. 

# Disclaimer
High Voltage!
The author / associates / owner of this project cannot be held responsible for any damages in the form of physical loss / monetary loss or any kind of loss that comes as the result of making projects that are presented in this website.

