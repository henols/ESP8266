/**************************************************************************/
/*!
 @file     ESP8266.cpp
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

 @section  HISTORY

 v1.0    - Initial release
 */
/**************************************************************************/
#include "ESP8266.h"

/***********************/

#define CC3000_SUCCESS                        (0)
#define CHECK_SUCCESS(func,Notify,errorCode)  {if ((func) != CC3000_SUCCESS) { CHECK_PRINTER CC3KPrinter->println(F(Notify)); return errorCode;}}

#define MAXSSID           (32)
#define MAXLENGTHKEY      (32)  /* Cleared for 32 bytes by TI engineering 29/08/13 */

#define MAXidS 32  // can change this
boolean closedids[MAXidS] = { false, false, false, false };

/* *********************************************************************** */
/*                                                                         */
/* PRIVATE FIELDS (SmartConfig)                                            */
/*                                                                         */
/* *********************************************************************** */

class ESP8266BitSet {
public:
	static const byte IsSmartConfigFinished = 0x01;
	static const byte IsConnected = 0x02;
	static const byte HasDHCP = 0x04;
	static const byte OkToShutDown = 0x08;

	void clear() {
		flags = 0;
	}

	bool test(const byte flag) {
		return (flags & flag) != 0;
	}

	void set(const byte flag) {
		flags |= flag;
	}

	void reset(const byte flag) {
		flags &= ~flag;
	}
private:
	volatile byte flags;
} esp8266Bitset;

/* *********************************************************************** */
/*                                                                         */
/* PRIVATE FUNCTIONS                                                       */
/*                                                                         */
/* *********************************************************************** */

/* *********************************************************************** */
/*                                                                         */
/* CONSTRUCTORS                                                            */
/*                                                                         */
/* *********************************************************************** */

ESP8266::ESP8266() {
	_initialised = false;
}

/* *********************************************************************** */
/*                                                                         */
/* PUBLIC FUNCTIONS                                                        */
/*                                                                         */
/* *********************************************************************** */

bool ESP8266::begin(Stream *serial, Stream *dbgSerial) {
	if (_initialised)
		return true;

	this->serial = serial;
	this->dbgSerial = dbgSerial;
	this->debug = true;

	_initialised = true;

	return true;
}

bool ESP8266::begin(Stream *serial) {
	if (_initialised)
		return true;

	this->serial = serial;
	this->debug = false;

	_initialised = true;

	return true;
}

/**************************************************************************/
/*!
 @brief   Reboot CC3000 (stop then start)
 */
/**************************************************************************/
void ESP8266::reboot(uint8_t patch) {
	if (!_initialised) {
		return;
	}

//  wlan_stop();
//  delay(5000);
//  wlan_start(patch);
}

/**************************************************************************/
/*!
 @brief   Stop CC3000
 */
/**************************************************************************/
void ESP8266::stop(void) {
	if (!_initialised) {
		return;
	}

//  wlan_stop();
}

/**************************************************************************/
/*!
 @brief  Disconnects from the network

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::disconnect(void) {
	if (!_initialised) {
		return false;
	}

//  long retVal = wlan_disconnect();

//  return retVal != 0 ? false : true;
	return 0;
}

/**************************************************************************/
/*!
 @brief    Reads the MAC address

 @param    address  Buffer to hold the 6 byte Mac Address

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::getMacAddress(uint8_t address[6]) {
	if (!_initialised) {
		return false;
	}

//  CHECK_SUCCESS(nvmem_read(NVMEM_MAC_FILEID, 6, 0, address),
//                "Failed reading MAC address!", false);

	return true;
}

/**************************************************************************/
/*!
 @brief   Reads the current IP address

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::getIPAddress(uint32_t *retip, uint32_t *netmask, uint32_t *gateway, uint32_t *dhcpserv, uint32_t *dnsserv) {
	if (!_initialised)
		return false;
	if (!esp8266Bitset.test(ESP8266BitSet::IsConnected))
		return false;
	if (!esp8266Bitset.test(ESP8266BitSet::HasDHCP))
		return false;

//  tNetappIpconfigRetArgs ipconfig;
//  netapp_ipconfig(&ipconfig);
//
//  /* If byte 1 is 0 we don't have a valid address */
//  if (ipconfig.aucIP[3] == 0) return false;
//
//  memcpy(retip, ipconfig.aucIP, 4);
//  memcpy(netmask, ipconfig.aucSubnetMask, 4);
//  memcpy(gateway, ipconfig.aucDefaultGateway, 4);
//  memcpy(dhcpserv, ipconfig.aucDHCPServer, 4);
//  memcpy(dnsserv, ipconfig.aucDNSServer, 4);

	return true;
}

