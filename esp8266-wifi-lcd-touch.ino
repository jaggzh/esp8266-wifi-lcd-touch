/**The MIT License (MIT)
Copyright (c) 2021 by jaggz.h .... .... gmail
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 Required libraries:
  - XPT2046: https://github.com/spapadim/XPT2046
  - UTFT-ESP8266: https://github.com/gnulabis/UTFT-ESP8266
  - Adafruit_ILI9341esp library from the subdirectory at:
	  https://github.com/jaggzh/esp8266-ili9341-touch-and-display-rotation
	  (it's not that hard. clone the repo and plop that directory in your
	  Arduino/libraries/ folder)
 Modify: XPT2046.cpp
 void XPT2046::begin(uint16_t width, uint16_t height) { 
   ...
   //SPI.begin(); //comment out
   ...
 }  
*/
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ILI9341esp_read.h>
#include <Adafruit_GFX.h>
#include <XPT2046.h>
#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266mDNS.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "ESP8266_240x320_LCD_Touch_Buttons-WiFi.h"
#include "printutils.h"
#define __MAIN_INO__
#include "settings.h"
#include "libsecretebmp.h"

#include "wifi_config.h" // This sets actual values (gross): ssid, password. ip, gw, nm
#include "printutils.h"
#include "ota.h"
#include "wifi.h"

#define ESP_SPI_FREQ 4000000
/* IMAGE data */
#define img_big_w 320
#define img_big_h 240
#define ALIGN     __attribute__ (( aligned ( sizeof(char*) ) ))
//#define ALIGN     __attribute__ (( aligned (__BIGGEST_ALIGNMENT__) ))
#define INFLASH   PROGMEM ALIGN
//extern unsigned int img_big[76800] INFLASH;
/* /IMAGE data */
// Modify the following two lines to match your hardware
// Also, update calibration parameters below, as necessary
// For the esp shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 lcd = Adafruit_ILI9341(TFT_CS, TFT_DC);
XPT2046 touch(/*cs=*/ 4, /*irq=*/ 5);
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";
int loop_millis=millis(); // at start of loop(), so everthing can use
// Text colors
ColorSet txtfg(255,255,255);
ColorSet txtbg(255,255,255); // adafruit says same == transparent bg
ColorSet nxtclr(255,255,255);
uint8_t txtsize=1;
ESP8266WebServer server(80); //main web server
ESP8266HTTPUpdateServer httpUpdater;

#if 0
void drawButtons() {
	int x, y;
// Draw the upper row of buttons
	for (x = 0; x < 5; x++) {
		lcd.setColor(0, 0, 255);
		lcd.fillRoundRect(10 + (x * 60), 10, 60 + (x * 60), 60);
		lcd.setColor(255, 255, 255);
		lcd.drawRoundRect(10 + (x * 60), 10, 60 + (x * 60), 60);
		lcd.printNumI(x + 1, 27 + (x * 60), 27);
	}
// Draw the center row of buttons
	for (x = 0; x < 5; x++) {
		lcd.setColor(0, 0, 255);
		lcd.fillRoundRect(10 + (x * 60), 70, 60 + (x * 60), 120);
		lcd.setColor(255, 255, 255);
		lcd.drawRoundRect(10 + (x * 60), 70, 60 + (x * 60), 120);
		if (x < 4)
			lcd.printNumI(x + 6, 27 + (x * 60), 87);
	}
	lcd.print((char *)"0", 267, 87);
// Draw the lower row of buttons
	lcd.setColor(0, 0, 255);
	lcd.fillRoundRect(10, 130, 150, 180);
	lcd.setColor(255, 255, 255);
	lcd.drawRoundRect(10, 130, 150, 180);
	lcd.print((char *)"Clear", 40, 147);
	lcd.setColor(0, 0, 255);
	lcd.fillRoundRect(160, 130, 300, 180);
	lcd.setColor(255, 255, 255);
	lcd.drawRoundRect(160, 130, 300, 180);
	lcd.print((char *)"Enter", 190, 147);
	lcd.setBackColor(0, 0, 0);
	wdt_enable(3000);
	//lcd.drawBitmap(0, 0, img_big_w, img_big_h, (unsigned int *) img_big, 1);
	Serial.println("Bitmap drawn");
	yield();
}
#endif
// Draw a red frame while a button is touched
#if 0
void waitForIt(int x1, int y1, int x2, int y2) {
	lcd.setColor(255, 0, 0);
	lcd.drawRoundRect(x1, y1, x2, y2);
	while (touch.isTouching()) yield();				//loop
	lcd.setColor(255, 255, 255);
	lcd.drawRoundRect(x1, y1, x2, y2);
}
#endif

