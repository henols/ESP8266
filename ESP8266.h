/**************************************************************************/
/*! 
  @file     ESP8266.h
  @author   KTOWN (Kevin Townsend for Adafruit Industries)
  @license  BSD (see license.txt)

  This is a library for the Adafruit CC3000 WiFi breakout board
  This library works with the Adafruit CC3000 breakout
  ----> https://www.adafruit.com/products/1469

  Check out the links above for our tutorials and wiring diagrams
  These chips use SPI to communicate.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
*/
/**************************************************************************/

#ifndef __ESP8266_H__
#define __ESP8266_H__

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Client.h"


#define WLAN_CONNECT_TIMEOUT 10000  // how long to wait, in milliseconds
#define RXBUFFERSIZE  64 // how much to buffer on the incoming side
#define TXBUFFERSIZE  32 // how much to buffer on the outgoing side

#define WIFI_ENABLE 1
#define WIFI_DISABLE 0
#define WIFI_STATUS_CONNECTED 1

#define      WLAN_SEC_UNSEC (0)
#define      WLAN_SEC_WEP	(1)
#define      WLAN_SEC_WPA	(2)
#define      WLAN_SEC_WPA2	(3)


/* Enum for wlan_ioctl_statusget results */
typedef enum
{
  STATUS_DISCONNECTED = 0,
  STATUS_SCANNING     = 1,
  STATUS_CONNECTING   = 2,
  STATUS_CONNECTED    = 3
} status_t;

class ESP8266;

class ESP8266_Client : public Client {
 public:
	ESP8266_Client(ESP8266* esp8266, int16_t id);
	ESP8266_Client(void);
  void operator=(const ESP8266_Client& other);

  int connect(IPAddress ip, uint16_t port);
  int connect(const char *host, uint16_t port);

  uint8_t connected(void);
  size_t write(uint8_t c);


  size_t write(const void *buf, uint16_t len);
  int read(void *buf, uint16_t len);
  int read(void);
  int32_t close(void);
  int available(void);

  int read(uint8_t *buf, size_t size);
  size_t write(const uint8_t *buf, size_t size);
  int peek();
  void flush();
  void stop();
  operator bool();

  uint8_t _rx_buf[RXBUFFERSIZE], _rx_buf_idx;
  int16_t bufsiz;

 private:
  int16_t id;
  ESP8266* esp8266;
};

class ESP8266 {
  public:
    ESP8266();
    bool     begin(Stream *serial, Stream *dbgSerial);
    bool     begin(Stream *serial);
    void     reboot(uint8_t patchReq = 0);
    void     stop(void);
    bool     disconnect(void);

    bool     getMacAddress(uint8_t address[6]);

    bool     setStaticIPAddress(uint32_t ip, uint32_t subnetMask, uint32_t defaultGateway, uint32_t dnsServer);
    bool     setDHCP();

    bool     connectToAP(const char *ssid, const char *key, uint8_t secmode, uint8_t attempts = 0);
    bool     connectSecure(const char *ssid, const char *key, int32_t secMode);
    bool     connectOpen(const char *ssid);
    bool     checkConnected(void);

    bool     getIPAddress(uint32_t *retip, uint32_t *netmask, uint32_t *gateway, uint32_t *dhcpserv, uint32_t *dnsserv);


    ESP8266_Client connectTCP(uint32_t destIP, uint16_t destPort);
    ESP8266_Client connectUDP(uint32_t destIP, uint16_t destPort);

    bool     getFirmwareVersion(uint8_t *major, uint8_t *minor);
    status_t getStatus(void);

    void debugPrintln(String str);
    void debugPrint(String str);

  private:
    Stream *serial;
    Stream *dbgSerial;
    bool _initialised;
    bool debug;

};


#endif
