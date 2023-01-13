# ArduDesk
Arduino (ESP8266-based) IoT Desk Controller

## What is it?

ArduDesk is an "intelligent" desk controller designed to work with [Uplift V2 standing desks](https://www.upliftdesk.com/standing-desks/) and allows for automated control via home-automation systems. This repository contains the firmware, schematics, PCB design files, and OpenHAB integration.

## Why did you make it?

Because why not? Main I just wanted to be able to control my desk from my phone.  The buttonpad that came with my desk allows raising/lowering and nothing else. No presets or anything. I quickly realized that I wanted to be able to just hit a button to go to a preset height for when I'm sitting and another for when I want to stand. ArduDesk fills those requirements and then some.

## But like... you can order a replacement handset that does that.

You are correct. Uplift does indeed make [varying different control pads that can do that](https://www.upliftdesk.com/advanced-comfort-flush-keypad-by-uplift-desk/) and they even make a [bluetooth adapter](https://www.upliftdesk.com/bluetooth-adapter-for-uplift-desk/). But I wanted to integrate with the rest of my home automation ecosystem, which opens up a bunch of other possibilities.

## So how does it work?

ArduDesk is designed to be a man-in-the-middle device. Meaning, can continue to use your existing button pad *and* ArduDesk due to ArduDesk's passthrough circuitry. The idea is you simply unplug your existing button pad from the desk's control box, and plug it into ArduDesk instead. Then take a regular network patch cable from ArduDesk to the control box. That is it in terms of connections. ArduDesk get's it's power from the control box and connects to your network over WiFi.

## Ok.. I'm sold. How do I get one?

I don't sell this as a product you can buy or even as a kit. You can grab the gerber files from the 'schematics' folder and send them to a PCB fab house like [PCBWay](https://www.pcbway.com) and have the boards made, then using the Bill-of-Materials (BOM) from the 'schematics' folder you can source the components from places like [DigiKey](https://www.digikey.com) and then whip out your soldering iron and assemble the board yourself. Some fab houses also offer to assemble the boards for you for an added fee.

## I have an assembled board, now what?

You'll need an FTDI cable like [this one](https://www.adafruit.com/product/70). Connect the USB end to your computer and the other end to the programming/monitoring header on ArduDesk board then press both buttons on the ESP8266 breakout module to put it into programming mode. Modify the config.json file in the 'data' folder to meet your needs. You may also need to modify the monitor_port setting in platformio.ini file to point to the correct serial port for your FTDI cable. If you don't already have [PlatformIO](https://platformio.org/) installed, you'll need to do that next (make sure if installed the VSCode extension, you also install the command line utils). Then you can run:

```shell
> pio run --environment huzzah --target uploadfs
```

and wait for it to complete. When it's done, press the reset button quickly followed by pressing both buttons to put it back in programming mode again. Then run:

```shell
> pio run --environment huzzah --target upload --target monitor
```

and wait for it to complete. Then press the reset button again and ArduDesk should start booting up. You should still be able to control your desk using the existing button pad. But now you can also control via [MQTT](https://mqtt.org/) or any software that allows integration via MQTT.

## The Nitty Gritty
Once every 5 seconds (by default), ArduDesk will poll the current state and height values and publish a status payload to the configured MQTT status topic. Likewise, you can send a control payload the configured MQTT control topic to raise or lower the height and stop the desk while it's actively moving. Now let's get deeper into details...

## Project Structure
This is a PlatformIO project, so please refer to the PlatformIO documentation for building.

```
|-- .github          # Continuous integration stuff
|-- data             # Configuration data. Anything here gets flashed when uploading a filesystem image
|-- include          # Contains C++ header files for the firmware
|-- openhab          # Contains OpenHAB integration files
|-- schematics       # Contains schematics (EasyEDA and PDF), PCB design (EasyEDA and PDF), BoM, and gerbers
|-- src              # Contains C++ sources for the firmware
|-- LICENSE          # MIT open source license
|-- platformio.ini   # PlatformIO project configuration file
|-- README.md        # This file
```

# Configuration

The config.h file contains default configuration directives. These options are the hardcoded default values that the system will initially boot with. However, if there is a config.json file present in the root of the SPIFFS file system, then its values will override the defaults. Here we'll discuss each option:

- DEBUG: Uncomment this define to generate additional debug output in the console.
- ENABLE_OTA: If you do not wish to support OTA updates, just comment this define.
- ENABLE_MDNS: If you do not wish to support [MDNS](https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html), comment this define.
- CONFIG_FILE_PATH: This is the path to the configuration file. The default is /config.json. The values in this file will override these defaults.
- DEFAULT_SSID: Change this line to reflect the SSID of your WiFi network.
- DEFAULT_PASSWORD: Change this line to reflect the password to your WiFi network.
- CLOCK_TIMEZONE: The GMT offset for your timezone (EST [America/New York] is -4 when observing DST. It's -5 when not).
- SERIAL_BAUD: While it is not recommended, you can change the BAUD rate of the serial port here.
- CHECK_WIFI_INTERVAL: The interval (in milliseconds) to check the water depth.
- CLOCK_SYNC_INTERVAL: The interval (in milliseconds) to try to resync the clock with NTP. Default is 1 hour.
- CHECK_DESK_HEIGHT_INTERVAL: The interval (in milliseconds) to get the desk height and state and publish if there has been a change since the last check.
- DEVICE_NAME: This essentially serves as the host name of the device on the network (default is ARDUDESK).
- CHECK_MQTT_INTERVAL: The interval (in milliseconds) to check connectivity to the MQTT broker. If the connection is lost, a reconnect will occur. The default value is 5 minutes.
- MQTT_TOPIC_STATUS: The MQTT status topic to publish device status messages to. Default is 'ardudesk/status'.
- MQTT_TOPIC_CONTROL: The MQTT control topic to subscribe to for control messages. Default is 'ardudesk/control'.
- MQTT_TOPIC_DISCOVERY: The MQTT discovery topic to publish discovery payloads to for systems that support device discovery via MQTT (specifically, [RedQueen](https://github.com/cyrusbuilt/RedQueen)).
- MQTT_BROKER: The hostname or IP of the MQTT broker.
- MQTT_PORT: The port on the MQTT broker to connect to. The default is 1883.
- OTA_HOST_PORT: Defines the port to listen for OTA updates on. This option is ignored if ENABLE_OTA is disabled.
- OTA_PASSWORD: The password used to authenticate with the OTA server. This option is ignored if ENABLE_OTA is disabled.
- defaultIp: The default IP address. By default, this device boots with a static IP configuration. The default IP is 192.168.0.229. You can change that here if you wish, but ideally you would just change it in config.json instead.
- defaultGw: The default gateway. The current default is 192.168.0.1. You can change that here if you wish.
- defaultSm: The subnet mask. By default, it is 255.255.255.0, but you can change that here if need be.
- defaultDns: The default primary DNS server. The current default is the same as the gateway (the typical configuration for residential routers) which is 192.168.0.1. You can change that here if you wish.

To override the default configuration options, you need to upload the 'config.json' file in the 'data' folder modified with your desired values. The file should look something like this:

```json
{
	"hostname": "ARDUDESK",
	"useDHCP": false,
	"ip": "192.168.0.229",
	"gateway": "192.168.0.1",
	"subnetMask": "255.255.255.0",
	"dnsServer": "192.168.0.1",
	"wifiSSID": "your_ssid_here",
	"wifiPassword": "your_wifi_password_here",
	"otaPort": 8266,
	"otaPassword": "your_ota_password_here",
	"mqttBroker": "your_mqtt_broker",
	"mqttPort": 1883,
	"mqttControlTopic": "ardudesk/control",
	"mqttStatusTopic": "ardudesk/status",
	"mqttUsername": "your_mqtt_username",
	"mqttPassword": "your_mqtt_password",
	"sittingHeight": 30,
	"standingHeight": 231
}
```

This configuration file is pretty self-explanatory. The file *MUST* be located in the 'data' directory located in the root of the project in order to be picked up by the flash uploader (either via Serial or OTA). Each of the options in the configuration file match up with the hard-coded default settings mentioned above. If this file is not present when the firmware boots, a new file will be created and populated with the hard-coded defaults. These defaults can then be changed via the fail-safe menu and saved. You'll notice some additonal options in the config.json file not present in config.h. They are as follows:

- mqttUsername: If you have enabled password authentication on your MQTT broker, provide the username here.
- mqttPassword: If you have enabled password authentication on your MQTT borker, provide the password here.
- sittingHeight: The desired height value for when you are sitting (0 - 256).
- standingHeight: The desired height value for when you are standing (0 - 256).

## OTA Updates
If you wish to be able to upload firmware updates Over-the-Air, then besides leaving the ENABLE_OTA option uncommented, you will also need to uncomment all the upload_* lines in platformio.ini, and change the line 'upload_port = ' to reflect the IP of the device and the line '--auth=' to reflect whatever 'otaPassword' in config.json is set to. Then when you click "upload" from the PlatformIO tasks (or if you execute 'pio run --target upload' from the command line) it should upload directly over WiFi and once completed, the device will automatically flash itself and reboot with the new version. If you wish to upload a new configuration file, you can also do this via OTA. Assuming the above-mentioned settings are configured, you can then click "Upload File System Image" from the PlatformIO project tasks (or execute 'pio run --target uploadfs' from the command line).

## Serial Console

If the device ever fails to connect to WiFi or if you press the 'i' key on your keyboard while in the serial console, normal operation of the device is suspended and the device will fall into a 'fail-safe' mode and present the user with a command menu, which can be used to reconfigure the device, reboot, or manually control the desk, etc.

## Integration

As previously mentioned, this was intended to be integrated with 3rd-party systems (ie. Home Automation). While my personal preference is [OpenHAB](https://www.openhab.org/), since ArduDesk communicates over MQTT, it can be integrated with any other system that can use MQTT as well (ie. [Home Assistant](https://www.home-assistant.io/), [Node-RED](https://nodered.org/), etc). While the integration files included in this repo are for OpenHAB, you should be able to fairly easily create your own configuration files to integrate with other systems. Do do so, you'll need the JSON message schema below:

ArduDesk control schema (refer to the ControlCommand enum in TelemetryHelper.h for valid command values):
```json
{
	"type": "object",
	"required": ["clientId", "command"],'
	"properties": {
		"clientId": {
			"type": "string"
		},
		"command": {
			"type": "integer",
			"minimum": 0,
			"maximum": 7
		},
		"requestedHeight": {
			"type": "integer",
			"minimum": 0,
			"maximum": 256
		}
	}
}
```

Example 1: Stand (Move to standing height preset)
```json
{
	"clientId": "ARDUDESK",
	"command": 7
}
```

Example 2: Move to specified height value (180 for this example)
```json
{
	"clientId": "ARDUDESK",
	"command": 4,
	"requestedHeight": 180
}
```

ArduDesk Status Schema (refer to SystemState enum in TelemetryHelper.h for valid system state values):
```json
{
	"type": "object",
	"required": [
		"clientId",
		"firmwareVersion",
		"systemState",
		"height",
		"state"
	],
	"properties": {
		"clientId": {
			"type": "string"
		},
		"firmwareVersion": {
			"type": "string"
		},
		"systemState": {
			"type": "integer",
			"minimum": 0,
			"maximum": 3
		},
		"height": {
			"type": "integer",
			"minimum": 0,
			"maximum": 256
		},
		"state": {
			"type": string
		},
		"sittingHeight": {
			"type": "integer",
			"minimum": 0,
			"maximum": 256
		},
		"standingHeight": {
			"type": "integer",
			"minimum": 0,
			"maximum": 256
		}
	}
}
```

Example (normal operation, desk idle at requested height):
```json
{
	"clientId": "ARDUDESK",
	"firmwareVersion": "1.1",
	"systemState": 1,
	"height": 180,
	"state": "STOPPED",
	"sittingHeight": 31,
	"standingHeight": 230
}
```

An ArduDesk status payload is emitted to the status topic periodically and every time there is a status change.

ArduDesk Discovery Schema:
```json
{
	"type": "object",
	"required": ["name", "class", "statusTopic", "controlTopic"],
	"properties": {
		"name": {
			"type": "string"
		},
		"class": {
			"type": "string"
		},
		"statusTopic": {
			"type": "string"
		},
		"controlTopic": {
			"type": "string"
		}
	}
}
```

Example:
```json
{
	"name": "ARDUDESK",
	"class": "ADUDESK",
	"statusTopic": "ardudesk/status",
	"controlTopic": "ardudesk/control"
}
```

An ArduDesk Discovery payload is emitted periodically whenever the MQTT connection check occurs.

## Interfacing With Uplift Desk
Based on everything I've read, examples I've seen, and my own experimentation, Uplift V2 desks appear to follow the same protocol over RS-232 Serial as [Jarvis](https://www.fully.com/standing-desks.html).