void httptext() {
	server.sendContent("Content-Type: text/html\r\n\r\n");
}
void httpbmp() {
	server.sendContent("Content-Type: image/bmp\r\n\r\n");
}
void http200plain() {
	server.sendContent("HTTP/1.0 200 OK\r\n");
}
void http200() {
	http200plain();
	httptext();
}
void http500() {
	server.sendContent("HTTP/1.0 500 Not OK\r\n");
	httptext();
}
unsigned char *callback_preview_row_by_pixel(void *cbd) {
	// Remember, 24 bit pixel data for BMP is in BGR order
	// Also, BMP does bottom left corner first, so we work our way up
	static int w=lcd.width();
	static int y=lcd.height();
	static unsigned char *row=NULL;
	if (row==NULL) row = (unsigned char *)malloc(w*3+3); // rgb*width + 1 for dumb
	y--;
	for (int x=0; x<w; x++) {
		uint8_t r, g, b;
		lcd.readPixel(&r, &g, &b, x, y);
		//rgb24from565(&r, &g, &b, pix565);
		row[x*3+2] = r;
		row[x*3+1] = g;
		row[x*3+0] = b;
	}
	if (y <= 0) y=lcd.height();
	return row;
}
unsigned char *callback_preview_row_by_row(void *cbd) {
	// Remember, 24 bit pixel data for BMP is in BGR order
	// Also, BMP does bottom left corner first, so we work our way up
	static int w=lcd.width();
	static int y=lcd.height();
	static unsigned char *row=NULL;
	if (row==NULL) row = (unsigned char *)malloc(w*3+3); // rgb*width + 1 for dumb
	y--;
	lcd.readRow(row, y);
	for (int i=0; i<w*3; i+=3) {
		unsigned char c;
		c        = row[i+0];
		row[i+0] = row[i+2];
		row[i+2] = c;
	}
	if (y <= 0) y=lcd.height();
	return row;
}
void callback_writer(unsigned char *data, int size, int nmemb, void *cbdata) {
	server.sendContent((const char*) data, size*nmemb);
}
bool hand_img_preview_by_pixel(void) {
	http200plain();
	httpbmp();
    generateBitmapImageByRow(
    		&callback_preview_row_by_pixel, &callback_writer, NULL, NULL,
    		lcd.height(), lcd.width());
    return true;
}
bool hand_img_preview(void) {
	http200plain();
	httpbmp();
    generateBitmapImageByRow(
    		&callback_preview_row_by_row, &callback_writer, NULL, NULL,
    		lcd.height(), lcd.width());
	server.sendContent("");
    return true;
}

void hand_post_img(void) {
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/"+filename;
		Serial.print("handleFileUpload Name: ");
		Serial.println(filename);
		//fsUploadFile = SPIFFS.open(filename, "w");     // Open the file for writing in SPIFFS (create if it doesn't exist)
		filename = String();
	} else if (upload.status == UPLOAD_FILE_WRITE) {
		/* if (fsUploadFile) */
		/* 	fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file */
	} else if (upload.status == UPLOAD_FILE_END) {
		/* if (fsUploadFile) {                                    // If the file was successfully created */
		/* 	fsUploadFile.close();                               // Close the file again */
		/* 	Serial.print("handleFileUpload Size: "); */
		/* 	Serial.println(upload.totalSize); */
		/* 	server.sendHeader("Location","/success.html");      // Redirect the client to the success page */
		/* 	server.send(303); */
		/* } else { */
		/* 	server.send(500, "text/plain", "500: couldn't create file"); */
		/* } */
	}
}

