#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define WIFI_CONFIG_GET_IPS
#define __WIFI_CPP
#include "wifi_config.h"
#include "printutils.h"

void setup_wifi(void) {
	WiFi.mode(WIFI_STA);
	WiFi.config(ip, gw, nm);
	WiFi.begin(ssid, password);
	//sp(F("Connecting to wife..."));
	/* while (WiFi.waitForConnectResult() != WL_CONNECTED) */
	/* 	{ spl(F("Conn. fail! Rebooting...")); delay(5000); ESP.restart(); } */
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);       // reconnect to prior access point
}

bool setup_wait_wifi(int timeout_s) {
	int mil = millis();
	while (((millis() - mil)/1000) < timeout_s) {

}

void loop_check_wifi(int loop_millis) {
	static int connected=false;
	static int last_wifi_millis = loop_millis;
	if (loop_millis - last_wifi_millis > 3000) {
		last_wifi_millis = loop_millis;
		if (WiFi.status() == WL_CONNECTED) {
			if (!connected) { // only if we toggled state
				connected = true;
				sp(F("Just connected to "));
				sp(ssid);
				sp(". IP: ");
				spl(WiFi.localIP());
			}
		} else {
			if (!connected) {
				spl(F("Still not connected"));
			} else { // only if we toggled state
				connected=false;
				spl(F("Lost WiFi connection. Will try again."));
			}
		}
	}
}
