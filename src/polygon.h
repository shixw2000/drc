/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : polygon.h
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
#ifndef __POLYGON_H__
#define __POLYGON_H__

#include"globaldef.h"
#include"box.h"
#include"coordtype.h"
#include"polydelegate.h"
#include"writer.h"


BEG_NS(geo)

template<typename T>
class Polygon {
public:
    Polygon() : m_id(0), m_delegate() {}
    Polygon(int id, const Contour<T>* contour) : m_id(id) {
        m_delegate.set(contour);
        m_box = calcArea();
    }

    ~Polygon() {}

    int id() const {
        return m_id;
    }
    
    int vertices() const {
        return m_delegate.vertices();
    }
    
    const Box<T>& box() const {
        return m_box;
    }

    void set(int id, const Contour<T>* contour) {
        m_id = id;
        m_delegate.set(contour);
        
        m_box = calcArea();
    }

    void normalize() {
        int old_size = 0;
        int new_size = 0;

        old_size = vertices();
        
        m_delegate.normalize();
        m_box = calcArea();

        new_size = vertices(); 
        
        LOG_INFO("polygon_normalize| id=%d| old_size=%d| new_size=%d|",
            id(), old_size, new_size);
    }

    void extractLine() {
        int old_size = 0;
        int new_size = 0;

        old_size = vertices();
        
        m_delegate.extractLine();
        m_box = calcArea();

        new_size = vertices(); 
        
        LOG_INFO("polygon_extractLine| id=%d| old_size=%d| new_size=%d|",
            id(), old_size, new_size);
    }
    

    const Point<T>& get(int index) const {
        return m_delegate.get(index);
    }

    const Point<T>* getP(int index) const {
        return m_delegate.getP(index);
    }

    void disp() const;

private: 
    Box<T> calcArea() const; 

private:
    /* forbidden to copy */
    Polygon(const Polygon<T>& );
    Polygon& operator=(const Polygon<T>& ); 
    
private:
    int m_id;
    PolygonDelegate<T> m_delegate;
    Box<T> m_box; 
};

template<typename T>
Box<T> Polygon<T>::calcArea() const {
    Box<T> box;

    if (!m_delegate.isEmpty()) {
        for (int i=0; i<vertices(); ++i) {
            box += get(i);
        }
    }

    return box;
}

template<typename T>
void Polygon<T>::disp() const {
    if (0 < vertices()) {
        FILE_LOG("%d %d", id(), 0);
        for (int i=0; i<vertices(); ++i) {
            const Point<T>&  p = get(i);
            
            FILE_LOG(" %.2lf %.2lf", p.x(), p.y());
        }
        /* add front point to a closure area */
        FILE_LOG(" %.2lf %.2lf\n", get(0).x(), get(0).y());
    }
        
    LOG_INFO("Polygon| id=%d| point_cnt=%d| widht=%.2lf| height=%.2lf| p1=<%.2lf, %.2lf>| p2=<%.2lf, %.2lf>|", 
        id(),
        vertices(),
        m_box.width(), m_box.height(),
        m_box.p1().x(), m_box.p1().y(),
        m_box.p2().x(), m_box.p2().y());
}


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Polygon< NS(base)::Coord > basePolygon;

END_NS(base)

#endif

