/*
 * ESP8266_Module.cpp
 *
 *  Created on: 27 okt 2014
 *      Author: Hernik
 */

#include "ESP8266_Module.h"
#include "util.h"

void debugPrintln(const char* str);

void debugPrintln(const char* str, int pos);

void debugPrint(const char* str);

void debugPrint(const char* str, int pos);

/* *********************************************************************** */
/*                                                                         */
/* CONSTRUCTORS */
/*                                                                         */
/* *********************************************************************** */

ESP8266_Module::ESP8266_Module(Stream * _serial, uint8_t rstPin) {
	this->serial = _serial;
	initialised = false;
	remainingSize = 0;
	connected = 0;
	clientId = -1;
	bufferToSmal = false;
	this->rstPin = rstPin;

	pinMode(rstPin, OUTPUT);
	digitalWrite(rstPin, HIGH);
}

/* *********************************************************************** */
/*                                                                         */
/* PUBLIC FUNCTIONS */
/*                                                                         */
/* *********************************************************************** */

bool ESP8266_Module::begin() {
	if (initialised) {
		return true;
	}

	initialised = true;
	write(AT);
	size_t s = read((uint8_t *) rx_buf, OK);
	if (s <= 0) {
		return false;
	}
	char* c = strcpy(tx_buf, AT_CWMODE);
	c = strcpy(c, "=1");
	write(tx_buf, 0, c - tx_buf);
//	write(AT_CWMODE + "=1");
	const char* a[] = { OK, NO_CHANGE };
	s = read((uint8_t*) rx_buf, a, 2);
	if (s <= 0) {
		return false;
	}

	return true;
}

bool ESP8266_Module::reset() {
	if (!initialised) {
		return false;
	}
	write(AT_RST);
	int s = 0;
	uint8_t found;
	while (found == 0) {
		const char* a[] = { READY };
		int ss = read((uint8_t*) rx_buf, a, 1, &found);
		s += ss;
	}
	return s >= 0;
}

bool ESP8266_Module::hardReset(){
	digitalWrite(rstPin, LOW);
	delay(200);
	digitalWrite(rstPin, HIGH);
	size_t s = read((uint8_t*) rx_buf, READY);
	return s >= 0;
}



bool ESP8266_Module::getVersion(uint8_t* version) {
	if (!initialised) {
		return false;
	}
	write(AT_GMR);
	size_t s = read((uint8_t*) rx_buf, OK);
	if (s <= 0) {
		return false;
	}

	int ind = indexOf(rx_buf, s, AT_GMR);
	for (int i = 0; i < 4; i++) {
		int index = ind + 9 + i * 2;
		char * s = &rx_buf[index];
		char * e = &rx_buf[index + 2];
		version[i] = (int) strtol(s, &e, 10);
	}

	return true;
}

void ESP8266_Module::stop() {
	if (!initialised) {
		return;
	}

}

bool ESP8266_Module::disconnect() {
	if (!initialised) {
		return false;
	}
	return true;
}

//	bool ESP8266_Module::getMacAddress(char[] address)  {
//		if (!initialised) {
//			return false;
//		}
//		return true;
//	}

//void ESP8266_Module::getListAPs() {
//	write(AT_CWLAP);
//	int pos = read(buf, new String[] { OK });
//	debugPrint(buf, pos);
//}

uint32_t ESP8266_Module::getIPAddress() {
	write(AT_CIFSR);

	size_t s = read((uint8_t*) rx_buf, DOT);
	if (s <= 0) {
		return 0;
	}
	while (available() > 0) {
		rx_buf[s++] = (char) serial->read();
	}
	uint32_t ip;
	int ind = indexOf(rx_buf, s, AT_CIFSR) + 11;
	for (int i = 0; i < 4; i++) {
		int lastInd = indexOf(rx_buf, s, DOT, ind);
		if (lastInd < 0) {
			lastInd = s - 2;
		}
		ip <<= 8;
		char * s = &rx_buf[ind];
		char * e = &rx_buf[lastInd];
		ip |= strtol(s, &e, 10);

		//ip[i] = Integer.parseInt(new String(rx_buf, ind, lastInd - ind));
		ind = lastInd + 1;
	}
	return true;
}

