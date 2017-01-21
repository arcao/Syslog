#include <stdio.h>
#include <stdarg.h>
#include <Arduino.h>

#include "Syslog.h"

// Public Methods //////////////////////////////////////////////////////////////

Syslog::Syslog(UDP &client) {
  this->_client = &client;
  this->_server = NULL;
  this->_port = 0;
  this->_deviceHostname = SYSLOG_EMPTY_VALUE;
  this->_appName = SYSLOG_EMPTY_VALUE;
  this->_defaultLevel = LOG_KERN;
}

Syslog::Syslog(UDP &client, const char* server, uint16_t port, const char* deviceHostname, const char* appName, int defaultLevel) {
  this->_client = &client;
  this->_server = server;
  this->_port = port;
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_EMPTY_VALUE : deviceHostname;
  this->_appName = (appName == NULL) ? SYSLOG_EMPTY_VALUE : appName;
  this->_defaultLevel = defaultLevel;
}

Syslog::Syslog(UDP &client, IPAddress ip, uint16_t port, const char* deviceHostname, const char* appName, int defaultLevel) {
  this->_client = &client;
  this->_ip = ip;
  this->_port = port;
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_EMPTY_VALUE : deviceHostname;
  this->_appName = (appName == NULL) ? SYSLOG_EMPTY_VALUE : appName;
  this->_defaultLevel = defaultLevel;
}

Syslog &Syslog::server(const char* server, uint16_t port) {
  this->_server = server;
  this->_port = port;
  return *this;
}

Syslog &Syslog::deviceHostname(const char* deviceHostname) {
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_EMPTY_VALUE : deviceHostname;
  return *this;
}

Syslog &Syslog::appName(const char* appName) {
  this->_appName = (appName == NULL) ? SYSLOG_EMPTY_VALUE : appName;
  return *this;
}

Syslog &Syslog::defaultLevel(int defaultLevel) {
  this->_defaultLevel = defaultLevel;
  return *this;
}

Syslog &Syslog::log(int level, const __FlashStringHelper *message) {
  return this->_sendLog(level, message);
}

Syslog &Syslog::log(int level, const String &message) {
  return this->_sendLog(level, message.c_str());
}

Syslog &Syslog::log(int level, const char *message) {
  return this->_sendLog(level, message);
}


Syslog &Syslog::vlogf(int level, const char *fmt, va_list args) {
  char message[SYSLOG_FMT_BUFFER_SIZE];

  vsnprintf(message, SYSLOG_FMT_BUFFER_SIZE, fmt, args);

  return this->_sendLog(level, message);
}

Syslog &Syslog::logf(int level, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->vlogf(level, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::logf(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->vlogf(this->_defaultLevel, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::log(const __FlashStringHelper *message) {
  return this->_sendLog(this->_defaultLevel, message);
}

Syslog &Syslog::log(const String &message) {
  return this->_sendLog(this->_defaultLevel, message.c_str());
}

Syslog &Syslog::log(const char *message) {
  return this->_sendLog(this->_defaultLevel, message);
}

// Private Methods /////////////////////////////////////////////////////////////

Syslog &Syslog::_sendLog(int level, const char *message) {
  int result;

  if (this->_server == NULL || this->_port == 0) {
    return *this;
  }

  if (level == 0) {
    level = this->_defaultLevel;
  } else if (level <= LOG_DEBUG) {
    level = LOG_MAKEPRI(LOG_FAC(this->_defaultLevel), level);
  }

  if (this->_server != NULL) {
    result = this->_client->beginPacket(this->_server, this->_port);
  } else {
    result = this->_client->beginPacket(this->_ip, this->_port);
  }

  if (result != 1) {
    return *this;
  }

  // Doc: https://tools.ietf.org/html/rfc5424
  this->_client->print('<');
  this->_client->print(level);
  this->_client->print(F(">1 - "));
  this->_client->print(this->_deviceHostname);
  this->_client->print(' ');
  this->_client->print(this->_appName);
  this->_client->print(F(" - - - \xEF\xBB\xBF"));
  this->_client->print(message);
  this->_client->endPacket();

  return *this;
}

Syslog &Syslog::_sendLog(int level, const __FlashStringHelper *message) {
  int result;

  if (this->_server == NULL || this->_port == 0) {
    return *this;
  }

  if (level == 0) {
    level = this->_defaultLevel;
  } else if (level <= LOG_DEBUG) {
    level = LOG_MAKEPRI(LOG_FAC(this->_defaultLevel), level);
  }

  if (this->_server != NULL) {
    result = this->_client->beginPacket(this->_server, this->_port);
  } else {
    result = this->_client->beginPacket(this->_ip, this->_port);
  }

  if (result != 1) {
    return *this;
  }

  // Doc: https://tools.ietf.org/html/rfc5424
  this->_client->print('<');
  this->_client->print(level);
  this->_client->print(F(">1 - "));
  this->_client->print(this->_deviceHostname);
  this->_client->print(' ');
  this->_client->print(this->_appName);
  this->_client->print(F(" - - - \xEF\xBB\xBF"));
  this->_client->print(message);
  this->_client->endPacket();

  return *this;
}