/**************************************************************************/
/*!
 @brief    Gets the two byte ID for the firmware patch version

 @note     This command isn't available when the CC3000 is configured
 in 'CC3000_TINY_DRIVER' mode

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::getFirmwareVersion(uint8_t *major, uint8_t *minor) {
	uint8_t fwpReturn[2];

	if (!_initialised) {
		return false;
	}

//  CHECK_SUCCESS(nvmem_read_sp_version(fwpReturn),
//                "Unable to read the firmware version", false);

	*major = fwpReturn[0];
	*minor = fwpReturn[1];

	return true;
}

/**************************************************************************/
/*!
 @Brief   Prints out the current status flag of the CC3000

 @note     This command isn't available when the CC3000 is configured
 in 'CC3000_TINY_DRIVER' mode
 */
/**************************************************************************/
status_t ESP8266::getStatus() {
	if (!_initialised) {
		return STATUS_DISCONNECTED;
	}

	long results = 0; //wlan_ioctl_statusget();

	switch (results) {
	case 1:
		return STATUS_SCANNING;
		break;
	case 2:
		return STATUS_CONNECTING;
		break;
	case 3:
		return STATUS_CONNECTED;
		break;
	case 0:
	default:
		return STATUS_DISCONNECTED;
		break;
	}
}

/**************************************************************************/
/*!
 @brief    Calls listSSIDs and then displays the results of the SSID scan

 For the moment we only list these via CC3KPrinter->print since
 this can consume a lot of memory passing all the data
 back with a buffer

 @note     This command isn't available when the CC3000 is configured
 in 'CC3000_TINY_DRIVER' mode

 @returns  False if an error occured!
 */
/**************************************************************************/

/**************************************************************************/
/*!
 Connect to an unsecured SSID/AP(security)

 @param  ssid      The named of the AP to connect to (max 32 chars)
 @param  ssidLen   The size of the ssid name

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::connectOpen(const char *ssid) {
	if (!_initialised) {
		return false;
	}

//  #ifndef CC3000_TINY_DRIVER
//    CHECK_SUCCESS(wlan_ioctl_set_connection_policy(0, 0, 0),
//                 "Failed to set connection policy", false);
//    delay(500);
//    CHECK_SUCCESS(wlan_connect(WLAN_SEC_UNSEC,
//          (const char*)ssid, strlen(ssid),
//          0 ,NULL,0),
//          "SSID connection failed", false);
//  #else
//    wlan_connect(ssid, ssidLen);
//  #endif

	return true;
}

/**************************************************************************/
/*!
 Connect to an SSID/AP(security)

 @note     This command isn't available when the CC3000 is configured
 in 'CC3000_TINY_DRIVER' mode

 @returns  False if an error occured!
 */
/**************************************************************************/
bool ESP8266::connectSecure(const char *ssid, const char *key, int32_t secMode) {
	if (!_initialised) {
		return false;
	}

//  if ( (secMode < 0) || (secMode > 3)) {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Security mode must be between 0 and 3"));
//    }
//    return false;
//  }
//
//  if (strlen(ssid) > MAXSSID) {
//    CHECK_PRINTER {
//      CC3KPrinter->print(F("SSID length must be < "));
//      CC3KPrinter->println(MAXSSID);
//    }
//    return false;
//  }
//
//  if (strlen(key) > MAXLENGTHKEY) {
//    CHECK_PRINTER {
//      CC3KPrinter->print(F("Key length must be < "));
//      CC3KPrinter->println(MAXLENGTHKEY);
//    }
//    return false;
//  }
//
//  CHECK_SUCCESS(wlan_ioctl_set_connection_policy(0, 0, 0),
//                "Failed setting the connection policy",
//                false);
//  delay(500);
//  CHECK_SUCCESS(wlan_connect(secMode, (char *)ssid, strlen(ssid),
//                             NULL,
//                             (unsigned char *)key, strlen(key)),
//                "SSID connection failed", false);
//
//  /* Wait for 'HCI_EVNT_WLAN_UNSOL_CONNECT' in CC3000_UsynchCallback */

	return true;
}

