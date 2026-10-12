# Syslog

An Arduino library for logging to Syslog server via `UDP` protocol in 
[IETF (RFC 5424)] and [BSD (RFC 3164)] message format

[![Compile examples](https://github.com/arcao/Syslog/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/arcao/Syslog/actions/workflows/compile-examples.yml) [![Join the chat at https://gitter.im/arcao/Syslog](https://badges.gitter.im/arcao/Syslog.svg)](https://gitter.im/arcao/Syslog?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

How to use, see [examples].

## Features
 - Supports original Syslog severity level and facility constants
 - Supports both Syslog messge formats: [IETF (RFC 5424)] and [BSD (RFC 3164)]
 - Supports `printf`-like formatting via `logf` methods (use `vsnprintf` method
   inside)
 - Supports fluent interface, see [AdvancedLogging] example
 - Allows to ignore sending specified severity levels with `logMask` function, 
   see [AdvancedLogging] example
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
 - Arduino boards with WiFiNINA module (Nano 33 IoT, MKR WiFi 1010, UNO WiFi Rev2)
 - [Arduino module RTL00(RTL8710AF), F11AMIM13 (RTL8711AM)][RTLDUINO]
 - ... you tell me!

## Usage notes
 - **Open the UDP socket first.** Some network libraries (e.g. Ethernet, WiFi
   Shield, WiFiNINA) do not send anything until the UDP socket is opened. Call
   `udpClient.begin(localPort)` with any free local port after the network is
   connected and before the first `log`/`logf` call. It is harmless on other
   platforms, so it is a good habit to always do it:

   ```c
   udpClient.begin(8888);
   ```

 - **Strings are not copied.** The constructor and the `server`,
   `deviceHostname` and `appName` methods store only pointers to the passed
   strings. The strings must stay valid and unchanged for the whole time the
   `Syslog` instance is used, so use string literals or global/static buffers,
   not local variables:

   ```c
   char hostname[64]; // global buffer, NOT a local variable in setup()

   void setup() {
     readHostnameFromConfig(hostname);
     syslog.deviceHostname(hostname);
   }
   ```

## Syslog message formats
This library supports both Syslog message formats [IETF (RFC 5424)] and 
[BSD (RFC 3164)]. The newer **IETF** format is used by default. If you want to use
older "obsolete" **BSD** format, just specify it with `SYSLOG_PROTO_BSD` constant 
in a last constructor parameter.

```c
Syslog syslog(udpClient, host, port, device_hostname, app_name, default_priority, SYSLOG_PROTO_BSD);
// or
Syslog syslog(udpClient, ip, port, device_hostname, app_name, default_priority, SYSLOG_PROTO_BSD);
// or
Syslog syslog(udpClient, SYSLOG_PROTO_BSD);
```

## Limitations
 - This library is sending empty timestamp in the syslog messages. For IETF 
   format it is `NILVALUE` (char `-`) in `TIMESTAMP` field, for BSD format the 
   `TIMESTAMP` field is completely ommited. Syslog server should use a time
   of receiving message in this case. It is OK in most cases. This issue will be
   fixed in some of the next releases.
   

[IETF (RFC 5424)]: https://tools.ietf.org/html/rfc5424
[BSD (RFC 3164)]: https://tools.ietf.org/html/rfc3164
[examples]: https://github.com/arcao/Syslog/tree/master/examples
[AdvancedLogging]: https://github.com/arcao/Syslog/blob/master/examples/AdvancedLogging/AdvancedLogging.ino
[RTLDUINO]: https://github.com/pvvx/RtlDuino
