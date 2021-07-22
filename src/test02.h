#ifndef __TEST02_H__
#define __TEST02_H__
#include"globaldef.h"


BEG_NS(test) 

struct CONFIG_PARAM;


/* space */
extern int testOpcSpaceNormal(const CONFIG_PARAM& conf);
extern int testOpcSpaceSlow(const CONFIG_PARAM& conf);

extern int testOpcSpaceEx(const CONFIG_PARAM& conf);
extern int testOpcSpaceExLoop(const CONFIG_PARAM& conf);


/* pinch */
extern int testOpcPinchNormal(const CONFIG_PARAM& conf);
extern int testOpcPinchEx(const CONFIG_PARAM& conf);

/* cd */
extern int testOpcCDNormal(const CONFIG_PARAM& conf);

extern int testOpcCDNormalByFile(const CONFIG_PARAM& conf);

END_NS(test)


#endif

