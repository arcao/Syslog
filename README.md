# Syslog client
Arduino library for logging to Syslog server in [IETF format (rfc5424)](https://tools.ietf.org/html/rfc5424)

[![Build Status](https://travis-ci.org/arcao/ESP8266_Syslog.svg?branch=master)](https://travis-ci.org/arcao/ESP8266_Syslog)

How to use, see [examples](https://github.com/arcao/ESP8266_Syslog/tree/master/examples).

## Features
 - Supports syslog severity and facility constants
 - Supports `printf`-like formatting via `vsnprintf` method
 - Supports fluent interface
 - Independent on underlying network hardware. The network hardware library has
   to implement methods of `UDP` astract class only.     

## Compatible Hardware
The library uses the Arduino UDP api (`UDP` class) for interacting with 
the underlying network hardware. This means it Just Works with a growing number
of boards and shields, including:

 - ESP8266 / ESP32
 - Arduino Ethernet
 - Arduino Ethernet Shield
 - Arduino YUN – use the included `BridgeUDP` in place of `EthernetUDP`, and
   be sure to call a `Bridge.begin()` first
 - Arduino WiFi Shield
 - Intel Galileo/Edison
 - Arduino/Genuino MKR1000
 - [Arduino module RTL00(RTL8710AF), F11AMIM13 (RTL8711AM)](https://github.com/pvvx/RtlDuino)
 - ... you tell me!

## Limitations
 - This library is not sending time in syslog messages. Thus syslog server 
   use the time of receiving these messages instead. This is OK in most cases.
 - The `logf`/`vlogf` functions alocate formatting buffer for `80` chars. If you
   need more, please change `SYSLOG_FMT_BUFFER_SIZE` in `Syslog.h`
   