bool ESP8266_Module::connectToAP(const char * ssid, const char * key) {
	if (!initialised) {
		return false;
	}
	char* c = strcpy(tx_buf, AT_CWJAP);
	c = strcpy(c, "=\"");
	c = strcpy(c, ssid);
	c = strcpy(c, "\",\"");
	c = strcpy(c, key);
	c = strcpy(c, "\"");
	write(tx_buf, 0, c - tx_buf);
//	write(AT_CWJAP + "=\"" + ssid + "\",\"" + key + "\"");

	size_t s = read((uint8_t*) rx_buf, OK);
	// System.out.println(s);
	if (s <= 0) {
		return false;
	}

	c = strcpy(tx_buf, AT_CIPMUX);
	c = strcpy(c, "=1");
	write(tx_buf, 0, c - tx_buf);
//	write(AT_CIPMUX + "=1");
	s = read((uint8_t*) rx_buf, OK);
	if (s <= 0) {
		return false;
	}
	unsigned long timeOut = millis() + TIME_OUT;
	IPAddress ip;
	while (!getIPAddress()) {
		if (timeOut < millis()) {
			return false;
		}
		delay(500);
	}
	connected = true;
	return true;
}

void ESP8266_Module::checkConnectionStatus() {
	// AT+CIPSTATUS
	//
	// STATUS:3
	// +CIPSTATUS:1,"TCP","77.53.152.17",80,0
	// +CIPSTATUS:2,"TCP","109.105.111.14",80,0
	// +CIPSTATUS:3,"TCP","80.76.152.133",80,0
	// +CIPSTATUS:4,"TCP","144.63.250.10",80,0
	//
	// OK

	write(AT_CIPSTATUS);

	bool con[4];
	while (true) {
		const char* a[] = { OK, CIPSTATUS };
		size_t s = read((uint8_t*) rx_buf, a, 2);
		if (endsWith(rx_buf, s, CIPSTATUS)) {
			con[serial->read() - '0' - 1] = true;
		} else {
			break;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (clients[i] != NULL) {
			clients[i].setConnected(con[i]);
		}
	}
}

bool ESP8266_Module::checkConnected() {
	return connected;
}

ESP8266_Module_Client* ESP8266_Module::connectTCP( uint32_t ip, uint16_t destPort) {
	ip2Char(ip, tx_buf);
	return connectTCP(tx_buf, destPort);
}
ESP8266_Module_Client* ESP8266_Module::connectUDP( uint32_t ip, uint16_t destPort) {
	ip2Char(ip, tx_buf);
	return connectUDP(tx_buf, destPort);
}
ESP8266_Module_Client* ESP8266_Module::connectTCP(const IPAddress* ip, uint16_t destPort) {
	return connectTCP((uint32_t)ip, destPort);
}
ESP8266_Module_Client* ESP8266_Module::connectUDP(const IPAddress* ip, uint16_t destPort) {
	return connectUDP((uint32_t)ip, destPort);
}
ESP8266_Module_Client* ESP8266_Module::connectTCP(const char* destAdr, uint16_t destPort) {
	return connect(destAdr, destPort, "TCP");
}

ESP8266_Module_Client* ESP8266_Module::connectUDP(const char* destAdr, uint16_t destPort) {
	return connect(destAdr, destPort, "UDP");
}

ESP8266_Module_Client* ESP8266_Module::connect(const char* dest, uint16_t destPort, const char* type) {
	if (!connected) {
		return NULL;
	}
	int clientNr = 0;
	for (int i = 0; i < 4; i++) {
		if (clients[i] == NULL || !clients[i].connected()) {
			clientNr = i;
			break;
		}
	}
	char * c = strcpy(tx_buf, AT_CIPSTART);
	c = strcpy(c, "=");
	char cln = '0' + clientNr + 1;
	c = strcpy(c, &cln);
	c = strcpy(c, ",\"");
	c = strcpy(c, type);
	c = strcpy(c, "\",\"");
	c = strcpy(c, dest);
	c = strcpy(c, "\",");
	char buf[5];
	itoa(destPort, buf, 10);
	c = strcpy(c, buf);
	write(tx_buf, 0, c - tx_buf);
//	write(AT_CIPSTART + "=" + (clientNr + 1) + ",\"" + type + "\",\"" + dest + "\"," + destPort);

	int s = read((uint8_t*) rx_buf, LINKED);
	if (s <= 0) {
		return NULL;
	}

	clients[clientNr] = ESP8266_Module_Client(this, clientNr + 1);
	return &clients[clientNr];
}

size_t ESP8266_Module::read(uint8_t* res, const char* good) {
	const char* a[] = { good };
	return read(res, a, 1);
}
size_t ESP8266_Module::read(uint8_t* res, const char** good, int arraySize) {
	uint8_t* f = 0;
	return read(res, good, arraySize, f);
}

size_t ESP8266_Module::read(uint8_t* res, const char** good, int arraySize, uint8_t* found) {
	int rSise = strlen((char*) res);
	int pos = 0;
	unsigned long timeOut = millis() + TIME_OUT;
	while (timeOut > millis()) {
		while (serial->available() > 0) {
			if (bufferToSmal) {
				if (readToClientBuffer()) {
					found[0] = 0;
					return 0;
				};
			}

			res[pos++] = (char) serial->read();
			if (endsWith((char*) res, pos, "+IPD,") && serial->available() > 3) {
				clientId = ((char) serial->read()) - '0' - 1;
				serial->read();
				remainingSize = 0;
				char c;
				while ((c = (char) serial->read()) != ':') {
					remainingSize *= 10;
					remainingSize += (c - '0');
				}
				if (readToClientBuffer()) {
					found[0] = 0;
					return pos;
				};
			}
			if (endsWith((char*) res, pos, ERRORS, 3)) {
				debugPrintln((char*) res, pos);
				return -1;
			}
			if (endsWith((char*) res, pos, good, 2) /* || rSise-1 <= pos */) {
				found[0] = 1;
				return pos;
			}
			if (rSise - 1 <= pos) {
				found[0] = 0;
				return pos;
			}
		}
		delay(200);
	}
	debugPrint("Time out ");
	debugPrint((char*) res, pos);
	return -2;
}

bool ESP8266_Module::readToClientBuffer() {
	int bufSize = clients[clientId].allocateBuffer(remainingSize);
	bufferToSmal = bufSize < remainingSize;
	remainingSize = remainingSize - bufSize;
	for (int i = 0; i < bufSize; i++) {
		clients[clientId].addToBuffer(serial->read());
	}
	return bufferToSmal;
}

size_t ESP8266_Module::write(const char* buf) {
	debugPrintln(buf);
	size_t l = serial->println(buf);
	serial->flush();
	return l;
}

size_t ESP8266_Module::write(const char* buf, size_t ind, size_t len) {
	debugPrintln(buf);
	size_t l;
	for (size_t i = ind; i < len; i++) {
		l += serial->print(buf[i]);
	}
	l = serial->println();
	serial->flush();
	return l;
}

int ESP8266_Module::available() {
	return serial->available();
}

ESP8266_Module_Client::ESP8266_Module_Client() {
	id = -1;
	this->_connected = false;
	this->writePos = 0;
	this->readPos = 0;
	this->module = NULL;
}

ESP8266_Module_Client::ESP8266_Module_Client(ESP8266_Module * module, int id) {
	this->module = module;
	this->id = id;
	this->_connected = true;
	this->writePos = 0;
	this->readPos = 0;
}

uint8_t ESP8266_Module_Client::connected() {
	if (id < 0) {
		return false;
	}
	module->checkConnectionStatus();
	return _connected;
}


size_t ESP8266_Module_Client::write(const uint8_t * buf, size_t len) {
	if (!connected()) {
		return -1;
	}
	char * c = strcpy(module->tx_buf, AT_CIPSEND);
	c = strcpy(c, "=");
	char cid = '0' + id;
	c = strcpy(c, &cid);
	c = strcpy(c, ",");
	char cLen[5];
	itoa(len + 1, cLen, 10);
	c = strcpy(c, cLen);

	// TODO is this correct c - &module->tx_buf[0] might be c - &module->tx_buf
	module->write(module->tx_buf,0, c - &module->tx_buf[0]);

//	size_t l = module->write(AT_CIPSEND + "=" + id + "," + (len + 1));
	size_t s = module->read((uint8_t *) module->rx_buf, SEND_DATA);
//	debugPrint((char *) buf, s);
	size_t l = module->write((char *) buf, 0, len);
	s = module->read((uint8_t *) module->rx_buf, SEND_OK);
	return l;
}

int ESP8266_Module_Client::read() {
	if (!connected()) {
		return -1;
	}

	return buffer[readPos++];
}

int ESP8266_Module_Client::read(uint8_t *buf, size_t size) {
	if (!connected()) {
		return -1;
	}
// TODO implement
	// return read(buf, len, 0);
	return 0;
}

int32_t ESP8266_Module_Client::close() {
	char * c = strcpy(module->tx_buf, AT_CIPCLOSE);
	c = strcpy(c, "=");
	char cid = '0' + id;
	c = strcpy(c, &cid);
	// TODO is this correct c - &module->tx_buf[0] might be c - &module->tx_buf
	module->write(module->tx_buf,0, c - &module->tx_buf[0]);
//	module->write(AT_CIPCLOSE + "=" + id);
	module->read((uint8_t *) module->rx_buf, OK);
	_connected = 0;
	return 1;
}

void ESP8266_Module_Client::stop() {
	close();
}


int ESP8266_Module_Client::available() {
	if (!connected()) {
		return -1;
	}
	int available = writePos - readPos;
	if (available > 0) {
		return available;
	}
	if (module->available() > 0) {
		// TODO
//		module.read(module.buf,new String[] {Module.OK});
	}
	return writePos - readPos;
}

void ESP8266_Module_Client::flush() {
	// No flush implementation, unclear if necessary.
}

int ESP8266_Module_Client::peek() {
	if (!connected()) {
		return -1;
	}
	return buffer[readPos];
}

void ESP8266_Module_Client::setConnected(bool connected) {
	_connected = connected;
}

int ESP8266_Module_Client::allocateBuffer(int size) {
	if (writePos - readPos <= 0) {
		this->writePos = 0;
		this->readPos = 0;
	}
	if (size < CLIENT_BUFFER_SIZE - writePos) {
		return size;
	}
	return CLIENT_BUFFER_SIZE - writePos;
}

void ESP8266_Module_Client::addToBuffer(int b) {
	buffer[writePos++] = b;
}


int ESP8266_Module_Client::connect(IPAddress ip, uint16_t port){
	return 0;
}
int ESP8266_Module_Client::connect(const char *host, uint16_t port){
	return 0;
}

ESP8266_Module_Client::operator bool()
{
  return connected();
}



/*
 * debug methods
 */

void debugPrintln(const char* str) {
	debugPrintln(str, strlen(str));
}

void debugPrintln(const char* str, int pos) {
//			System.out.println("d: " + new String(str, 0, pos));
}

void debugPrint(const char* str) {
	debugPrint(str, strlen(str));
}

void debugPrint(const char* str, int pos) {
//			System.out.print(new String(str, 0, pos));
}
