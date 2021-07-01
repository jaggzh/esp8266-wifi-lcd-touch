#include "non-arduino.h"
#include <stdint.h>
#include "colorstuff.h"
#include "printutils.h"
uint16_t rgb24to565(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t c;
	dbsp("rgb24to565(): r=");
	dbsp((int)r);
	dbsp(" g=");
	dbsp((int)g);
	dbsp(" b=");
	dbspl((int)b);
	c = ((((uint16_t)r) / 8) << 11) |
		((((uint16_t)g) / 4) << 5) |
		(((uint16_t)b) / 8);
	dbsp("565 c = ");
	dbspl((int)c);
	return c;
}
void rgb24from565(uint8_t *rp, uint8_t *gp, uint8_t *bp, uint16_t c) {
	unsigned r = (c & 0xF800) >> 11;
	unsigned g = (c & 0x07E0) >> 5;
	unsigned b = c & 0x001F;
	r = (r * 255) / 31;
	g = (g * 255) / 63;
	b = (b * 255) / 31;
	if (rp) *rp = r;
	if (gp) *gp = g;
	if (bp) *bp = b;
}
