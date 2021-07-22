/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : point.h
 * Author        : shixw
 * Date          : 2021-04-19
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2021-04-19
 *   Author      : shixw
 *   Modification: Created file

*************************************************************************************************************/
#ifndef __POINT_H__
#define __POINT_H__

#include"globaldef.h"
#include"coordtype.h"


BEG_NS(geo)

template<typename T>
class OPC_API Point {
public:
    typedef NS(base)::coord_traits<T> coord_traits;
    typedef typename coord_traits::distance_type distance_type; 
    typedef typename coord_traits::area_type area_type; 

    Point() : m_x(0), m_y(0) {}
    Point(T x, T y) : m_x(x), m_y(y) {}

    template<typename D>
    Point (D x, D y) : m_x (coord_traits::rounded(x)), m_y (coord_traits::rounded (y)) {}

    template<typename D>
    Point(const Point<D>& other) : m_x(coord_traits::rounded(other.m_x)), m_y(coord_traits::rounded(other.m_y)) {} 
  
    bool operator==(const Point<T>& p) const {
        return (m_x == p.m_x) && (m_y == p.m_y);
    }

    template<typename D>
    Point<T>& operator*=(D s) {
        m_x = coord_traits::rounded(s * m_x);
        m_y = coord_traits::rounded(s * m_y);
        
        return *this;
    }
    
    T x()const {
        return m_x;
    }
    
    T y()const {
        return m_y;
    }
    
    void set_x(T x) {
        m_x = x;
    }
    
    void set_y(T y) {
        m_y = y;
    }

    int locate(const Point<T>& po) const;
    const Point<T>& add(const Point<T>& po);
    const Point<T>& sub(const Point<T>& po);
    const Point<T>& mult(const Point<T>& p);

    Point<T> addP(const Point<T>& po) const;
    Point<T> subP(const Point<T>& po) const;
    Point<T> multP(const Point<T>& p) const;
    
    const Point<T>& move(T x, T y);
    Point<T> moveP(T x, T y) const;

    void disp() const;    
    
private:
    T m_x;
    T m_y;
};


/*****************************************************************************
 * Function      : Point.chkAreac
 * Description   : 比较并返回po所在的相对象限
 * Input         : const Point<T>& po  
 * Output        : None
 * Return        : int
 * Others        : 
 * Record
 * 1.Date        : 20210420
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
int Point<T>::locate(const Point<T>& po) const {
    int isRight = (int)NS(base)::coord_traits<T>::less(this->m_x, po.m_x);
    int isUp = (int)NS(base)::coord_traits<T>::less(this->m_y, po.m_y);

    return ( (isUp << 1) + isRight );
}

template<typename T>
const Point<T>& Point<T>::add(const Point<T>& po) {
    m_x += po.x();
    m_y += po.y();
    return *this;
}

template<typename T>
const Point<T>& Point<T>::sub(const Point<T>& po) {
    m_x -= po.x();
    m_y -= po.y();
    return *this;
}

template<typename T>
const Point<T>& Point<T>::mult(const Point<T>& p) {
    m_x *= p.x();
    m_y *= p.y();

    return *this;
}

template<typename T>
Point<T> Point<T>::addP(const Point<T>& p) const {
    Point<T> po = *this;

    po.add(p);
    return po;
}

template<typename T>
Point<T> Point<T>::subP(const Point<T>& p) const {
    Point<T> po = *this;

    po.sub(p);
    return po;
}

template<typename T>
Point<T> Point<T>::multP(const Point<T>& p) const {
    Point<T> po = *this;

    po.mult(p);
    return po;
}

template<typename T>
const Point<T>& Point<T>::move(T x, T y) {
    m_x += x;
    m_y += y;
    return *this;
}

template<typename T>
Point<T> Point<T>::moveP(T x, T y) const {
    Point<T> po = *this;

    po.move(x, y);
    return po;
}

template<typename T>
void Point<T>::disp() const {
    LOG_INFO("Point| x=%.2lf| y=%.2lf|", m_x, m_y);
}

template<typename T>
typename Point<T>::area_type OPC_API sq_length(const Point<T>& p1, const Point<T>& p2) {
    return Point<T>::coord_traits::sq_length(p1.x(), p1.y(), p2.x(), p2.y());
}

template<typename T>
double OPC_API length1(const Point<T>& p1, const Point<T>& p2) {
    return Point<T>::coord_traits::length1( p1.x(), p1.y(), p2.x(), p2.y() );
}

template<typename T>
typename Point<T>::distance_type OPC_API max_xy_length(const Point<T>& p1, const Point<T>& p2) {
    T maxlenx = 0;
    T maxleny = 0;

    maxlenx = (p1.x() > p2.x() ? (p1.x() - p2.x()) : (p2.x() - p1.x()));
    maxleny = (p1.y() > p2.y() ? (p1.y() - p2.y()) : (p2.y() - p1.y()));
    
    return Point<T>::coord_traits::rounded_distance(maxlenx < maxleny ? maxleny : maxlenx);
}

/* p2p1 is colinear with p2p3 */
template<typename T>
bool isColinear(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3, bool remove_reflect) {
    if (NS(base)::isParallel(p1.x(), p1.y(), p3.x(), p3.y(), p2.x(), p2.y())) {
        return remove_reflect || 0 >= NS(base)::coord_traits<T>::sprod_sign(
            p1.x(), p1.y(), p3.x(), p3.y(), p2.x(), p2.y());
    } else {
        return false;
    }
}

/* p2p1 is orthogonal with p2p3*/
template<typename T>
bool isOrthogonal(const Point<T>& p1, const Point<T>& p2, const Point<T>& p3) {
    return NS(base)::isOrthogonal(p1.x(), p1.y(), p3.x(), p3.y(), p2.x(), p2.y());
}

template<typename T>
bool equal(const Point<T>& p1, const Point<T>& p2) {
    return NS(base)::coord_traits<T>::equal(p1.x(), p2.x()) && 
        NS(base)::coord_traits<T>::equal(p1.y(), p2.y());
}

template<typename T>
bool isOverlap(const Point<T>& p1, const Point<T>& p2) {
    static const int Overlap_prec = 2;
    T dx = p1.x() - p2.x();
    T dy = p1.y() - p2.y();
    T delta = Overlap_prec;

    if (dx < 0) {
        dx = -dx;
    }

    if (dy < 0) {
        dy = -dy;
    }
    
    return dx <= delta && dy <= delta;
}

/* first compare x, if equal, then compare y */
template<typename T>
bool less(const Point<T>& p1, const Point<T>& p2) {
    if (p1.x() < p2.x() || (p1.x() == p2.x() && p1.y() < p2.y())) {
        return true;
    } else {
        return false;
    }
}


END_NS(geo)

BEG_NS(base)

typedef NS(geo)::Point< NS(base)::Coord > basePoint;
typedef NS(geo)::Point< NS(base)::DCoord > baseDPoint;

END_NS(base)

#endif

