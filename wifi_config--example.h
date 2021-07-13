#ifndef __WIFI_CONFIG_H
#define __WIFI_CONFIG_H

#include <ESP8266WiFi.h>

#define MDNS_NAME "LanDevice"
#define SSID_NAME "YourSSID"
#define SSID_PW   "YourPASSWORD"
// The below might not be implemented
#define WEBUPDATE_USER "webuser"
#define WEBUPDATE_PW   "webpw"

#ifdef __WIFI_CPP
const char *ssid = SSID_NAME;
//#define SSPW {33+22, 3, 62+22, 4+2, 6+129, 0}
char password[] = SSID_PW;

const char *update_user = WEBUPDATE_USER; // HTTP auth user for OTA http update
const char *update_pw = WEBUPDATE_PW";  // HTTP auth password

#else // like: ifndef __MAIN_INO__
extern const char *ssid;
extern char password[];
extern const char *update_user; // HTTP auth user for OTA http update
extern const char *update_pw;  // HTTP auth password
#endif

#ifdef WIFI_CONFIG_GET_IPS
IPAddress ip(192, 168, 1, 14);
IPAddress gw(192, 168, 1, 1);
IPAddress nm(255, 255, 255, 0);
#endif

#define MQTT_IP         "192.168.1.11"     // IP of MQTT Broker
#define MQTT_PORT       1883
#define MQTT_USER       "mqtt_sub_user"    // set your user here
#define MQTT_PW         "mqtt_sub_userpw"
#define MQTT_CLIENTNAME "LCDClient"


#endif
