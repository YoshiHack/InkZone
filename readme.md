<h1 align="center">InkZone</h1>

## InkZone is a Wi-Fi e-paper sports display that displays information like Score, and Time

### Access the demo at: https://wokwi.com/projects/470194904712457217

### What you can display 
You can display sports information like Upcoming dates, times even live scores. So far you can view these leagues:

- NFL
- NCAA Football
- NBA
- NCAA Basketball
- NHL

### How it works
InkZone works by connecting to the internet via Wi-Fi then pulls from the unofficial ESPN scoreboard source then the software translates that data and displays it on the e-ink display

### Features
Inkzone has many features, including but not limited to;

- Webpage that you can change settings, favorite teams, Wi-Fi settings, and timezone.
- If not connected to a Wi-Fi it will broadcast i'ts own Wi-Fi where you can access the webpage 
- You can install buttons to open the different pages (More on that in a bit)

### Pages
So far Inkzone has 2 pages (both screens also have scoring animations now too):

##### Page 1
displays 5 upcoming games of the league you select (See ==Webpage==)
<img src="images\page1.png" width="400">
 
##### Page 2
displays one singular game (The game will be the team you favorited in the webpage) and it will also display the date and time. 

<img src="images\page2.png" width="400">

##### Page 3
This dashboard displays all upcoming favorite games

<img src="images\page3.png" width="400">


##### Auto and Manual modes
There are 2 modes auto and manual by default InkZone is on auto In automatic mode, InkZone prioritizes a live favorite game, then other live games, then an upcoming favorite game, then the selected league scoreboard, and finally the idle dashboard. The user can temporarily change views using the buttons or website controls. page 2 if there are more than one games it will display page 2, if there are no games it will display page 1 (you can change them on the webpage or with the buttons)

### Webpage
The Webpage can let you change these items:

- Wi-fi Networks
- Timezone
- Favorite league
- Favorite team abbreviations (more than 1)
- Update interval in seconds
- Display controls (See Pages)

##### How to access it
You can access the webpage via connecting to the hotspot it created (See ==Features==) and going to 192.168.4.1:90 or if its connected to Wi-Fi <The Inkzone's IP>:8090

### How to make one
#### This guide covers both the Wokwi simulator and the planned physical e-paper build.

> [!IMPORTANT]
> The Wokwi version uses an ILI9341 LCD because Wokwi does not simulate the planned 7.3-inch Spectra 6 panel. The physical display uses a different driver and wiring. Do not copy the Wokwi display wiring directly to the e-paper HAT.

- ESP32 DevKit V1
- Waveshare 7.3-inch e-Paper HAT (E), including its driver board
- Eight female-to-female jumper wires
- USB data cable for the ESP32
- USB power supply
- A computer with Wi-Fi
- Optional enclosure and mounting hardware

Buy the HAT version with the driver board. A raw e-paper panel cannot be connected directly to an ESP32.