// Connect with timeout
bool ESP8266::connectToAP(const char *ssid, const char *key, uint8_t secmode, uint8_t attempts) {
	if (!_initialised) {
		return false;
	}
	Serial.println("AT+CWMODE=1");
	String cmd = "AT+CWJAP=\"";
	cmd += String(ssid);
	cmd += "\",\"";
	cmd += String(key);
	cmd += "\"";
	debugPrintln(cmd);
	Serial.println(cmd);
	delay(2000);
	if (Serial.find("OK")) {
		debugPrintln("OK, Connected to WiFi.");
		return true;
	} else {
		debugPrintln("Can not connect to the WiFi.");
		return false;
	}
//  int16_t timer;
//
//  // If attempts is zero interpret that as no limit on number of retries.
//  bool retryForever = attempts == 0;
//
//  do {
//    // Stop if the max number of attempts have been tried.
//    if (!retryForever) {
//      if (attempts == 0) {
//        return checkConnected();
//      }
//      attempts -= 1;
//    }
//
//    cc3k_int_poll();
//    /* MEME: not sure why this is absolutely required but the cc3k freaks
//       if you dont. maybe bootup delay? */
//    // Setup a 4 second SSID scan
//    scanSSIDs(4000);
//    // Wait for results
//    delay(4500);
//    scanSSIDs(0);
//
//    /* Attempt to connect to an access point */
//    CHECK_PRINTER {
//      CC3KPrinter->print(F("\n\rConnecting to "));
//      CC3KPrinter->print(ssid);
//      CC3KPrinter->print(F("..."));
//    }
//    if ((secmode == 0) || (strlen(key) == 0)) {
//      /* Connect to an unsecured network */
//      if (! connectOpen(ssid)) {
//        CHECK_PRINTER {
//          CC3KPrinter->println(F("Failed!"));
//        }
//        continue;
//      }
//    } else {
//      /* NOTE: Secure connections are not available in 'Tiny' mode! */
//#ifndef CC3000_TINY_DRIVER
//      /* Connect to a secure network using WPA2, etc */
//      if (! connectSecure(ssid, key, secmode)) {
//        CHECK_PRINTER {
//          CC3KPrinter->println(F("Failed!"));
//        }
//        continue;
//      }
//#endif
//    }
//
//    timer = WLAN_CONNECT_TIMEOUT;
//
//    /* Wait around a bit for the async connected signal to arrive or timeout */
//    CHECK_PRINTER {
//      CC3KPrinter->print(F("Waiting to connect..."));
//    }
//    while ((timer > 0) && !checkConnected())
//    {
//      cc3k_int_poll();
//      delay(10);
//      timer -= 10;
//    }
//    if (timer <= 0) {
//      CHECK_PRINTER {
//        CC3KPrinter->println(F("Timed out!"));
//      }
//    }
//  } while (!checkConnected());

	return true;
}

/**************************************************************************/
/*!
 Checks if the device is connected or not

 @returns  True if connected
 */
/**************************************************************************/
bool ESP8266::checkConnected(void) {
	return esp8266Bitset.test(ESP8266BitSet::IsConnected);
}

/**************************************************************************/
/*!
 @brief  Quick socket test to pull contents from the web
 */
/**************************************************************************/
ESP8266_Client ESP8266::connectTCP(uint32_t destIP, uint16_t destPort) {
//  sockaddr      socketAddress;
//  int32_t       tcpid;
//
//  // Create the socket(s)
//  //if (CC3KPrinter != 0) CC3KPrinter->print(F("Creating socket ... "));
//  tcpid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//  if (-1 == tcpid)
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Failed to open socket"));
//    }
//    return ESP8266_Client();
//  }
//  //CC3KPrinter->print(F("DONE (socket ")); CC3KPrinter->print(tcpid); CC3KPrinter->println(F(")"));
//
//  closedids[tcpid] = false; // Clear any previous closed event
//
//  // Try to open the socket
//  memset(&socketAddress, 0x00, sizeof(socketAddress));
//  socketAddress.sa_family = AF_INET;
//  socketAddress.sa_data[0] = (destPort & 0xFF00) >> 8;  // Set the Port Number
//  socketAddress.sa_data[1] = (destPort & 0x00FF);
//  socketAddress.sa_data[2] = destIP >> 24;
//  socketAddress.sa_data[3] = destIP >> 16;
//  socketAddress.sa_data[4] = destIP >> 8;
//  socketAddress.sa_data[5] = destIP;
//
//  CHECK_PRINTER {
//    CC3KPrinter->print(F("\n\rConnect to "));
//    printIPdotsRev(destIP);
//    CC3KPrinter->print(':');
//    CC3KPrinter->println(destPort);
//  }
//
//  //printHex((byte *)&socketAddress, sizeof(socketAddress));
//  //if (CC3KPrinter != 0) CC3KPrinter->print(F("Connecting socket ... "));
//  if (-1 == ::connect(tcpid, &socketAddress, sizeof(socketAddress)))
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Connection error"));
//    }
//    closesocket(tcpid);
//    return ESP8266_Client();
//  }
//  //if (CC3KPrinter != 0) CC3KPrinter->println(F("DONE"));
	return ESP8266_Client(this, 0);
}

