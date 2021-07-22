/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : contour.h
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
#ifndef __CONTOUR_H__
#define __CONTOUR_H__

#include"globaldef.h"
#include"point.h"
#include"box.h"


BEG_NS(geo)

template<typename T>
class Contour {
public:
    Contour() : m_points(NULL), m_size(0) {}
    
    Contour(Point<T>* points, int size) : m_points(points), m_size(size) {}

    ~Contour() {
        if (NULL != m_points) {
            delete[] m_points;
        }
    }

    int size() const {
        return m_size;
    }

    void set(Point<T>* points, int size) {
        m_points = points;
        m_size = size;
    }

    const Point<T>& get(int index) const {
        return m_points[index];
    }

    Point<T>& get(int index) {
        return m_points[index];
    }

    const Point<T>* getP(int index) const {
        return &m_points[index];
    }

private:
    Contour(const Contour<T>&);
    Contour<T>& operator=(const Contour<T>&);
    
private:
    Point<T>* m_points;
    int m_size;
};


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Contour< NS(base)::Coord > baseContour;

END_NS(base)

#endif

