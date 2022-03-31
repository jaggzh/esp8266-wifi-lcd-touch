#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define WIFI_CONFIG_GET_IPS
#define __WIFI_CPP
#include "esp8266-wifi-lcd-touch.h"
#include "wifi_config.h"
#include "printutils.h"
#include "wifi.h"

#ifdef USE_MQTT
#include "EspMQTTClient.h"
EspMQTTClient mqtt_client(MQTT_IP, MQTT_PORT, MQTT_USER, MQTT_PW, MQTT_CLIENTNAME);
#endif

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

// Wait max of passed seconds for wifi
// Returns true immediately upon success
// False after timeout expires
bool setup_wait_wifi(int timeout_s) {
	int mil = millis();
	bool ret;
	while (((millis() - mil)/1000) < timeout_s) {
		ret = loop_check_wifi(); // after 3s this fn will start printing to serial
		if (ret) return ret;
		delay(1000);
	}
	return false;
}

void loop_wifi() {        // Required for loop updates
	#ifdef USE_MQTT
		mqtt_client.loop();
	#endif
}

void show_wifi_strength() {
	// init at -1 .. to know
	static int16_t  last_x1=-1, last_y1;
	static uint16_t last_w, last_h;
	const int xoff=52, yoff=26; // offset from bottom right
	const int xpos=lcd_width-xoff, ypos=lcd_height-yoff;
	uint16_t cx=lcd.getCursorX(), cy=lcd.getCursorY();
	//spl(F"Printing WIFI strength");
	long rssi = WiFi.RSSI();
	lcd.setTextSize(2);
	lcd.setCursor(xpos, ypos);
	// clear last text
	if (last_x1 != -1) lcd.fillRect(last_x1, last_y1, last_w, last_h, 0);
	lcd.setTextColor(rgb24to565(100,255,255), 0);
	lcd.print(rssi);
	lcd.getTextBounds(String(rssi), xpos, ypos, &last_x1, &last_y1, &last_w, &last_h);
	set_text_color_honoring_trans(txtfg.c);
	lcd.setTextSize(txtsize);
	lcd.setCursor(cx, cy);
}

bool loop_check_wifi() {
	static int connected=false;
	int cur_millis = millis();
	static int last_wifi_millis = cur_millis;
	static int last_connect_millis = 0;
	if (cur_millis - last_wifi_millis > 4000) {
		last_wifi_millis = cur_millis;
		if (WiFi.status() == WL_CONNECTED) {
			if (!connected) { // only if we toggled state
				connected = true;
				last_connect_millis = cur_millis;
				sp(F("Just connected to "));
				sp(ssid);
				sp(". IP: ");
				spl(WiFi.localIP());
				return true;
			}
		} else {
			if (!connected) {
				spl(F("Still not connected"));
				if (cur_millis - last_connect_millis > MAX_MS_BEFORE_RECONNECT) {
					WiFi.reconnect();
				}
			} else { // only if we toggled state
				connected=false;
				spl(F("Lost WiFi connection. Will try again."));
			}
		}
		show_wifi_strength();
	}
	return false;
}

void onConnectionEstablished() {
	#ifdef USE_MQTT
		mqtt_client.subscribe("lr/lcd/px",[](const String & payload) {
			mqtt_pixel(payload); },
			/* main_debug(payload); }, */
			1);
			//Serial.println(payload);});
		//client.publish("mytopic/test", "This is a message");
	#endif
}
