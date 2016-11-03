#include "Arduino.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "Syslog.h"

Syslog::Syslog(const char* server, int port, const char* localHostname, const char* appName, int defaultLevel) {
  this->_server = server;
  this->_port = port;
  this->_localHostname = localHostname;
  this->_appName = (appName == NULL) ? "-" : appName;
  this->_defaultLevel = defaultLevel;
}

void Syslog::log(int level, const char *fmt, ...) {
  char buffer[SYSLOG_PACKET_SIZE];
  char message[SYSLOG_MESSAGE_SIZE];
  va_list args;

  time_t rawtime;
  struct tm* timeInfo;

  time(&rawtime);
  timeInfo = localtime(&rawtime);

  va_start(args, fmt);
  vsnprintf(message, SYSLOG_MESSAGE_SIZE, fmt, args);
  va_end(args);

  if (level <= LOG_DEBUG) {
    level = level | this->_defaultLevel;
  }

  // Doc: https://tools.ietf.org/html/rfc5424
  int len = sprintf(buffer, "<%d>1 %04d-%02d-%02dT%02d:%02d:%02d %s %s - - - %s",
    level,
    timeInfo->tm_year + 1900,
    timeInfo->tm_mon + 1,
    timeInfo->tm_mday,
    timeInfo->tm_hour,
    timeInfo->tm_min,
    timeInfo->tm_sec,
    this->_localHostname,
    this->_appName,
    message);

  this->_udp.beginPacket(this->_server, this->_port);
  this->_udp.write(buffer, len);
  this->_udp.endPacket();     
}

