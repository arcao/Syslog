#include "Arduino.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "Syslog.h"

Syslog::Syslog() {
  this->_server = NULL;
  this->_port = 0;
  this->_deviceHostname = "-";
  this->_appName = "-";
  this->_defaultLevel = LOG_KERN;
}

Syslog::Syslog(const char* server, int port, const char* deviceHostname, const char* appName, int defaultLevel) {
  this->_server = server;
  this->_port = port;
  this->_deviceHostname = (deviceHostname == NULL) ? "-" : deviceHostname;
  this->_appName = (appName == NULL) ? "-" : appName;
  this->_defaultLevel = defaultLevel;
}

Syslog &Syslog::server(const char* server, int port) {
  this->_server = server;
  this->_port = port;
  return *this;
}

Syslog &Syslog::deviceHostname(const char* deviceHostname) {
  this->_deviceHostname = (deviceHostname == NULL) ? "-" : deviceHostname;
  return *this;
}

Syslog &Syslog::appName(const char* appName) {
  this->_appName = (appName == NULL) ? "-" : appName;
  return *this;
}
Syslog &Syslog::defaultLevel(int defaultLevel) {
  this->_defaultLevel = defaultLevel;
  return *this;
}

Syslog &Syslog::logV(int level, const char *fmt, va_list args) {
  char buffer[SYSLOG_PACKET_SIZE];
  char message[SYSLOG_MESSAGE_SIZE];

  time_t rawtime;
  struct tm* timeInfo;

  if (this->_server == NULL || this->_port == 0) {
    return *this;
  }

  time(&rawtime);
  timeInfo = localtime(&rawtime);

  vsnprintf(message, SYSLOG_MESSAGE_SIZE, fmt, args);

	if (level == 0) {
		level = this->_defaultLevel;
	} else if (level <= LOG_DEBUG) {
    level = LOG_MAKEPRI(LOG_FAC(this->_defaultLevel), level);
  }

  // Doc: https://tools.ietf.org/html/rfc5424
  int len = sprintf(buffer, "<%d>1 %04d-%02d-%02dT%02d:%02d:%02d %s %s - - - \xEF\xBB\xBF%s",
    level,
    timeInfo->tm_year + 1900,
    timeInfo->tm_mon + 1,
    timeInfo->tm_mday,
    timeInfo->tm_hour,
    timeInfo->tm_min,
    timeInfo->tm_sec,
    this->_deviceHostname,
    this->_appName,
    message);

  this->_udp.beginPacket(this->_server, this->_port);
  this->_udp.write(buffer, len);
  this->_udp.endPacket();

  return *this;
}

Syslog &Syslog::log(int level, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->logV(level, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::log(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->logV(this->_defaultLevel, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::log(int level, String fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->logV(level, fmt.c_str(), args);
  va_end(args);
  return *this;
}

Syslog &Syslog::log(String fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->logV(this->_defaultLevel, fmt.c_str(), args);
  va_end(args);
  return *this;
}

