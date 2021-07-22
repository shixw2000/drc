/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : box.h
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
#ifndef __BOX_H__
#define __BOX_H__

#include"globaldef.h"
#include"point.h"


BEG_NS(geo)

template<typename T>
class OPC_API Box {
public:
    Box() : m_p1(1,1), m_p2(-1, -1) {}
    
    Box(T x1, T y1, T x2, T y2)
        : m_p1(x1 < x2 ? x1 : x2, y1 < y2 ? y1 : y2),
        m_p2(x2 > x1 ? x2 : x1, y2 > y1 ? y2 : y1) {}
        
    Box(const Point<T>& p1, const Point<T>& p2) 
        : m_p1(p1.x() < p2.x() ? p1.x() : p2.x(), p1.y() < p2.y() ? p1.y() : p2.y()), 
        m_p2(p2.x() > p1.x() ? p2.x() : p1.x(), p2.y() > p1.y() ? p2.y() : p1.y()) {}
    
    const Point<T>& p1() const {
        return m_p1;
    }
    
    const Point<T>& p2() const {
        return m_p2;
    }

    void set_p1(const Point<T>& p1) {
        m_p1 = p1;
    }
    
    void set_p2(const Point<T>& p2) {
        m_p2 = p2;
    }

    T x1() const {
        return m_p1.x();
    }

    T y1() const {
        return m_p1.y();
    }
    
    T x2() const {
        return m_p2.x();
    }

    T y2() const {
        return m_p2.y();
    }

    void reset() {
        m_p1 = Point<T>(1,1);
        m_p2 = Point<T>(-1,-1);
    }

    T width() const {
        return m_p2.x() - m_p1.x();
    }

    T height() const {
        return m_p2.y() - m_p1.y();
    }

    bool isEmpty() const {
        return m_p2.x() < m_p1.x() || m_p2.y() < m_p1.y();
    }

    Box<T>& operator+=(const Point<T>& p);

    Box<T>& operator+=(const Box<T>& other);

    bool contain(const Point<T>& p) const;

    bool inside(const Box<T>& other) const;
    
    bool touches (const Box<T>& other) const;

    bool overlaps (const Box<T>& other) const;


    Box<T>& enlarge(T l);
    Box<T> enlarged(T l) const;

    void disp() const;

private:
    Point<T> m_p1;
    Point<T> m_p2;
};


/*****************************************************************************
 * Function      : Box.operator+=
 * Description   : return the rectangular box of input points
 * Input         : const Point<T>& p  
 * Output        : None
 * Return        : Box<T>&
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template <typename T>
Box<T>& Box<T>::operator+= (const Point<T>& p) {
    if (isEmpty ()) {
        m_p1 = p;
        m_p2 = p;
    } else {
        if (p.x() < m_p1.x()) {
            m_p1.set_x(p.x());
        } else if (p.x() > m_p2.x()) {
            m_p2.set_x(p.x());
        }

        if (p.y() < m_p1.y()) {
            m_p1.set_y(p.y());
        } else if (p.y() > m_p2.y()) {
            m_p2.set_y(p.y());
        }
    }
  
    return *this;
}

template <typename T>
Box<T>& Box<T>::operator+= (const Box<T>& other) {
    if (!other.isEmpty()) {
        if (isEmpty ()) {
            *this = other;
        } else {
            if (other.m_p1.x() < m_p1.x()) {
                m_p1.set_x(other.m_p1.x());
            }

            if (other.m_p1.y() < m_p1.y()) {
                m_p1.set_y(other.m_p1.y());
            }

            if (other.m_p2.x() > m_p2.x()) {
                m_p2.set_x(other.m_p2.x());
            }

            if (other.m_p2.y() > m_p2.y()) {
                m_p2.set_y(other.m_p2.y());
            }
        }
    }
  
    return *this;
}

/*****************************************************************************
 * Function      : Box.contain
 * Description   : check if a point is in the box
 * Input         : const Point<T>& p  
 * Output        : None
 * Return        : bool
 * Others        : 
 * Record
 * 1.Date        : 20210422
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template <typename T>
bool Box<T>::contain(const Point<T>& p) const {
    if (isEmpty()) {
        return false;
    } else {
        return (!(p.x() < m_p1.x()) && !(m_p2.x() < p.x())) &&
           (!(p.y() < m_p1.y()) && !(m_p2.y() < p.y()));
    }
}

template <typename T>
bool Box<T>::inside(const Box<T>& other) const {
    if (isEmpty() || other.isEmpty()) {
        return false;
    } else {
        return (m_p1.x() >= other.m_p1.x() && m_p2.x() <= other.m_p2.x())
            && (m_p1.y() >= other.m_p1.y() && m_p2.y() <= other.m_p2.y()); 
    }
}


template <typename T>
bool Box<T>::touches (const Box<T>& other) const {
    if (isEmpty() || other.isEmpty()) {
        return false;
    } else {
        return (m_p1.x() <= other.m_p2.x() && m_p2.x() >= other.m_p1.x())
            && (m_p1.y() <= other.m_p2.y() && m_p2.y() >= other.m_p1.y()); 
    }
}

template <typename T>
bool Box<T>::overlaps (const Box<T>& other) const {
    if (isEmpty() || other.isEmpty()) {
        return false;
    } else {
        return (m_p1.x() < other.m_p2.x() && m_p2.x() > other.m_p1.x())
            && (m_p1.y() < other.m_p2.y() && m_p2.y() > other.m_p1.y()); 
    }
}

template <typename T>
Box<T>& Box<T>::enlarge(T l) {
    if (!this->isEmpty()) {
        m_p1.move(-l, -l);
        m_p2.move(l, l);
    }

    return *this; 
}

template <typename T>
Box<T> Box<T>::enlarged(T l) const {
    Box<T> box = *this;

    if (!box.isEmpty()) {
        box.enlarge(l);
    }

    return box; 
}

template<typename T>
void Box<T>::disp() const {
    LOG_INFO("Box| widht=%.2lf| height=%.2lf| p1=<%.2lf, %.2lf>| p2=<%.2lf, %.2lf>|", 
        width(), height(),
        m_p1.x(), m_p1.y(),
        m_p2.x(), m_p2.y());
}

END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Box< NS(base)::Coord > baseBox;

END_NS(base)

#endif

