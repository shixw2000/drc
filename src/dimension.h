/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : dimension.h
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
#ifndef __DIMENSION_H__
#define __DIMENSION_H__
#include"globaldef.h"
#include"point.h"


BEG_NS(geo)

template<typename T>
class Dimension {
public:
    Dimension();
    Dimension(const Point<T>& p, T width);

    T width() const;
    const Point<T>& origin() const;
    
    void setOrig(const Point<T>& p);
    void setWidth(T width);

    void disp() const;

private:
   Point<T> m_origin;  // the origin in coordinate
   T m_width;   // the half width of the whole area
};

template<typename T>
Dimension<T>::Dimension() {
   m_width = (T)0;
}

template<typename T>
Dimension<T>::Dimension(const Point<T>& p, T width) 
    : m_origin(p), m_width(width) {
}

template<typename T>
T Dimension<T>::width() const {
    return m_width;
}

template<typename T>
const Point<T>& Dimension<T>::origin() const {
    return m_origin;
}

template<typename T>
void Dimension<T>::setOrig(const Point<T>& p) {
    m_origin = p;
}

template<typename T>
void Dimension<T>::setWidth(T width) {
    m_width = width; 
}

template<typename T>
void Dimension<T>::disp() const {
    LOG_INFO("Dimension| widht=%.2lf| origin=<%.2lf, %.2lf>|", 
        width(), 
        m_origin.x(), m_origin.y());
}

END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Dimension< NS(base)::Coord > baseDimension;

END_NS(base)
    
#endif

