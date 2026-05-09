#include "SemanticTable.h"

//                              SUM  SUB  MUL  DIV  REL  MOD  POT  ROO  AND  OR
int const SemanticTable::expTable[5][5][10] = {
/*INT*/ {
    { INT, INT, INT, FLO, BOO, INT, INT, FLO, ERR, ERR },
    { FLO, FLO, FLO, FLO, BOO, ERR, FLO, FLO, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR }
},
/*FLO*/ {
    { FLO, FLO, FLO, FLO, BOO, ERR, FLO, FLO, ERR, ERR },
    { FLO, FLO, FLO, FLO, BOO, ERR, FLO, FLO, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR }
},
/*CHA*/ {
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { STR, ERR, ERR, ERR, BOO, ERR, ERR, ERR, ERR, ERR },
    { STR, ERR, ERR, ERR, BOO, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR }
},
/*STR*/ {
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { STR, ERR, ERR, ERR, BOO, ERR, ERR, ERR, ERR, ERR },
    { STR, ERR, ERR, ERR, BOO, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR }
},
/*BOO*/ {
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR },
    { ERR, ERR, ERR, ERR, BOO, ERR, ERR, ERR, BOO, BOO }
}
};

//                     INT  FLO  CHA  STR  BOO
int const SemanticTable::atribTable[5][5] = {
/*INT*/ { OK_, WAR, ERR, ERR, ERR },
/*FLO*/ { OK_, OK_, ERR, ERR, ERR },
/*CHA*/ { ERR, ERR, OK_, ERR, ERR },
/*STR*/ { ERR, ERR, OK_, OK_, ERR },
/*BOO*/ { ERR, ERR, ERR, ERR, OK_ }
};
