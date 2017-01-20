# ESP8266_Syslog
Arduino ESP8266 library for logging to Syslog server in [IETF format (rfc5424)](https://tools.ietf.org/html/rfc5424)

[![Build Status](https://travis-ci.org/arcao/ESP8266_Syslog.svg?branch=master)](https://travis-ci.org/arcao/ESP8266_Syslog)

How to use, see [examples](https://github.com/arcao/ESP8266_Syslog/tree/master/examples).

## Sending time in Syslog packets
This library is not sending time in Syslog messages by default. So Syslog server 
use the time of receiving logging message instead. This is OK in most cases. 
Anyway you can enable sending time in `src/Syslog.h` file with uncommenting 
below line:
```c
// #define SYSLOG_USE_TIME_H
```

After that, the library will use methods from `time.h`. So, be sure you have 
synced time with NTP using build-in NTP client in **ESP8266**. Just call 
`configTime` method. See example:
```c
#include <Arduino.h> // required for configTime method

// Timezone for NTP
#define TIME_ZONE         +1 // For +1 hour
#define TIME_ZONE_DST     0  // Daylight Saving Time: 0 = No DST, 1 = DST
#define TIME_ZONE_SECONDS (TIME_ZONE + TIME_ZONE_DST) * 3600

void setup() {
  // Connect to WIFI network
  ...

  // Setup build-in NTP client to receive time from NTP    
  configTime(TIME_ZONE_SECONDS, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
  ...
}
```

If you want to use time from NTP in your sketch, just use methods from `time.h`.
The `time.h` header file is placed in [`tools/sdk/libc/xtensa-lx106-elf/include/time.h`](https://github.com/esp8266/Arduino/blob/master/tools/sdk/libc/xtensa-lx106-elf/include/time.h)
in **ESP8266** hardware directory.

For more, [search tutorial *how to use `time.h`*](https://www.google.com/search?q=time.h%20tutorial)
at Google.

#### This works with the ESP8266 Arduino platform with a recent stable release(2.0.0 or newer) https://github.com/esp8266/Arduino
