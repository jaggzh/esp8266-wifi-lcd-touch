#define SUBPARAM_SERIAL_DEBUG
/* For splitting query parameters into additional params
 * Example:
 *   char *str="a=b,c=d,e";
 *   SubParams pset(str);
 *   while (pset.next(&var, &val)) {
 *      // 'var' and 'val are now a NUL-terminated strings (char *)
 *      //   1st call: var=="a", b=="b"
 *      //   3rd call: var=="e", b==NUL
 *   }
 */
#include "printutils.h" // dbsp() dbspl()
class SubParams {
	private:
		char *_val;
		char *_arg;
		char *_rest;
	public:
		SubParams(char *val) {
			_val = val;
			_rest = val;
			dbsp("esp-subparams.h -> New pset with val: ");
			dbspl(val);
		};
		int next(char **var, char **val) {
			dbspl("esp-subparams.h -> next()");
			dbsp("    _rest:"); dbspl(_rest);
			if (!(_arg=strtok_r(_rest, ",", &_rest))) {
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
