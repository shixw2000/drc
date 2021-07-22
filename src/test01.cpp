#include"test01.h"
#include"edgefilter.h"
#include"libexport.h"
#include"test02.h"


BEG_NS(test) 

typedef NS(base)::baseTraits traits_type;
typedef traits_type::coord_type coord_type;
typedef traits_type::area_type area_type;
typedef traits_type::distance_type distance_type;

typedef NS(alg)::EdgeRelationFilter EdgeRelationFilter;
typedef EdgeRelationFilter::point_type point_type;
typedef EdgeRelationFilter::vectors_type vectors_type;
typedef EdgeRelationFilter::edge_type edge_type;
typedef EdgeRelationFilter::edgepair_type edgepair_type; 

TestSuits::TestSuits() {
}

int TestSuits::test(const CONFIG_PARAM& conf) {
    int ret = 0;
    
    switch (conf.m_opt) {
    case 0:
        test01(conf);
        break;

    case 1:
        ret = testOpcSpaceExLoop(conf);
        break;

    case 2:
        ret = testOpcSpaceEx(conf);
        break;

    case 3:
        ret = testOpcSpaceNormal(conf);
        break;

    case 4:
        ret = testOpcSpaceSlow(conf);
        break;

    case 5:
        ret = testOpcPinchNormal(conf);
        break;

    case 15:
        ret = testOpcPinchEx(conf);
        break;

    case 6:
        ret = testOpcCDNormal(conf);
        break;

    case 7:
        ret = testOpcCDNormalByFile(conf);
        break;

    default:
        ret = -1;
        break;
    } 

    /* process ok */
    if (0 <= ret) {
        ret = 0;
    }
    
    return ret;
}

void TestSuits::test01(const CONFIG_PARAM& conf) {
    bool bIn = false;
    distance_type distance = 0;
    distance_type min_distance = 0;
    coord_type ex1, ey1, ex2, ey2;
    coord_type ox1, oy1, ox2, oy2;
    
    LOG_INFO("enter a distance and two edges:");
    scanf("%lf %lf %lf %lf %lf %lf %lf %lf %lf", 
        &distance,
        &ex1, &ey1, &ex2, &ey2,
        &ox1, &oy1, &ox2, &oy2);
    
    EdgeRelationFilter filter(distance);

    bIn = filter.check(edge_type(ex1, ey1, ex2, ey2), edge_type(ox1, oy1, ox2, oy2), &min_distance);
    LOG_INFO("filter_check| ret=%d| distance=%.2lf:%.2lf|",
        bIn, distance, min_distance);

    return;
}


END_NS(test)

