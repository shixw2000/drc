#ifndef __TEST01_H__
#define __TEST01_H__

#include"globaldef.h"


BEG_NS(test) 

    
struct CONFIG_PARAM {
    int m_opt;
    bool m_isPrnt;
    int m_minPolygon;
    int m_loop;
    double m_minWidth;
    const char* m_name;
    double m_distance;
    double m_edges[10][4];
    int m_edge_cnt;
    const char* m_cdfile;
};

class TestSuits {
public:
    TestSuits();

    int test(const CONFIG_PARAM& conf);

private:
    void test01(const CONFIG_PARAM& conf);
    
};


END_NS(test)

#endif

