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
bool hand_lcd_status(void);

#define BOO(b) (b ? "True" : "False")
