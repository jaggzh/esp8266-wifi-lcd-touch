#define SUBPARAM_SERIAL_DEBUG
/* For splitting query parameters into additional params
 * Example:
 *   char *str = "txt=s=1,t=Hello,,world";
 *   char *str = "a=b,c=d,e";
 *   SubParams pset(str);
 *   while (pset.next(&var, &val)) {
 *      // 'var' and 'val are now a NUL-terminated strings (char *)
 *      //   1st call: var=="a", b=="b"
 *      //   3rd call: var=="e", b==NUL
 *   }
 */
#define SUBPARAM_SEP               ','
#define SUBPARAM_SEP_INTERNAL      255
#define SUBPARAM_SEP_INTERNAL_STR  "\xFF"

#include "printutils.h" // dbsp() dbspl()
class SubParams {
	private:
		char *_val;
		char *_arg;
		char *_rest;
	public:
		SubParams(char *val) {
			uint16_t len=-1; // only check strlen if needed
			_val = val;
			_rest = val;
			dbsp("esp-subparams.h -> New pset with val: ");
			dbspl(val);
			for (char *s=val; *s; s++) {
				if (*s == SUBPARAM_SEP) {         // txt=hi,,there,2ndopt
					if (s[1] != SUBPARAM_SEP)     //     hi,there{255}2ndopt
						*s = SUBPARAM_SEP_INTERNAL;
					else {
						if (len == -1) len = strlen(s); // only retrieve once
						memmove(s, s+1, len); // len-1 at s+1, but include \0
						len--;                // we just shortened str
					}
				}
				if (len>0) len--;             // remaining length
			}
			dbsp("esp-subparams.h -> Parsed commas: ");
			dbspl(val);
		};
		int next(char **var, char **val) {
			dbspl("esp-subparams.h -> next()");
			dbsp("    _rest:"); dbspl(_rest);
			if (!(_arg=strtok_r(_rest, SUBPARAM_SEP_INTERNAL_STR, &_rest))) {
				dbspl("  Done with params");
				return 0;
			}
			dbsp("     _arg: "); dbsp(_arg); dbsp(", ");
			char *subvar=_arg;
			char *subval=strchr(_arg, '=');
			if (!subval) {
				dbsp("(plain) ");
				dbspl(subvar);
				*var = subvar;
				*val = NULL;
			} else {
				*(subval++) = 0;
				dbsp("(w/val) ");
				dbsp(subvar);
				dbsp(" = ");
				dbspl(subval);
				*var = subvar;
				*val = subval;
			}
			return 1;
		}
};
