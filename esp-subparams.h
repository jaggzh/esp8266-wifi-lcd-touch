UnDo
			#ifdef SUBPARAM_SERIAL_DEBUG
				spsp("  Command='");
				spsp(val);
				spspl("' (txt)");
				#endif5
			spsp("    var:"); spsp(5
			spsp("    rest:"); spsp(5
-			spsp("    Arg: "); spsp(_arg); spsp(", ");5
.			spsp("     Arg: "); spsp(_arg); spsp(", ");5
.			spsp("     arg: "); spsp(_arg); spsp(", ");5
#			spsp("    rest:"); spspl(_rest);5
.			spsp("     _rg: "); spsp(_arg); spsp(", ");5
;#define spsp(a)  Serial.print(a)    // Just for convenience
"#define spspl(a) Serial.println(a)5
$			spsp("    _rest:"); spspl(_rest);5
class SubParams {5
				spspl(subval);
				spsp(" = ");
				spsp(subvar);
				spsp("(w/val) ");
				spspl(subvar);
				spsp("(plain) ");
/			spsp("     _arg: "); spsp(_arg); spsp(", ");5
			sp("next()");5
#			sp("esp-subparams.h -> next()");5
				sp(" = ");
				sp(subvar);
				sp("(w/val) ");
				sp("(plain) ");
)			sp("     _arg: "); sp(_arg); sp(", ");
 			sp("    _rest:"); spl(_rest);
%#include "printutils.h" // sp() spl()5
				spl(subval);
				spl(subvar);
"			dbsp("    _rest:"); spl(_rest);
$			spl("esp-subparams.h -> next()");
'#include "printutils.h" // dbsp() spl()5
				
+			dbsp("     _arg: "); sp(_arg); sp(", ");5
&			dbspl("esp-subparams.h -> next()");5
