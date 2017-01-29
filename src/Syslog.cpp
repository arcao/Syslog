#include <stdio.h>
#include <stdarg.h>
#include <Arduino.h>

#include "Syslog.h"

// Public Methods //////////////////////////////////////////////////////////////

Syslog::Syslog(UDP &client, uint8_t protocol) {
  this->_client = &client;
  this->_protocol = protocol;
  this->_server = NULL;
  this->_port = 0;
  this->_deviceHostname = SYSLOG_NILVALUE;
  this->_appName = SYSLOG_NILVALUE;
  this->_priDefault = LOG_KERN;
}

Syslog::Syslog(UDP &client, const char* server, uint16_t port, const char* deviceHostname, const char* appName, uint16_t priDefault, uint8_t protocol) {
  this->_client = &client;
  this->_protocol = protocol;
  this->_server = server;
  this->_port = port;
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_NILVALUE : deviceHostname;
  this->_appName = (appName == NULL) ? SYSLOG_NILVALUE : appName;
  this->_priDefault = priDefault;
}

Syslog::Syslog(UDP &client, IPAddress ip, uint16_t port, const char* deviceHostname, const char* appName, uint16_t priDefault, uint8_t protocol) {
  this->_client = &client;
  this->_protocol = protocol;
  this->_ip = ip;
  this->_port = port;
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_NILVALUE : deviceHostname;
  this->_appName = (appName == NULL) ? SYSLOG_NILVALUE : appName;
  this->_priDefault = priDefault;
}

Syslog &Syslog::server(const char* server, uint16_t port) {
  this->_server = server;
  this->_port = port;
  return *this;
}

Syslog &Syslog::server(IPAddress ip, uint16_t port) {
  this->_ip = ip;
  this->_port = port;
  return *this;
}


Syslog &Syslog::deviceHostname(const char* deviceHostname) {
  this->_deviceHostname = (deviceHostname == NULL) ? SYSLOG_NILVALUE : deviceHostname;
  return *this;
}

Syslog &Syslog::appName(const char* appName) {
  this->_appName = (appName == NULL) ? SYSLOG_NILVALUE : appName;
  return *this;
}

Syslog &Syslog::defaultPriority(uint16_t pri) {
  this->_priDefault = pri;
  return *this;
}

Syslog &Syslog::logMask(uint8_t priMask) {
  this->_priMask = priMask;
  return *this;
}


Syslog &Syslog::log(uint16_t pri, const __FlashStringHelper *message) {
  return this->_sendLog(pri, message);
}

Syslog &Syslog::log(uint16_t pri, const String &message) {
  return this->_sendLog(pri, message.c_str());
}

Syslog &Syslog::log(uint16_t pri, const char *message) {
  return this->_sendLog(pri, message);
}


Syslog &Syslog::vlogf(uint16_t pri, const char *fmt, va_list args) {
  char message[SYSLOG_FMT_BUFFER_SIZE];

  vsnprintf(message, SYSLOG_FMT_BUFFER_SIZE, fmt, args);

  return this->_sendLog(pri, message);
}

Syslog &Syslog::logf(uint16_t pri, const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->vlogf(pri, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::logf(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  this->vlogf(this->_priDefault, fmt, args);
  va_end(args);
  return *this;
}

Syslog &Syslog::vlogf_P(uint16_t pri, PGM_P formatP, ...) {
  va_list arg;
  va_start(arg, formatP);
  char* temp = new char[64];
  if(!temp){
    return *this;
  }
  char* buffer = temp;
  size_t len = vsnprintf_P(temp, 64, formatP, arg);
  va_end(arg);
  if (len > 63) {
    buffer = new char[len + 1];
    if (!buffer) {
      return *this;
    }
    va_start(arg, formatP);
    vsnprintf_P(buffer, len + 1, formatP, arg);
    va_end(arg);
  }
  this->_sendLog(pri, buffer);
  if (buffer != temp) {
    delete[] buffer;
  }
  delete[] temp;
  return *this;
}



Syslog &Syslog::log(const __FlashStringHelper *message) {
  return this->_sendLog(this->_priDefault, message);
}

Syslog &Syslog::log(const String &message) {
  return this->_sendLog(this->_priDefault, message.c_str());
}

Syslog &Syslog::log(const char *message) {
  return this->_sendLog(this->_priDefault, message);
}

// Private Methods /////////////////////////////////////////////////////////////

Syslog &Syslog::_sendLog(uint16_t pri, const char *message) {
  int result;

  if ( (this->_server == NULL && this->_ip == INADDR_NONE) || this->_port == 0)
    return *this;

  // Check priority against priMask values.
  if ((LOG_MASK(LOG_PRI(pri)) & this->_priMask) == 0)
    return *this;

  // Set default facility if none specified.
  if ((pri & LOG_FACMASK) == 0)
    pri = LOG_MAKEPRI(LOG_FAC(this->_priDefault), pri);

  if (this->_server != NULL) {
    result = this->_client->beginPacket(this->_server, this->_port);
  } else {
    result = this->_client->beginPacket(this->_ip, this->_port);
  }

  if (result != 1)
    return *this;

  // IETF Doc: https://tools.ietf.org/html/rfc5424
  // BSD Doc: https://tools.ietf.org/html/rfc3164
  this->_client->print('<');
  this->_client->print(pri);
  if (this->_protocol == SYSLOG_PROTO_IETF) {
    this->_client->print(F(">1 - "));
  } else {
    this->_client->print(F(">"));
  }
  this->_client->print(this->_deviceHostname);
  this->_client->print(' ');
  this->_client->print(this->_appName);
  if (this->_protocol == SYSLOG_PROTO_IETF) {
    this->_client->print(F(" - - - \xEF\xBB\xBF"));
  } else {
    this->_client->print(F("[0]: "));
  }
  this->_client->print(message);
  this->_client->endPacket();

  return *this;
}

Syslog &Syslog::_sendLog(uint16_t pri, const __FlashStringHelper *message) {
  int result;

  if (this->_server == NULL || this->_port == 0)
    return *this;

  // Check priority against priMask values.
  if ((LOG_MASK(LOG_PRI(pri)) & this->_priMask) == 0)
    return *this;

  // Set default facility if none specified.
  if ((pri & LOG_FACMASK) == 0)
    pri = LOG_MAKEPRI(LOG_FAC(this->_priDefault), pri);

  if (this->_server != NULL) {
    result = this->_client->beginPacket(this->_server, this->_port);
  } else {
    result = this->_client->beginPacket(this->_ip, this->_port);
  }

  if (result != 1)
    return *this;

  // IETF Doc: https://tools.ietf.org/html/rfc5424
  // BSD Doc: https://tools.ietf.org/html/rfc3164
  this->_client->print('<');
  this->_client->print(pri);
  if (this->_protocol == SYSLOG_PROTO_IETF) {
    this->_client->print(F(">1 - "));
  } else {
    this->_client->print(F(">"));
  }
  this->_client->print(this->_deviceHostname);
  this->_client->print(' ');
  this->_client->print(this->_appName);
  if (this->_protocol == SYSLOG_PROTO_IETF) {
    this->_client->print(F(" - - - \xEF\xBB\xBF"));
  } else {
    this->_client->print(F("[0]: "));
  }
  this->_client->print(message);
  this->_client->endPacket();


  return *this;
}
