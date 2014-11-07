/*
 * ESP8266_Module.h
 *
 *  Created on: 27 okt 2014
 *      Author: Hernik
 */

#ifndef ESP8266_MODULE_H_
#define ESP8266_MODULE_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Client.h"
#include "IPAddress.h"

const char AT[] = "AT";
const char AT_GMR[] = "AT+GMR";
const char AT_RST[] = "AT+RST";

const char AT_CWLAP[] = "AT+CWLAP";
const char AT_CWMODE[] = "AT+CWMODE";
const char AT_CWJAP[] = "AT+CWJAP";

const char AT_CIFSR[] = "AT+CIFSR";
const char AT_CIPMUX[] = "AT+CIPMUX";
const char AT_CIPSEND[] = "AT+CIPSEND";
const char AT_CIPSTART[] = "AT+CIPSTART";
const char AT_CIPCLOSE[] = "AT+CIPCLOSE";

const char* AT_CIPSTATUS = "AT+CIPSTATUS";

const char* CIPSTATUS = "+CIPSTATUS:";

const char* SEND_DATA = ">";
const char* SEND_OK = "SEND OK";

#define TIME_OUT 	 5000

const char BUSY[] = "busy";
const char ERROR[] = "ERROR";
const char ERROR_1[] = "error";
const char* ERRORS[] = { ERROR, BUSY, ERROR_1 };

const char* READY = "ready";
const char* OK = "OK";
const char* ALREAY_CONNECT = "ALREAY CONNECT";
const char* const LINKED = "Linked";
const char* NO_CHANGE = "no change";
const char* DOT = ".";

#define CLIENT_BUFFER_SIZE	128

#define RX_BUFFER_SIZE  64 // how much to buffer on the incoming side#define TX_BUFFER_SIZE  32 // how much to buffer on the outgoing sideclass ESP8266_Module;class ESP8266_Module_Client: public Client {public:ESP8266_Module_Client(ESP8266_Module* module, int id);
ESP8266_Module_Client(void);

int connect(IPAddress ip, uint16_t port);
int connect(const char *host, uint16_t port);

uint8_t connected(void);
size_t write(uint8_t c);

int read(void);
int32_t close(void);
int available(void);

int read(uint8_t *buf, size_t size);
size_t write(const uint8_t *buf, size_t size);
int peek();
void flush();
void stop();
operator bool();

void addToBuffer(int b);
void setConnected(bool connected);
int allocateBuffer(int size);

private:

int id;
ESP8266_Module* module;
uint8_t _connected;

int buffer[CLIENT_BUFFER_SIZE];

int writePos;
int readPos;
};

class ESP8266_Module {
public:
	ESP8266_Module(Stream * serial, uint8_t rstPin);
	bool begin();
	bool reset();
	bool hardReset();
	void stop(void);
	bool disconnect(void);

	bool connectToAP(const char *ssid, const char *key);
	bool checkConnected(void);
	bool checkDHCP(void);

	uint32_t getIPAddress(void);

	ESP8266_Module_Client* connectTCP(const IPAddress* ip, uint16_t destPort);
	ESP8266_Module_Client* connectUDP(const IPAddress* ip, uint16_t destPort);
	ESP8266_Module_Client* connectTCP(const char* destAdr, uint16_t destPort);
	ESP8266_Module_Client* connectUDP(const char* destAdr, uint16_t destPort);
	ESP8266_Module_Client* connectTCP(uint32_t destIP, uint16_t destPort);
	ESP8266_Module_Client* connectUDP(uint32_t destIP, uint16_t destPort);


	ESP8266_Module_Client* connect(const char* destAdr, uint16_t destPort, const char* type);

	bool getVersion(uint8_t *version);

	bool startSSIDscan(uint32_t *index);
	void stopSSIDscan();
	uint8_t getNextSSID(uint8_t *rssi, uint8_t *secMode, char *ssidname);

	bool listSSIDResults(void);

	bool scanSSIDs(uint32_t time);

	void checkConnectionStatus(void);
	int available(void);
	size_t write(const char* buf);
	size_t write(const char * buf, size_t ind, size_t len);

	size_t read(uint8_t *buf, const char * good);
	size_t read(uint8_t *buf, const char ** good, int arraySize);
	size_t read(uint8_t *buf, const char ** good, int arraySize, uint8_t* found);
	bool readToClientBuffer();

	char rx_buf[RX_BUFFER_SIZE];
	char tx_buf[TX_BUFFER_SIZE];
private:

	bool initialised;
	int connected;
	int clientId;
	uint8_t rstPin;

	ESP8266_Module_Client clients[4];
	bool bufferToSmal;
	int remainingSize;

	Stream * serial;

};

#endif /* ESP8266_MODULE_H_ */