void hand_lcd_status(void) {
	http200();
	bool row_addr_order, col_addr_order, row_col_exchange,
       vert_refresh, rgbbgr, hor_refresh;
	lcd.readDisplayStatus(&row_addr_order, &col_addr_order,
            &row_col_exchange, &vert_refresh, &rgbbgr, &hor_refresh);
	server.sendContent("<!DOCTYPE html><html><head><meta charset='UTF-8' /><title>Guidance</title>"
		"<style type='text/css'>"
		"</style></head><body><ul>");
	server.sendContent("<li>row_addr_order: ");
	server.sendContent(BOO(row_addr_order));
	server.sendContent("<li>col_addr_order: ");
	server.sendContent(BOO(col_addr_order));
	server.sendContent("<li>row_col_exchange: ");
	server.sendContent(BOO(row_col_exchange));
	server.sendContent("<li>vert_refresh: ");
	server.sendContent(BOO(vert_refresh));
	server.sendContent("<li>rgbbgr: ");
	server.sendContent(BOO(rgbbgr));
	server.sendContent("<li>hor_refresh: ");
	server.sendContent(BOO(hor_refresh));
	server.sendContent("</ul></body></html>");
	server.sendContent("");
    return true;
}
bool hand_root(void) {
	http200();
	server.sendContent("<!DOCTYPE html><html><head><meta charset='UTF-8' /><title>Guidance</title>"
		"<style type='text/css'>"
		"</style></head><body>");
	server.sendContent("<img src=/preview.bmp /><br />");
	server.sendContent("</style></head><body>");
	server.sendContent("</body></html>");
	server.sendContent("");
    return true;
}
bool hand_cmd_list() {
	// /cs?font=&f=b&|=&txt=&
	for (uint8_t i=0; i<server.args(); i++) {
		String cmd=server.argName(i);
		String vals;
		sp("\nInput arg ["); sp(i); sp("]="); sp(cmd);
		sp(" length="); spl(cmd.length());
		vals = server.arg(i);
		sp(" String val=("); sp(vals); sp(") Length:"); spl(vals.length());
		char val[vals.length()+1];
		vals.toCharArray(val, vals.length()+1);
		sp(" char *val=("); sp(val); sp(")  Length:");
		spl(strlen(val));
		if (cmd.equals("txt")) {
			spl("  COMMAND: txt");
			cmd_txt(val);
		} else if (cmd.equals("px")) {
			spl("  COMMAND: px");
			cmd_pixel(val);
		} else if (cmd.equals("fnt")) {
			spl("  COMMAND: fnt");
			//cmd_font(val);
		} else if (cmd.equals("tfg")) {
			spl("  COMMAND: tfg");
			if (!cmd_color(txtfg, val)) lcd.setTextColor(txtfg.c);
		} else if (cmd.equals("tbg")) {
			spl("  COMMAND: tbg");
			if (!cmd_color(txtbg, val)) lcd.setTextColor(txtfg.c, txtbg.c);
		} else if (cmd.equals("col")) {
			spl("  COMMAND: col");
			cmd_color(nxtclr, val);
		} else if (cmd.equals("cls")) {
			spl("  COMMAND: cls");
			cmd_clear(val);
		} else if (cmd.equals("off")) {
			spl("  COMMAND: off");
			cmd_off(val);
		} else if (cmd.equals("rect")) {
			spl("  COMMAND: rect");
			cmd_rect(val, 0); // NOT filled
		} else if (cmd.equals("frect")) {
			spl("  COMMAND: frect");
			cmd_rect(val, 1); // filled
		} else {
			http500();
			server.sendContent("Unknown command: " + cmd + "\n");
		}
	}
	http200();
	server.sendContent("k\n");
	server.sendContent("");
    return true;
}
int cmd_off(char *val) {
	lcd.displayOff();
	return 1;
}
int cmd_font(char *name) { // f=[sbn]
	String font;
	// SmallFont 8x12, BigFont 16x16,
	//  SevenSegNumFont 32x50 (only #s), 
	/* if (name[0]='s') lcd.setFont(SmallFont); */
	/* else if (name[0]='b') lcd.setFont(BigFont); */
	/* else if (name[0]='n') lcd.setFont(SevenSegNumFont); */
	/* else { */
		http500();
		//server.sendContent("Not a font\n");
		server.sendContent("Not implemented\n");
		return 1;
	/* } */
	/* return 0; */
}
int cmd_txt(char *opts) {
	// t={text}  x=#  y=#
	SubParams pset(opts); // for &foo=a=b,c=d,e...
	char *var, *val;
	char *txt=NULL;
	int x=0, y=0;
	bool usercoords=false;
	while (pset.next(&var, &val)) {
		dbsp("Var:"); dbsp(var); dbsp(" val:"); dbspl(val);
		if (*var == 't')      txt = val;
		else if (*var == 'x') x = !val ? 0 : strtol(val, NULL, 10),
		                      usercoords=true;
		else if (*var == 'y') y = !val ? 0 : strtol(val, NULL, 10),
		                      usercoords=true;
		else if (*var == 's') {
			uint8_t s = !val ? 0 : strtol(val, NULL, 10);
			lcd.setTextSize(s);
			txtsize=s;
		} else {
			http500();
			server.sendContent("txt: Invalid opt (");
			server.sendContent(var);
			server.sendContent(")\n");
			return 1;
		}
	}
	if (usercoords) lcd.setCursor(x,y);
	lcd.print(txt);
	return 0;
}

