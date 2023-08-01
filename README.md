# HamLoraChat

The HamLoraChat project use the power of LoRa protocol and bluetooth module to exchange messages between two or more radio amateurs, this version contain an basic aprs tracker.

<img src="https://github.com/iu7igu/HamLoraChat/assets/39775345/582d3b5a-77dc-4620-ad63-e17684a3b4f3" width="500" height="500">

## Standard mode.
In this mode, our device will exchange messages with other radio amateurs through repeaters/gateways (a device can be configured as both a gateway and a repeater)
Which through the MQTT protocol communicate with other gateways. With this mode, the Internet is used to convey messages from one zone to another.
Messages will also be displayed on the telegram channel [HamLoraChat Italia](https://t.me/+WrLhWyv1iXFjMGU0)

<img src="https://github.com/iu7igu/HamLoraChat/assets/39775345/a9f9f851-0a49-42b6-a925-73d6926b045d" width="800" height="500">

## Direct mode
In this mode each device will exchange messages with other radio amateurs without taking advantage of the repeater/gateway network, so each device communicates directly with other devices even if they are not in the same mode.

<img src="https://github.com/iu7igu/HamLoraChat/assets/39775345/868c9625-4a95-4940-a852-cbaf6529b098" width="800" height="500">


# Compiling and configuration

### How to compile

This project use PlatformIO.
* Go to PlatformIO download and install the IDE.
* If installed open the IDE, go to the left side and click on 'extensions' then search for 'PlatformIO' and install.
* When installed click 'the ant head' on the left and choose import the project on the right.
* Just open the folder and you can compile the Firmware.

### Configuration
To configure the node, modify these parameters in main.cpp

```
String qrz = "IU7IGU-10";   
const char* beacon_message = "HamLoraChat info: www.github.com/iu7igu/HamLoraChat";
const bool beacon = false;                //Enable or disable beacon aprs
const int beacon_time = 5;                // Beacon delay in minute   
const bool gps = true;                    //Enable or disable gps
const long lora_freq = 433775000;         //Frequency
const int lora_power = 20;                //Lora output power level
const int lora_spread = 12;               //Lora spreading factor
const float lora_bandw = 125000;          //Lora bandwith
const bool oled = true;                   //Enable or disable OLED
const char* locatore = "JN70BN";          //Maidenhead Locator
const bool diretta = false;               //Enable or disable direct mode
```

### Upload firmware

After the configuration change, the Flash must be deleted through Erase Flash command in Platformio Section of Visual Studio Code.

Afterwards, the firmware can be loaded via Upload command in Platformio Section.


## Use HamLoraChat
For use HamLoraChat system you must need a smartphone with bluetooth and an app like Serial Bluetooth Terminal.
Download link for [Android](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=it&gl=US&pli=1) or for [Apple](https://apps.apple.com/it/app/bluetooth-terminal/id1058693037) 

**WARNING! I could not try iPhone App**

Once paired device (you should see HamLoraChat on list of bluetooth devices) you can connect them with Serial Bluetooth Terminal like this:

<img src="https://github.com/iu7igu/HamLoraChat/assets/39775345/15893e5a-da43-4d75-a0bf-19c75769e0a6" widht="500" height="500">

When you are in this mode the node are working yet. If you write a message and send it, it will be sent via Lora protocol to GW/RPT or Node.

# Menu command
From Serial Bluetooth Terminal is possible to modify some parameter throught command listed below

```
/menu              ->   Print list of command
/stato             ->   Print to terminal a list of information about HamLoraNode
/gps on/off        ->   Enable or disable gps
/beacon on/off     ->   Enable or disable beacon
/beacontime 5      ->   Modify beacon delay
/freq 433775000    ->   Modify frequency of node
/locatore JK85ND   ->   Modify locator
/direct on/off     ->   Enable or disable Direct Mode
```

