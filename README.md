# GPS-Fan-Controller
Arduino Pro Mini GPS Fan Controller for my Honda Super Blackbird motorbike

The Arduino Pro Mini is used to set up the UBlox GPS Module (NEO-8M) via a serial interface and then extract the relevant GPS data from the module, once a second, using the "NAV-PVT" UBX binary dataset.
Additionally, the microcontroller also sends information on: 
  - speed,
  - number of satellites in range, and
  - fan relay state
via a license-exempt low power 433MHz transmitter module to a separate wireless receiver unit (see Wireless Receiver Unit).

The ubx.h file contains all the specific code for the UBlox GPS Module, including  the control data packets which turn off the default NMEA ascii string data packets and turns on the UBX binary NAV-PVT packet.  It also contains the NAVPVT data structure.

The ask.h file contains code relevant to the 433MHz low power transmitter module.

The file Pro_Mini_GPS_Transmitter.ino is the main program, developed within the Arduino IDE and main.h is a simple header file.

If the Fan Controller is being installed without a Wireless Receiver Unit, then the 433MHz transmitter module is not needed (and its associated code, including ask.h, can be removed )