int cmd_rect(char *opts, bool filled=0) {
	SubParams pset(opts); // for &foo=a=b,c=d,e...
	char *var, *val;
	int args[5] = {1,1,15,5,1}; // x,y,w,h,r
	uint8_t i;
	for (i=0; i<6; i++) {
		if (!pset.next(&var, &val)) break;
		sp("         var:"); spl(var);
		args[i]=atoi(var);
	}
	if (i<5) {
		http500();
		server.sendContent("Err: frect(x,y,w,h,r)\n");
		return 1;
	} else {
		sp("     x:"); sp(args[0]);
		sp(" y:"); sp(args[1]);
		sp(" w:"); sp(args[2]);
		sp(" h:"); sp(args[3]);
		sp(" r:"); sp(args[4]);
		sp(" c:"); spl(nxtclr.c);
		if (filled) lcd.fillRoundRect(args[0], args[1], args[2], args[3], args[4], nxtclr.c);
		else        lcd.drawRoundRect(args[0], args[1], args[2], args[3], args[4], nxtclr.c);
	}
	return 0;
}
int cmd_pixel(char *opts) {
	SubParams pset(opts); // for &foo=a=b,c=d,e...
	char *var, *val;
	int x=1, y=1;
	while (pset.next(&var, &val)) {
		if (*var = 'x') x=strtol(val, NULL, 10);
		else if (*var = 'y') y=strtol(val, NULL, 10);
		else {
			http500();
			server.sendContent("px: Invalid opt\n");
			return 1;
		}
	}
	return 0;
}
int cmd_clear(char *opts) {
	SubParams pset(opts); // for &foo=a=b,c=d,e...
	char *var, *val;
	ColorSet clr;
	bool usercolor=false;
	while (pset.next(&var, &val)) {
		if (*var == 'r')      clr.r = !val ? 0 : strtol(val, NULL, 10),
		                      usercolor=true;
		else if (*var == 'g') clr.g = !val ? 0 : strtol(val, NULL, 10),
		                      usercolor=true;
		else if (*var == 'b') clr.b = !val ? 0 : strtol(val, NULL, 10),
		                      usercolor=true;
		else {
			http500();
			server.sendContent("cls: Invalid opt\n");
			return 1;
		}
	}
	if (usercolor) {
		clr.updatec();
		lcd.fillScreen(clr.c);
	} else {
		lcd.fillScreen(nxtclr.c);
	}
	return 0;
}
int cmd_color(ColorSet &clr, char *opts) {
	SubParams pset(opts); // for &foo=a=b,c=d,e...
	char *var, *val;
	clr.r = clr.g = clr.b = 0;
	while (pset.next(&var, &val)) {
		dbsp("Var:"); dbsp(var); dbsp(" val:"); dbspl(val);
		if (*var == 'r')      clr.r = !val ? 0 : strtol(val, NULL, 10);
		else if (*var == 'g') clr.g = !val ? 0 : strtol(val, NULL, 10);
		else if (*var == 'b') clr.b = !val ? 0 : strtol(val, NULL, 10);
		else {
			http500();
			server.sendContent("*col: Invalid opt\n");
			return 1;
		}
	}
	clr.updatec();
	dbsp("r=");
	dbsp(clr.r);
	dbsp(" g=");
	dbsp(clr.g);
	dbsp(" b=");
	dbsp(clr.b);
	dbsp(" c=");
	dbspl(clr.c);
	return 0;
}
void handleNotFound() {
	char temp[SMALL_HTML];
	digitalWrite ( LEDPIN, 1 );
	snprintf(temp, SMALL_HTML,
		"File Not Found\n\n"
		"URI: %s\nMethod: %s\nArguments: %d\n"
		"",
		server.uri().c_str(),
		( server.method() == HTTP_GET ) ? "GET" : "POST",
		server.args());
	String msg = temp;
	for ( uint8_t i = 0; i < server.args(); i++ ) {
		msg += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}
	server.send ( 404, "text/plain", msg );
	digitalWrite ( LEDPIN, 0 );
}
#define SC(d,s) strcpy(d,s)
void initial_display() {
	char val[30];
	cmd_clear(SC(val, "r=0"));
	//if (!cmd_color(nxtclr, SC(val, "r=200,g=300,b=250")) lcd.setTextColor(nxtclr.c);
	cmd_color(nxtclr, SC(val, "r=200,g=300,b=250"));
	cmd_rect(SC(val, "1,1,318,238,20"), false);
	cmd_rect(SC(val, "2,2,316,236,19"), false);
	cmd_color(txtfg, SC(val, "b=200,g=100"));
	cmd_txt(SC(val, "y=20,s=3,t=+Reminders:%0a"));
	cmd_color(txtfg, SC(val, "r=200"));
	cmd_txt(SC(val, "s=5,t=+NAC%0a+E%0a+D%0a"));
	cmd_color(txtfg, SC(val, "b=200,g=100"));
	cmd_txt(SC(val, "s=4,t=+Dance!"));
}

