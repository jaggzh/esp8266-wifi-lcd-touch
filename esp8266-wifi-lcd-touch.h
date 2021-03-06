#include <Adafruit_ILI9341esp_read.h>
#include "colorstuff.h"
#include "esp-subparams.h"

#define MAX_MS_BEFORE_RECONNECT 15000

int cmd_txt(char *val);                 // txt=hello
int cmd_font(char *name);               // fnt=[sbn]
int cmd_color(ColorSet &clr, char *opts); //r=255,g=255,b=0
int cmd_clear(char *opts);              // [r=#,g=#,b=#] (default white!)
int cmd_txt(char *opts);                // t=txt,[x=#,][y=#]
int cmd_pixel(char *opts);              // x=#,y=#
int cmd_rect(char *opts, bool filled);  // [x,y,w,h,r]=#s
int cmd_off(char *val);
int cmd_pixel(char *opts);
int cmd_hline(char *opts);
int mqtt_pixel(String opts);
void hand_lcd_status(void);
void main_debug(String s); // output a string to lcd
void lcd_notify_wifi_connect();
uint16_t rowstr_to_colors(uint16_t *row, char *str, bool bgr=false);
void set_text_color_honoring_trans(uint16_t fg);

#define BOO(b) (b ? "True" : "False")

#ifndef __MAIN_INO__
extern Adafruit_ILI9341 lcd;
extern ColorSet txtfg;
extern ColorSet txtbg;
extern ColorSet nxtclr;
extern uint8_t txtsize;
extern uint16_t lcd_width, lcd_height;
#endif
