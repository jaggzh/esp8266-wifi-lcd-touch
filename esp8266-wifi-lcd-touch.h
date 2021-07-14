#include "colorstuff.h"
#include "esp-subparams.h"

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

#define BOO(b) (b ? "True" : "False")