void setup() {
	Serial.begin(115200);
	SPI.setFrequency(ESP_SPI_FREQ);
	lcd.begin();
	lcd.setRotation(1); // landscape (for me)
	touch.begin(lcd.height(), lcd.width());  // Must be done before setting rion
	touch.setRotation(touch.ROT90);
	lcd.fillScreen(0);
	sp("tftx =");
	sp(lcd.width());
	sp(" tfty =");
	spl(lcd.height());
	touch.setCalibration(209, 1759, 1775, 273);
	//touch.setCalibration(234, 1696, 1776, 267);
	//lcd.setFont(BigFont);
	//lcd.setTextColor(0xFFFF, 0); // white on black
	//drawButtons();
	setup_wifi();
	setup_wait_wifi(10); // wait max 10s
	setup_ota();
	sp(F("Connecting to wife..."));
	server.on(F("/"), hand_root );
	server.on(F("/preview.bmp"), hand_img_preview );
	server.on(F("/cs"), hand_cmd_list );
	server.on(F("/lcd"), hand_lcd_status );

	/* This doesn't work yet. Adafruit_GFX generally wants progmem for image data */
	server.on(F("/img"), HTTP_POST,
		[](){ server.send(200); },    // Send status 200 (OK) to tell the client we are ready to receive
		hand_post_img                 // Receive and save the file
	);

	/* server.on(F("/tclr"), hand_tclr ); */
	/* server.on(F("/font"), hand_font ); */
	/* server.on(F("/rec"), cmd_rec ); */
	httpUpdater.setup(&server, update_user, update_pw); // adds /update path for OTA
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( F("HTTP svr started") );
	initial_display();
}
void loop() {
	uint16_t x, y;
	loop_check_wifi(loop_millis);
	loop_ota();
	server.handleClient();

	loop_millis=millis();
	if (touch.isTouching()) {
		touch.getPosition(x, y);
		Serial.println("Touching... x: "+ String(x) + ", y: " + String(y));
	}
	//yield();
}

