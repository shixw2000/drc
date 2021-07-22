#ifndef __OPCSPACERULE_H__
#define __OPCSPACERULE_H__
#include<list>
#include<set>
#include"globaldef.h"
#include"coordtype.h"
#include"polygon.h"
#include"box.h"
#include"edge.h"
#include"dimension.h"
#include"datanode.h"
#include"nodedeal.h"


BEG_NS(alg) 

/* 注意：排列的时候，必须使polygon1.id < polygon2.id，否则会有重复的点集 */
struct StuSpaceItem {
    const typename baseNodeDeal::typePolygon* m_polygon1; //key1
    const typename baseNodeDeal::typePoint* m_p1;
    int m_index1;
    const typename baseNodeDeal::typePolygon* m_polygon2; //key2
    const typename baseNodeDeal::typePoint* m_p2;
    int m_index2;
    typename baseNodeDeal::area_type m_sq_distance; //square distance
};

struct ItemSpaceCmp {
    bool operator()(const StuSpaceItem& item1, const StuSpaceItem& item2) const {
        return (item1.m_polygon1->id() < item2.m_polygon1->id())
            || ((item1.m_polygon1->id() == item2.m_polygon1->id())
                && item1.m_polygon2->id() < item2.m_polygon2->id());
    }
};

typedef StuSpaceItem typeItemResult;
typedef STD_NS(set)<typeItemResult, ItemSpaceCmp> typeChkResult;


class OpcSpaceStrategy : public baseNodeDeal { 
    typedef NS(geo)::Border<coord_type> typeBorder;
    typedef NS(geo)::Borders<coord_type> typeBorders;
    typedef NS(geo)::Boundary<coord_type> typeBoundary; 
    
public:
    virtual void process(int type, typeNode* node, const typePolygon* polygon, typeSegment* segment);
    
    virtual void processEx(int type, typeNode* node, const typePolygon* polygon1, typeSegment* segment1,
        const typePolygon* polygon2, typeSegment* segment2);

    void buildTreeNode(typeNodePtr node);
    void chkLeafNode(typeNodePtr node);
    void chkInterNode(typeNodePtr node); 

private:
    void partitionSegment(typeNodePtr node, typeSegment& segment, const typePolygon* polygon);
    
    void calcSegmentDist(const typeSegment& segment_src, const typePolygon* polygon_src, 
        const typeSegment& segment_dst, const typePolygon* polygon_dst, typeChkResult& ret);

private:
    area_type m_sq_distance;
    distance_type m_distance;
    typeChkResult* m_ret;
    typeBoundary* m_boundary;
};


END_NS(alg)

#endif

