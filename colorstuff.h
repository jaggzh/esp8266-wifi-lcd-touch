#ifndef COLORSTUFF_H
#define COLORSTUFF_H
#include "printutils.h"
uint16_t rgb24to565(uint8_t r, uint8_t g, uint8_t b);
void rgb24from565(uint8_t *rp, uint8_t *gp, uint8_t *bp, uint16_t c);

class ColorSet {
	public:
		uint8_t r, g, b;
		uint16_t c;
		ColorSet() { r=g=b=c=0; }
		void updatec() {
			dbprintf("updatec(): r=%d, g=%d, b=%d\n", this->r, this->g, this->b);
			c = rgb24to565(r, g, b);
			dbprintf("  -> c=%d\n", c);
		}
		ColorSet(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {
			updatec();
		}
};
#endif // COLORSTUFF_H
