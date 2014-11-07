#include "ESP8266_Module.h"
#include "SoftwareSerial.h"
#include "util.h"

#define SSID "C64"
#define KEY "feelgood"

#define HOST "baidu.com"

ESP8266_Module module(&Serial, 4);
SoftwareSerial out(10, 11); // RX, TX

//The setup function is called once at startup of the sketch
void setup() {
	module.begin();
	Serial.begin(115200);
	out.begin(19200);


	if (!module.begin()) {
		out.println("Failed to start module.");
		return;
	}
	out.println("Module started.");
	if (!module.reset()) {
		out.println("Failed to reset module.");
		return;
	}
	module.checkConnected();
	out.println("Module reset.");
	uint8_t version[4];
	if (!module.getVersion(version)) {
		out.println("No module version.");
	} else {
//		out.println("Version: " + version[0] + "." + version[1] + " " + version[2] + "." + version[3]);
	}
	if (!module.connectToAP(SSID, KEY)) {
		out.println("Failed connect to AP.");
		return;
	}
	out.println("Connected to AP.");

	module.checkConnected();
	uint32_t ip = module.getIPAddress();
	char ipAdr[15];

	ip2Char(ip, ipAdr);
	out.print("Got ip: ");
	out.println(ipAdr);
	// module.getListAPs();
	// module.query(Module.AT_CWJAP);
	// module.connectTCP("220.181.111.85", 80);
	// String destIP = "aceone.se";

	ESP8266_Module_Client * client = module.connectTCP(HOST, 80);
	String cmd = "GET /status HTTP/1.0\r\nHost: ";
	cmd += HOST;
	cmd += "\r\n\r\n";

	char buf[cmd.length()];
	cmd.toCharArray(buf,cmd.length());
	client->write((uint8_t*)buf, cmd.length());
	out.println("--------------");

	while (client->available() > 0) {
		out.print(client->read());
	}

}

// The loop function is called in an endless loop
void loop() {
//Add your repeated code here
}

