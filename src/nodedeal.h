#ifndef __NODEDEAL_H__
#define __NODEDEAL_H__
#include"globaldef.h"
#include"coordtype.h"


BEG_NS(geo) 

template<typename T> class Polygon;

template<typename T> class Segment;
template<typename T> class Segments;

template<typename T> class Area;
template<typename T> class Region;

template<typename T> class Dimension;

template<typename T> class Box;

template<typename T> class Point;

template<typename T> class Edge;

template<typename T> class AreaEnum;
template<typename T> class AreaEnumEx;
template<typename T> class AreaEnumEx2;

template<typename T> class SegmentsEnum;
template<typename T> class SegmentsVisitor; 

END_NS(geo)

    
BEG_NS(alg) 

template<typename T> class Node;
template<typename T> class AreaHandler;
template<typename T> class AreaHandlerEx;
template<typename T> class SegmentEdgeIterator;

template<typename T>
class NodeDeal {
public:
    typedef T coord_type;
    typedef NS(base)::coord_traits<T> typeTraits;
    typedef typename typeTraits::distance_type distance_type;
    typedef typename typeTraits::area_type area_type;
    typedef Node<T> typeNode;
    typedef typeNode* typeNodePtr;
    typedef NS(geo)::Polygon<T> typePolygon;
    typedef NS(geo)::Segment<T> typeSegment;
    typedef NS(geo)::Segments<T> typeSegments;
    typedef NS(geo)::Area<T> typeArea; 
    typedef NS(geo)::Region<T> typeRegion; 
    typedef NS(geo)::Dimension<T> typeDimension;
    typedef NS(geo)::Box<T> typeBox;
    typedef NS(geo)::Point<T> typePoint;
    typedef NS(geo)::Edge<T> typeEdge;

    typedef NS(geo)::AreaEnum<T> typeAreaEnum;
    typedef NS(geo)::AreaEnumEx<T> typeAreaEnumEx;
    typedef NS(geo)::AreaEnumEx2<T> typeAreaEnumEx2;
    
    typedef SegmentEdgeIterator<T> typeSegmentEdgeIterator;

    typedef AreaHandler<T> typeAreaHandler;
    typedef AreaHandlerEx<T> typeAreaHandlerEx;
    
    virtual ~NodeDeal() {}
    virtual void process(int type, typeNode* node, const typePolygon* polygon, typeSegment* segment) = 0;
    
    virtual void processEx(int type, typeNode* node, const typePolygon* polygon1, typeSegment* segment1,
        const typePolygon* polygon2, typeSegment* segment2) {}

protected:
    void box2Dim(typeDimension& dim, const typeBox& box);
    
    typeNode* createRootNode(const typeDimension& dim, const typePolygon* polygons, int size);
};


typedef NodeDeal< NS(base)::Coord > baseNodeDeal;

END_NS(alg) 

#endif

