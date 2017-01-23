# Syslog client
An Arduino library for logging to Syslog server in [IETF format (rfc5424)](https://tools.ietf.org/html/rfc5424)

[![Build Status](https://travis-ci.org/arcao/ESP8266_Syslog.svg?branch=master)](https://travis-ci.org/arcao/ESP8266_Syslog)

How to use, see [examples](https://github.com/arcao/ESP8266_Syslog/tree/master/examples).

## Features
 - Supports original syslog severity level and facility constants
 - Supports `printf`-like formatting via `logf` methods (use `vsnprintf` method
   inside)
 - Supports fluent interface, see [AdvancedLogging]https://github.com/arcao/ESP8266_Syslog/blob/master/examples/AdvancedLogging/AdvancedLogging.ino)
   example
 - Allows to ignore sending specified severity levels with `logMask` function, see [AdvancedLogging]https://github.com/arcao/ESP8266_Syslog/blob/master/examples/AdvancedLogging/AdvancedLogging.ino)
   example
 - Independent on underlying network hardware. The network hardware library has
   to implement methods of `UDP` astract class only.

## Compatible Hardware
The library uses the Arduino UDP Network API (`UDP` class) for interacting with 
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
 - This library is sending empty time (`NILVALUE`) in `TIMESTAMP` field in the
   syslog messages. Thus syslog server use a time of receiving these messages 
   instead. This is OK in most cases.
 - The `logf`/`vlogf` functions alocate formatting buffer for `80` chars. If you
   need more, please change `SYSLOG_FMT_BUFFER_SIZE` in `Syslog.h`
