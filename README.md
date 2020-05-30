# RbRemo1

This is the firmware of Turcotronics Remo1 remote for the Robello T1 robots.
It runs on a ESP32 NodeMCU-32S board.
It can be used to control your robots too, both hardware and software are open :-)
You can buy the remotes, the robots or parts of them from https://turcotronics.it
In the robot the WiFi and Bluetooth connection point is a ESP32-CAM,
check https://github.com/turcotronics/RbT1E32C for the robot firmware.
The robots can be controlled from remotes, phones, tablets and computers.

This project was originally forked from https://github.com/bkeevil/esp32-cam


## License

 Modified ESPRESSIF MIT License
 
 Copyright (c)  - <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
                - <Bond Keevil (bkeevil), https://github.com/bkeevil/esp32-cam>
                - <Turco Rodolfo, Turcotronics, https://turcotronics.it>
 
 Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 it is free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 to do so, subject to the following conditions:

 The names of Turcotronics, Robello and TuT may not be used to endorse or promote
 products derived from this software without specific prior written permission.
 
 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## Installation

The source code requires the [installation of the ESP-IDF toolchain and development framework]
(https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)

**The source code works with the release/v4.0 branch.**

Once that is done, create a folder for your projects and set the path and environment variables of your shell:

```
cd /home/username/esp/esp-idf
. ./export.sh
cd ..
mkdir myprojects
cd myprojects
```

Clone the repository using the recursive option:

```
git clone --recursive https://github.com/bkeevil/esp32-cam.git
cd esp32-cam
```

Compress the stylesheet, html and javascript pages in main/www:

```
cd main/www
./compress_pages.sh
cd ../..
```

From the root of the repository source tree, create a build directory and then run CMake:

```
cd /path/to/repository
mkdir build
cd build
cmake ..
cd ..
```

Then, to configure the source code you need to run `make menuconfig` from the root of the source tree. This will bring up an ncurses based repository editor.

Important settings are:
- Select a USB port for uploading the firmware under *Serial Flasher Config -> Default serial port*
- Select what board you are using under *Camera Web Server -> Camera Pins*
- You can select default wifi settings under *Camera Web Server -> Wifi Settings*
- Under *Component Config* pin the WiFi and Bluetooth tasks to Core0
- Enable NTP and select a default NTP server and timezone under *NTP configuration*

When your settings are complete, save them and exit.

Compile the source code by executing `make app`

Flash the binary file to your ESP32 module using `make flash` 

Monitor the debug serial output by running `make monitor`


### Build Notes

Instead of using make xxxx commands, you can use idf.py xxxx commands, it's my preferred way on Ubuntu.

```
idf.py build
idf.py menuconfig
idf.py app
idf.py -p /dev/ttyUSB0 app-flash
```


### Windows Notes

Start ESP-IDF Command Prompt from Windows Start Menu to have idf.py available automatically.
To further simplify the build process, create a build-windows-defaultcomport file (without any extension) with your devices COM port, e.g. COM15, then from within the ESP-IFD prompt run the build script.
```
echo COM15 > build-windows-defaultcomport
powershell.exe .\build-windows.ps1
```


## First Run

When the firmware first starts up, it will try to establish a wifi connection in the following order:

1. Establish a connection using the SSID and password stored in non-volatile storage (nvs)
2. Establish a connection using the default STA SSID and password provided by `make menuconfig`
3. After five failed attempts at starting in station (STA) mode, it will fall back to SoftAP mode and act as a wifi access point

To connect to the access point, point a browser to 192.168.4.1. There is no username or password.

From the web interface, select a Wifi SSID and password and optionally specify a static IP address and other network settings. Reboot the device for the settings to take effect.