See [BOM.md](BOM.md) for the current bill of materials.

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- A copy of this repository
- Optional: a free [Wokwi](https://wokwi.com/) account

Testing in Wokwi is the safest way to check the firmware before buying or connecting hardware.

1. Open the InkZone folder in Visual Studio Code.
2. Install the PlatformIO extension if it is not already installed.
3. Open the PlatformIO sidebar.
4. Select **Project Tasks**, then **esp32dev**, then **Build**.
5. Wait for the terminal to report `SUCCESS`.
6. Start the Wokwi simulation.

The repository's `diagram.json` connects the simulated display as follows:

| ILI9341 signal | ESP32 pin |
| --- | --- |
| VCC | 5V |
| GND | GND |
| CS | GPIO 15 |
| D/C | GPIO 2 |
| MOSI | GPIO 23 |
| SCK | GPIO 18 |
| LED | 3V3 |

The simulated buttons use these pins:

| Button | ESP32 pin | Other terminal |
| --- | --- | --- |
| Previous | GPIO 32 | GND |
| Select | GPIO 33 | GND |
| Next | GPIO 25 | GND |

The firmware enables the ESP32's internal pull-up resistors, so the buttons do not need separate resistors in the Wokwi circuit.

Confirm that the package contains:

- The 7.3-inch e-paper panel
- The Waveshare driver HAT
- The display ribbon cable
- The eight-pin SPI cable

Do not bend the display, press on its visible area, or connect the raw ribbon cable directly to the ESP32.

Unplug the ESP32 before changing any wiring. Connect the display only while the system is powered off.

The current physical-display source uses this proposed SPI pin map:

| E-paper HAT signal | ESP32 pin | Purpose |
| --- | --- | --- |
| VCC | 3V3 | Display and logic power |
| GND | GND | Common ground |
| DIN | GPIO 23 | SPI MOSI |
| CLK | GPIO 18 | SPI clock |
| CS | GPIO 15 | Chip select |
| DC | GPIO 27 | Data/command selection |
| RST | GPIO 26 | Display reset |
| BUSY | GPIO 25 | Refresh status from display |

The Waveshare HAT accepts 3.3 V or 5 V, but its documentation says the I/O voltage must match the supply voltage. Powering it from the ESP32's 3.3 V pin keeps both power and logic at 3.3 V.

> [!WARNING]
> GPIO 25 is the e-paper BUSY signal in the physical display code, but it is the Next button in the Wokwi configuration. Do not connect both to GPIO 25. The physical build should omit that button until the firmware assigns it a different pin.

The current `platformio.ini` contains:

```ini
build_flags =
    -D INKZONE_WOKWI_SIMULATOR
```

That flag selects the Wokwi ILI9341 display. Remove the flag before compiling for physical e-paper hardware:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
board_build.partitions = partitions.csv
framework = arduino
monitor_speed = 115200
extra_scripts = post:merge_firmware.py
lib_deps =
    bblanchon/ArduinoJson @ ^7.4.2
    zinggjm/GxEPD2 @ ^1.6.9
    adafruit/Adafruit ILI9341 @ ^1.6.3
```

> [!CAUTION]
> The physical driver in `src/core/epaper_display.cpp` is still configured for a 2.9-inch black-and-white test panel. The planned 7.3-inch Spectra 6 display needs its correct driver class and six-color rendering implementation before the physical build is ready to flash. First run the display manufacturer's example for the exact HAT revision, then update InkZone's display adapter.

In Visual Studio Code:

1. Open the PlatformIO sidebar.
2. Select **Project Tasks**.
3. Open **esp32dev**.
4. Choose **Build**.
5. Fix any build errors before connecting the board.

You can also build from a terminal:

```powershell
platformio run --environment esp32dev
```

1. Connect the ESP32 with a USB data cable.
2. In PlatformIO, choose **Upload**.
3. Open **Serial Monitor** at `115200` baud.
4. If uploading does not start, hold the ESP32 **BOOT** button while PlatformIO begins the upload, then release it.

The serial monitor should eventually show:

```text
InkZone starting...
```

When InkZone has no saved Wi-Fi network, it creates this temporary access point:

```text
InkZone-Setup
```

1. Connect a phone or computer to `InkZone-Setup`.
2. Open `http://192.168.4.1:90` in a browser.
3. Select the home Wi-Fi network.
4. Enter its password.
5. Choose a timezone, league, and favorite team abbreviation.
6. Save the settings.
7. Allow the ESP32 time to reconnect.

Do not post screenshots or logs containing a real Wi-Fi password.

After InkZone joins the home network, open its local IP address on port 90:

```text
http://INKZONE-IP:90
```

The exact IP address appears in the serial monitor or in the home router's connected-device list.

InkZone supports two display-control modes:

- **Automatic mode** chooses the most useful screen based on live and upcoming games.
- **Manual mode** lets the user select a screen from the settings website or optional buttons.

The device currently supports:

- NFL
- NCAA football
- NBA
- NCAA basketball
- NHL

Sports data comes from an unofficial ESPN scoreboard endpoint. That endpoint may change or become unavailable, so InkZone preserves the most recent valid scoreboard when possible.

Read the first error in the terminal rather than the last line. The first error is usually the cause of the later errors.

- Confirm that the display driver matches the exact panel model.
- Confirm VCC and GND before checking signal wires.
- Confirm DIN, CLK, CS, DC, RST, and BUSY one at a time.
- Keep jumper wires short.
- Wait for the full refresh to finish; the planned color display can take about 25 seconds for a full refresh.
- Never start another refresh while BUSY indicates that the display is working.

- Confirm the phone or computer is connected to `InkZone-Setup`.
- Turn off mobile data temporarily if the phone keeps leaving the setup network.
- Enter `http://192.168.4.1:90` exactly, including `http://` and port `90`.
- The setup network has no internet access; that is expected.

- Confirm the ESP32 is connected to the internet.
- Check the serial monitor for the HTTP status and parser diagnostic.
- Make sure the selected league has games available for the requested date.
- Restart the ESP32 and let its clock synchronize.

Wokwi's simulated storage may reset depending on how the simulation is started. Test permanent saved settings again on the physical ESP32 before relying on them.

- Test every screen and supported color.
- Test Wi-Fi loss and reconnection.
- Unplug and restart the device several times.
- Confirm that saved settings survive a restart.
- Confirm the display cable is not pinched or sharply bent.
- Keep pressure away from the visible panel.
- Provide access to USB, reset, and any buttons.
- Verify that the mounting system cannot let the display fall.

The networking, sports-data parsing, settings website, automatic screen selection, Wokwi display, and multiple leagues have been prototyped. The final 7.3-inch Spectra 6 driver, color renderer, physical pin-conflict cleanup, and real-hardware validation still need to be completed before this guide can be considered a plug-and-play physical build.

For the display's electrical requirements and exact revision information, consult the [official Waveshare 7.3-inch e-Paper HAT (E) manual](https://www.waveshare.com/wiki/7.3inch_e-Paper_HAT_%28E%29_Manual).
