#ifndef __OPCPINCHRULE_H__
#define __OPCPINCHRULE_H__
#include<list>
#include<vector>
#include<map>
#include"globaldef.h"
#include"coordtype.h"
#include"polygon.h"
#include"box.h"
#include"edge.h"
#include"dimension.h"
#include"datanode.h"
#include"nodedeal.h"


BEG_NS(alg) 


struct StuPinchItem {
    const baseNodeDeal::typePolygon* m_polygon;
    baseNodeDeal::typeEdge m_edg1;
    baseNodeDeal::typeEdge m_edg2;
    baseNodeDeal::distance_type m_distance;
};

typedef STD_NS(list)<StuPinchItem> typePinchResult;
typedef STD_NS(map)<int, baseNodeDeal::distance_type> typePolyDistance;
typedef typePolyDistance::const_iterator typePolyDistanceItrConst;

struct StuCDItem {
    const baseNodeDeal::typePolygon* m_polygon;
    baseNodeDeal::typePoint m_point1;
    baseNodeDeal::typePoint m_point2;
    baseNodeDeal::distance_type m_length;
};

typedef STD_NS(list)<StuCDItem> typeCDItems;

struct StuRetCD {
    baseNodeDeal::typeEdge m_edge;
    typeCDItems m_cds;
};

typedef STD_NS(list)<StuRetCD> typeCDResult;

union StuRets {
    void* m_ret;
    typePinchResult* m_retPinch;
    typeCDResult* m_retCD;
};

union StuCond {
    const void* m_cond;
    const typePolyDistance* m_polyDistance;
    const baseNodeDeal::distance_type* m_distance;
};

class OpcPinchStrategy : public baseNodeDeal { 
    typedef STD_NS(vector)<typePoint> typeVecPoints;
    
public:    
    OpcPinchStrategy();
    virtual ~OpcPinchStrategy();

    void finish();
    
    int procPinchRule(const typeBox* area, const typePolygon* polygons, int size, 
        distance_type distance, typePinchResult& result); 

    int procPinchRuleEx(const typeBox* area, const typePolygon* polygons, int size, 
        const typePolyDistance& polyDistance, typePinchResult& result);

    int procCDRule(const typeBox* area, const typePolygon* polygons, int size, 
        const STD_NS(vector)<typeEdge>& edges, typeCDResult& result);

    virtual void process(int type, typeNodePtr node, const typePolygon* polygon, typeSegment* segment);

protected:
    int dealNode(int type, typeNodePtr node);

    void chkPolygonWholePinch(distance_type minDistance, typeNodePtr node, const typePolygon* polygon, 
        typeSegment* segment, typePinchResult* result);

    void calcPolygonWholeCD(typeNodePtr node, const typePolygon* polygon, typeSegment* segment, typeCDResult* result);
    
    void calcPolygonCD(const typeEdge& edge, typeNodePtr node, const typePolygon* polygon, 
        typeSegment* segment, typeCDItems& result);

    void unique(typeVecPoints& pts);
    
private:
    typeNodePtr m_root;
    typeBox m_area;
    typeDimension m_dim;
    StuRets m_result;
    StuCond m_conditon;
}; 



END_NS(alg)


#endif

