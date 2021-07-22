#ifndef __UTILTIME_H__
#define __UTILTIME_H__
#include"globaldef.h"


BEG_NS(util)

class OPC_API UtilTime {
public:
    UtilTime() : m_sec(0), m_nsec(0) {}

    long long diffTimeMs(const UtilTime& other) const;
    
    void mark();

private:
    long long m_sec;
    long long m_nsec;
};

END_NS(util)

#endif