ESP8266_Client ESP8266::connectUDP(uint32_t destIP, uint16_t destPort) {
//  sockaddr      socketAddress;
//  int32_t       udpid;
//
//  // Create the socket(s)
//  // socket   = SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW
//  // protocol = IPPROTO_TCP, IPPROTO_UDP or IPPROTO_RAW
//  //if (CC3KPrinter != 0) CC3KPrinter->print(F("Creating socket... "));
//  udpid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//  if (-1 == udpid)
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Failed to open socket"));
//    }
//    return ESP8266_Client();
//  }
//  //if (CC3KPrinter != 0) { CC3KPrinter->print(F("DONE (socket ")); CC3KPrinter->print(udpid); CC3KPrinter->println(F(")")); }
//
//  // Try to open the socket
//  memset(&socketAddress, 0x00, sizeof(socketAddress));
//  socketAddress.sa_family = AF_INET;
//  socketAddress.sa_data[0] = (destPort & 0xFF00) >> 8;  // Set the Port Number
//  socketAddress.sa_data[1] = (destPort & 0x00FF);
//  socketAddress.sa_data[2] = destIP >> 24;
//  socketAddress.sa_data[3] = destIP >> 16;
//  socketAddress.sa_data[4] = destIP >> 8;
//  socketAddress.sa_data[5] = destIP;
//
//  CHECK_PRINTER {
//    CC3KPrinter->print(F("Connect to "));
//    printIPdotsRev(destIP);
//    CC3KPrinter->print(':');
//    CC3KPrinter->println(destPort);
//  }
//
//  //printHex((byte *)&socketAddress, sizeof(socketAddress));
//  if (-1 == ::connect(udpid, &socketAddress, sizeof(socketAddress)))
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Connection error"));
//    }
//    closesocket(udpid);
//    return ESP8266_Client();
//  }
//
	return ESP8266_Client();
}

void ESP8266::debugPrintln(String str) {
	if (debug) {
		dbgSerial->println(str);
	}
}

void ESP8266::debugPrint(String str) {
	if (debug) {
		dbgSerial->print(str);
	}
}

/**********************************************************************/
ESP8266_Client::ESP8266_Client(void) {
	id = -1;
}

ESP8266_Client::ESP8266_Client(ESP8266* esp8266, int16_t id) {
	this->esp8266 = esp8266;
	this->id = id;

//  id = s;
//  bufsiz = 0;
//  _rx_buf_idx = 0;
}

ESP8266_Client::operator bool() {
	return connected();
}

int ESP8266_Client::connect(const char *host, uint16_t port) {

//  // if (!_initialised) return 0;
//  // if (!ulCC3000Connected) return 0;
//  // if (!ulCC3000DHCP) return 0;
//
//  uint32_t ip = 0;
//
//  int16_t r = gethostbyname(host, strlen(host), &ip);
//
//  if (ip!=0 && r!=0)
//    return connect(ip, port);
//  else
//    return 0;
	return 0;
}

int ESP8266_Client::connect(IPAddress destIP, uint16_t destPort) {
//  bufsiz = 0;
//  _rx_buf_idx = 0;
//  sockaddr      socketAddress;
//  int32_t       tcpid;
//
//  // Create the socket(s)
//  //if (CC3KPrinter != 0) CC3KPrinter->print(F("Creating socket ... "));
//  tcpid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//  if (-1 == tcpid)
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Failed to open socket"));
//    }
//    return 0;
//  }
//  //CC3KPrinter->print(F("DONE (socket ")); CC3KPrinter->print(tcpid); CC3KPrinter->println(F(")"));
//
//  // Try to open the socket
//  memset(&socketAddress, 0x00, sizeof(socketAddress));
//  socketAddress.sa_family = AF_INET;
//  socketAddress.sa_data[0] = (destPort & 0xFF00) >> 8;  // Set the Port Number
//  socketAddress.sa_data[1] = (destPort & 0x00FF);
//  socketAddress.sa_data[2] = destIP >> 24;
//  socketAddress.sa_data[3] = destIP >> 16;
//  socketAddress.sa_data[4] = destIP >> 8;
//  socketAddress.sa_data[5] = destIP;
//
//  CHECK_PRINTER {
//    CC3KPrinter->print(F("\n\rConnect to "));
//    CC3KPrinter->print(destIP);
//    CC3KPrinter->print(':');
//    CC3KPrinter->println(destPort);
//  }
//
//  //printHex((byte *)&socketAddress, sizeof(socketAddress));
//  //if (CC3KPrinter != 0) CC3KPrinter->print(F("Connecting socket ... "));
//  if (-1 == ::connect(tcpid, &socketAddress, sizeof(socketAddress)))
//  {
//    CHECK_PRINTER {
//      CC3KPrinter->println(F("Connection error"));
//    }
//    closesocket(tcpid);
//    return 0;
//  }
//  // if (CC3KPrinter != 0) CC3KPrinter->println(F("DONE"));
//
//  id = tcpid;
//  return 1;
	return 0;
}

