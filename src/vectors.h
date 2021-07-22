#ifndef __VECTORS_H__
#define __VECTORS_H__
#include"globaldef.h"


BEG_NS(geo)


template<typename T>
class OPC_API Vectors {
public:
    typedef NS(base)::coord_traits<T> coord_traits;
    typedef typename coord_traits::coord_type coord_type; 
    typedef typename coord_traits::area_type area_type;
    typedef typename coord_traits::distance_type distance_type;

    Vectors() : m_x(0), m_y(0) {}

    Vectors(coord_type x, coord_type y) : m_x(x), m_y(y) {}

    template<typename D>
    Vectors(const Vectors<D>& v) : m_x (coord_traits::rounded(v.m_x)), m_y (coord_traits::rounded(v.m_y)) {}
    
    template<typename D>
    Vectors(D x, D y) : m_x (coord_traits::rounded(x)), m_y (coord_traits::rounded(y)) {}

    
    coord_type x() const {
        return m_x;
    }

    coord_type y() const {
        return m_y;
    }

    void set_x(coord_type x) {
        m_x = x;
    }

    void set_y(coord_type y) {
        m_y = y;
    }

    area_type sq_length() const {
        return area_type(m_x) * m_y;
    }

    double double_length () const {
        area_type sq_len = sq_length();
        
        return coord_traits::sqrt(sq_len);
    }

    bool operator<(const Vectors<coord_type>& v) const {
        return m_y < v.m_y || (m_y == v.m_y && m_x < v.m_x);
    }

    bool operator==(const Vectors<coord_type>& v) const {
        return m_x == v.m_x && m_y == v.m_y;
    }

    Vectors<coord_type>& operator+=(const Vectors<coord_type>& v) {
        m_x += v.m_x;
        m_y += v.m_y;

        return *this;
    }

    Vectors<coord_type>& operator-=(const Vectors<coord_type>& v) {
        m_x -= v.m_x;
        m_y -= v.m_y;

        return *this;
    }

    template<typename D>
    Vectors<coord_type>& operator*=(D d) {
        m_x = coord_traits::rounded(m_x * d);
        m_y = coord_traits::rounded(m_y * d);

        return *this;
    }

    template<typename D>
    Vectors<coord_type>& operator/=(D d) {
        double mult = 1.0 / static_cast<double>(d);
        
        m_x = coord_traits::rounded(m_x * d);
        m_y = coord_traits::rounded(m_y * d);

        return *this;
    }

private:
    coord_type m_x;
    coord_type m_y;
};

template<typename T>
Vectors<T> operator+(const Vectors<T>& e1, const Vectors<T>& e2) {
    Vectors<T> e(e1);

    e += e2;
    return e;
}

template<typename T>
Vectors<T> operator-(const Vectors<T>& e1, const Vectors<T>& e2) {
    Vectors<T> e(e1);

    e -= e2;
    return e;
}

template<typename T, typename D>
Vectors<T> operator*(const Vectors<T>& e1, D s) {
    Vectors<T> e(e1);

    e *= s;
    return e;
}

template<typename T, typename D>
Vectors<T> operator/(const Vectors<T>& e1, D s) {
    Vectors<T> e(e1);

    e /= s;
    return e;
}


template<typename T>
typename NS(base)::coord_traits<T>::area_type vprod(const Vectors<T>& e1, const Vectors<T>& e2) {
    return NS(base)::coord_traits<T>::vprod(e1.x(), e1.y(), e2.x(), e2.y(), 0, 0);
}

template<typename T>
int vprod_sign(const Vectors<T>& e1, const Vectors<T>& e2) {
    return NS(base)::coord_traits<T>::vprod_sign(e1.x(), e1.y(), e2.x(), e2.y(), 0, 0);
}

template<typename T>
typename NS(base)::coord_traits<T>::area_type sprod(const Vectors<T>& e1, const Vectors<T>& e2) {
    return NS(base)::coord_traits<T>::sprod(e1.x(), e1.y(), e2.x(), e2.y(), 0, 0);
}

template<typename T>
int sprod_sign(const Vectors<T>& e1, const Vectors<T>& e2) {
    return NS(base)::coord_traits<T>::sprod_sign(e1.x(), e1.y(), e2.x(), e2.y(), 0, 0);
}


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Vectors< NS(base)::Coord > baseVectors;
typedef NS(geo)::Vectors< NS(base)::DCoord > baseDVectors;

END_NS(base)

#endif

