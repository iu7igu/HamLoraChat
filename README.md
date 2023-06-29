# HamLoraChat

The HamLoraChat project use the power of LoRa protocol and bluetooth module to exchange messages between two or more radio amateurs.

![photo_5832603551640174400_y](https://github.com/iu7igu/HamLoraChat/assets/39775345/582d3b5a-77dc-4620-ad63-e17684a3b4f3)


## Standard mode.
In this mode, our device will exchange messages with other radio amateurs through repeaters/gateways (a device can be configured as both a gateway and a repeater)
Which through the MQTT protocol communicate with other gateways. With this mode, the Internet is used to convey messages from one zone to another.
Messages will also be displayed on the telegram channel [HamLoraChat Italia](https://t.me/+WrLhWyv1iXFjMGU0)

![Immagine 2023-06-25 184729](https://github.com/iu7igu/HamLoraChat/assets/39775345/a9f9f851-0a49-42b6-a925-73d6926b045d)

## Direct mode
In this mode each device will exchange messages with other radio amateurs without taking advantage of the repeater/gateway network, so each device communicates directly with other devices even if they are not in the same mode.

![Immagine 2023-06-29 161448](https://github.com/iu7igu/HamLoraChat/assets/39775345/868c9625-4a95-4940-a852-cbaf6529b098)


# Compiling and configuration

### How to compile

This project use PlatformIO.
* Go to PlatformIO download and install the IDE.
* If installed open the IDE, go to the left side and click on 'extensions' then search for 'PlatformIO' and install.
* When installed click 'the ant head' on the left and choose import the project on the right.
* Just open the folder and you can compile the Firmware.

### Configuration

To be continue---