uint8_t ESP8266_Client::connected(void) {
//  if (id < 0) return false;
//
//  if (! available() && closedids[id] == true) {
//    //if (CC3KPrinter != 0) CC3KPrinter->println("No more data, and closed!");
//    closesocket(id);
//    closedids[id] = false;
//    id = -1;
//    return false;
//  }
//
//  else return true;
	return 0;

}

size_t ESP8266_Client::write(const uint8_t *buf, size_t len) {
//  return write(buf, len, 0);
	return 0;

}

size_t ESP8266_Client::write(uint8_t c) {
//  int32_t r;
//  r = send(id, &c, 1, 0);
//  if ( r < 0 ) return 0;
//  return r;
	return 0;
}

int ESP8266_Client::read(uint8_t *buf, size_t len) {
//  return read(buf, len, 0);
	return 0;

}

int32_t ESP8266_Client::close(void) {
//  int32_t x = closesocket(id);
//  id = -1;
//  return x;
	return 0;
}

void ESP8266_Client::stop() {
	close();
}

int ESP8266_Client::read(void) {
//  while ((bufsiz <= 0) || (bufsiz == _rx_buf_idx)) {
//    cc3k_int_poll();
//    // buffer in some more data
//    bufsiz = recv(id, _rx_buf, sizeof(_rx_buf), 0);
//    if (bufsiz == -57) {
//      close();
//      return 0;
//    }
//    //if (CC3KPrinter != 0) { CC3KPrinter->println("Read "); CC3KPrinter->print(bufsiz); CC3KPrinter->println(" bytes"); }
//    _rx_buf_idx = 0;
//  }
//  uint8_t ret = _rx_buf[_rx_buf_idx];
//  _rx_buf_idx++;
//  //if (CC3KPrinter != 0) { CC3KPrinter->print("("); CC3KPrinter->write(ret); CC3KPrinter->print(")"); }
//  return ret;
	return 0;
}

int ESP8266_Client::available(void) {
	// not open!
//  if (id < 0) return 0;
//
//  if ((bufsiz > 0) // we have some data in the internal buffer
//      && (_rx_buf_idx < bufsiz)) {  // we havent already spit it all out
//    return (bufsiz - _rx_buf_idx);
//  }
//
//  // do a select() call on this socket
//  timeval timeout;
//  fd_set fd_read;
//
//  memset(&fd_read, 0, sizeof(fd_read));
//  FD_SET(id, &fd_read);
//
//  timeout.tv_sec = 0;
//  timeout.tv_usec = 5000; // 5 millisec
//
//  int16_t s = select(id+1, &fd_read, NULL, NULL, &timeout);
//  //if (CC3KPrinter != 0) } CC3KPrinter->print(F("Select: ")); CC3KPrinter->println(s); }
//  if (s == 1) return 1;  // some data is available to read
//  else return 0;  // no data is available
	return 0;  // no data is available
}

void ESP8266_Client::flush() {
	// No flush implementation, unclear if necessary.
}

int ESP8266_Client::peek() {
//  while ((bufsiz <= 0) || (bufsiz == _rx_buf_idx)) {
//    cc3k_int_poll();
//    // buffer in some more data
//    bufsiz = recv(id, _rx_buf, sizeof(_rx_buf), 0);
//    if (bufsiz == -57) {
//      close();
//      return 0;
//    }
//    //if (CC3KPrinter != 0) { CC3KPrinter->println("Read "); CC3KPrinter->print(bufsiz); CC3KPrinter->println(" bytes"); }
//    _rx_buf_idx = 0;
//  }
//  uint8_t ret = _rx_buf[_rx_buf_idx];
//
//  //if (CC3KPrinter != 0) { CC3KPrinter->print("("); CC3KPrinter->write(ret); CC3KPrinter->print(")"); }
//  return ret;
	return 0;
}

