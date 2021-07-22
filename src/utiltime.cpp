#include<time.h>
#include"utiltime.h"


BEG_NS(util)


/*****************************************************************************
 * Function      : const
 * Description   : 返回以毫秒为单位的时间间隔
 * Input          : None
 * Output        : None
 * Return        : long
 * Others        : 
 * Record
 * 1.Date        : 20210507
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
long long UtilTime::diffTimeMs(const UtilTime& other) const {
    long long ret = 0L;

    ret = (m_sec - other.m_sec) * 1000 + (m_nsec - other.m_nsec) / 1000000;
    return ret;
}

void UtilTime::mark() {
    struct timespec now = {0, 0};

    clock_gettime(CLOCK_MONOTONIC, &now);
    m_sec = now.tv_sec;
    m_nsec = now.tv_nsec;
}

END_NS(util)

