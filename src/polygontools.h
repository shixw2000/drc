#ifndef __POLYGONTOOLS_H__
#define __POLYGONTOOLS_H__
#include"globaldef.h"


BEG_NS(geo)


template<typename T> class Box<T>;

/* intersect */
template<typename T>
bool intersect(const Box<T>& box1, const Box<T>& box2);

template<typename T>
bool intersect(const Polygon<T>& poly, const Edge<T>& edge);

template<typename T>
bool intersect(const Polygon<T>& poly, const Box<T>& box);

template<typename  T>
bool intersect(const Polygon<T>& poly1, const Polygon<T>& poly2);



/* inside */
template<typename T>
bool insidePolyTest(const Polygon<T>& poly, const Point<T>& point);

template<typename T>
bool is_colinear(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3) {
    return 0 == NS(base)::coord_traits<T>::vprod_sign(p1.x(), p1.y(), p3.x(), p3.y(), p2.x(), p2.y()); 
}

template<typename T>
class PolygonEdgeIterator {
public:
    PolygonEdgeIterator();
};



END_NS(geo)

#endif

