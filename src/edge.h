#ifndef __EDGE_H__
#define __EDGE_H__
#include<utility>
#include<algorithm>
#include"globaldef.h"
#include"box.h"
#include"point.h"
#include"coordtype.h"


BEG_NS(geo)

template<typename X>
X OPC_API div_exact(X x, double a, double b) {
    return X( double(x) * a / b );
}

template<typename T>
class OPC_API Edge {
public: 
    typedef NS(base)::coord_traits<T> coord_traits;
    typedef typename coord_traits::coord_type coord_type; 
    typedef typename coord_traits::area_type area_type;
    typedef typename coord_traits::distance_type distance_type;
    typedef Point<coord_type> point_type;
    typedef Box<coord_type> box_type;

    Edge() : m_p1(), m_p2() {}
    
    Edge(coord_type x1, coord_type y1, coord_type x2, coord_type y2) : m_p1(x1, y1), m_p2(x2, y2) {}
        
    Edge(const point_type& p1, const point_type& p2) : m_p1(p1), m_p2(p2) {}

    template<typename X>
    Edge(const Edge<X>& other) : m_p1(other.m_p1), m_p2(other.m_p2) {}

    const point_type& p1() const {
        return m_p1;
    }

    const point_type& p2() const {
        return m_p2;
    }

    coord_type x1() const {
        return m_p1.x();
    }

    coord_type y1() const {
        return m_p1.y();
    }

    coord_type x2() const {
        return m_p2.x();
    }

    coord_type y2() const {
        return m_p2.y();
    }

    coord_type dx() const {
        return m_p2.x() - m_p1.x();
    }

    coord_type dy() const {
        return m_p2.y() - m_p1.y();
    }

    void set_p1(const point_type& p1) {
        m_p1 = p1;
    }

    void set_p2(const point_type& p2) {
        m_p2 = p2;
    }

    Edge<T>& swap_points() {
        point_type p = m_p1;

        m_p1 = m_p2;
        m_p2 = p;
        return *this;
    }

    Edge<T> swapped_points() const {
        Edge<T> e = *this;
        
        e.swap_points();
        return e;
    }

    box_type box() const {
        return box_type(m_p1, m_p2);
    }

    bool is_ortho() const {
        return m_p1.x() == m_p2.x() || m_p1.y() == m_p2.y();
    }

    bool is_degenerate () const {
        return m_p1 == m_p2;
    }

    area_type sq_length() const {
        return coord_traits::sq_length(m_p2.x(), m_p2.y(), m_p1.x(), m_p1.y());
    }

    double double_length() const {
        area_type sq_len = sq_length();
        
        return coord_traits::sqrt(sq_len);
    }

    Edge<coord_type>& shift (coord_type e) {
        if (!is_degenerate()) {
            double len = double_length();
            coord_type x = div_exact(dx(), e, len);
            coord_type y = div_exact(dy(), e, len);
            
            move(-y, x);
        }

        return *this;
    }

    Edge<coord_type> shifted (coord_type e) const { 
        if (is_degenerate ()) {
            return *this;
        } else {
            double len = double_length();
            coord_type x = div_exact(dx(), e, len);
            coord_type y = div_exact(dy(), e, len);

            return moved(-y, x);
        }
    }

    Edge<coord_type>& move(const point_type& o) {
        m_p1.add(o);
        m_p2.add(o);

        return *this;
    }

    Edge<coord_type> moved(const point_type& o) const {
        Edge<coord_type> e(*this);
        
        e.move(o); 
        return e;
    }

    bool parallel(const Edge<coord_type> &e) const {
        int sign = 0;

        sign = coord_traits::vprod_sign(dx(), dy(), e.dx(), e.dy(), 0, 0);
        return 0 == sign;
    }

    /* the distance is computed from d = (a x b) / sqrt (a * a) */
    distance_type distance(const point_type& p) const {
        area_type vx = 0;
        double d = .0;

        if (!is_degenerate()) { 
            vx = coord_traits::vprod(x2(), y2(), p.x(), p.y(), x1(), y1());
            d = vx / double_length(); 
        } else {
            /* undefined result */
            d = .0;
        }
        
        return coord_traits::rounded_distance(d);
    }

    /* absolute distance */
    distance_type abs_distance(const point_type& p) const {
        area_type vx = 0;
        double d = .0;

        if (!is_degenerate()) { 
            vx = coord_traits::vprod(x2(), y2(), p.x(), p.y(), x1(), y1());
            d = fabs(vx / double_length()); 
        } else {
            /* undefined result */
            d = .0;
        }
        
        return coord_traits::rounded_distance(d);
    }

    /* point is on the line of the edge */
    bool online(const point_type& p) const {
        return !is_degenerate() && coord_traits::isZero( distance(p) );
    }

    /* True if the point is on the edge. */
    bool contains(const point_type& p) const {
        if (is_degenerate ()) {
            return m_p1 == p;
        } else {
            return coord_traits::isZero( distance(p) ) &&
                coord_traits::sprod_sign (p.x (), p.y (), m_p2.x (), m_p2.y (), m_p1.x (), m_p1.y ()) >= 0 &&
                coord_traits::sprod_sign (p.x (), p.y (), m_p1.x (), m_p1.y (), m_p2.x (), m_p2.y ()) >= 0;
        }
    }

    /* True if the point is on the edge but not equal p1 or p2. */
    bool contains_excl(const point_type& p) const {
        if (is_degenerate ()) {
            return false;
        } else {
            return coord_traits::isZero( distance(p) ) &&
                coord_traits::sprod_sign(p.x (), p.y (), m_p2.x (), m_p2.y (), m_p1.x (), m_p1.y ()) > 0 &&
                coord_traits::sprod_sign(p.x (), p.y (), m_p1.x (), m_p1.y (), m_p2.x (), m_p2.y ()) > 0;
        }
    }    

    /* Coincidence is defined by being parallel, oriented the same way
        and that both edges share more than one point */
    bool coincident(const Edge<coord_type> &e) const {
        return !is_degenerate () && !e.is_degenerate () &&
            coord_traits::isZero( distance(e.p1()) ) &&
            coord_traits::isZero( distance(e.p2()) ) &&
            (sprod_sign (*this, e) < 0 ? 
                (coord_traits::sprod_sign (e.p2 ().x (), e.p2 ().y (), p1 ().x (), p1 ().y (), p2 ().x (), p2 ().y ()) > 0 && 
                    coord_traits::sprod_sign (e.p1 ().x (), e.p1 ().y (), p2 ().x (), p2 ().y (), p1 ().x (), p1 ().y ()) > 0) :
                (coord_traits::sprod_sign (e.p1 ().x (), e.p1 ().y (), p1 ().x (), p1 ().y (), p2 ().x (), p2 ().y ()) > 0 && 
                    coord_traits::sprod_sign (e.p2 ().x (), e.p2 ().y (), p2 ().x (), p2 ().y (), p1 ().x (), p1 ().y ()) > 0));
    }

    /* Returns true if the edges intersect */
    bool intersect(const Edge<coord_type>& e) const {
        if (is_degenerate ()) {
            return e.contains(p1());
        } else if (e.is_degenerate()) {
            return contains(e.p1());
        } else if (!this->box().touches(e.box())) {
            return false;
        } else if (is_ortho() && e.is_ortho()) {
            return true;
        } else {
            return crossed_by(e) && e.crossed_by(*this);
        }
    }

    /* in addtion to intersect, also return the intersection point if true */
    STD_NS(pair)<bool, point_type> intersect_point(const Edge<coord_type>& e) const {
        if (is_degenerate ()) {
            if (e.contains(p1())) {
                return STD_NS(make_pair)(true, p1());
            } else {
                return STD_NS(make_pair)(false, point_type(0, 0));
            }
        } else if (e.is_degenerate()) {
            if (contains(e.p1())) {
                return STD_NS(make_pair)(true, e.p1());
            } else {
                return STD_NS(make_pair)(false, point_type(0, 0));
            }
        } else if (!this->box().touches(e.box())) {
            return STD_NS(make_pair)(false, point_type(0, 0));
        } else if (is_ortho() && e.is_ortho()) {
            coord_type x = STD_NS(max)( STD_NS(min)(p1().x(), p2().x()), STD_NS(min)(e.p1().x(), e.p2().x()) );
            coord_type y = STD_NS(max)( STD_NS(min)(p1().y(), p2().y()), STD_NS(min)(e.p1().y(), e.p2().y()) );
            
            return STD_NS(make_pair)(true, point_type(x, y));
        } else if (!crossed_by(e)) {
            return STD_NS(make_pair)(false, point_type(0, 0));
        } else {
            bool res = true;
            bool ends_on_edge = false;

            STD_NS(pair)<area_type, int> vsa = coord_traits::vprod_with_sign(e.p2().x(), e.p2().y(), 
                m_p1.x(), m_p1.y(), e.p1().x(), e.p1().y());
            area_type vxa = vsa.first;
            
            if (vsa.second < 0) {
                res = false;
            } else if (vsa.second == 0) {
                ends_on_edge = true;
            }

            STD_NS(pair)<area_type, int> vsb = coord_traits::vprod_with_sign(e.p2().x(), e.p2().y(),
                m_p2.x(), m_p2.y(), e.p1().x(), e.p1().y());
            area_type vxb = -vsb.first;
            
            if (vsb.second > 0) {
                res = !res;
            } else if (vsb.second == 0) {
                ends_on_edge = true;
            }

            if (ends_on_edge) {

                if (contains(e.p1())) {
                    return STD_NS(make_pair)(true, e.p1());
                } else if (contains(e.p2())) {
                    return STD_NS(make_pair)(true, e.p2());
                } else if (e.contains(p1())) {
                    return STD_NS(make_pair)(true, p1());
                } else if (e.contains(p2())) {
                    return STD_NS(make_pair)(true, p2());
                } else {
                    return STD_NS(make_pair)(false, point_type(0, 0));
                }

            } else if (res) {

                if (vxa < 0) {
                    vxa = -vxa;
                }
                
                if (vxb < 0) {
                    vxb = -vxb;
                }

                coord_type x = m_p1.x() + div_exact(dx(), vxa, vxa + vxb);
                coord_type y = m_p1.y() + div_exact(dy(), vxa, vxa + vxb);

                return STD_NS(make_pair)(true, point_type(x, y));

            } else {
                return STD_NS(make_pair)(false, point_type(0, 0));
            }
        }
    }

    /* Check, if an edge is cut by a line, the basically algorithm is:
     * if side_of e.p1 < 0, res = false
     * if side_of e.p1 == 0, return true;
     * if side_of e.p2 > 0, res = !res
     * if side_of e.p2 == 0, return true;
     * return res;
     ** attention: a cross_by b, donot means b cross_by a. for example, ...
     */
    bool crossed_by(const Edge<coord_type>& e) const {
        bool res = true;

        int vsa = coord_traits::vprod_sign(m_p2.x(), m_p2.y(), e.p1().x(), e.p1().y(), m_p1.x(), m_p1.y());
        if (vsa < 0) {
            res = false;
        } else if (vsa == 0) {
            return true;
        }

        int vsb = coord_traits::vprod_sign(m_p2.x(), m_p2.y(), e.p2().x(), e.p2().y(), m_p1.x(), m_p1.y());
        if (vsb > 0) {
            res = !res;
        } else if (vsb == 0) {
            return true;
        }

        return res;
    }

    /* In addition to "crossed_by", also returns
     * the point at which the edge is crossed */
    STD_NS(pair)<bool, point_type> crossed_by_point(const Edge<coord_type>& e) const {
        bool res = true;

        STD_NS(pair)<area_type, int> vsa = coord_traits::vprod_with_sign(m_p2.x(), m_p2.y(), 
            e.p1().x(), e.p1().y(), m_p1.x(), m_p1.y());
        area_type vxa = vsa.first;
        
        if (vsa.second < 0) {
            res = false;
        } else if (vsa.second == 0) {
            return STD_NS(make_pair)(true, e.p1());
        }

        STD_NS(pair)<area_type, int> vsb = coord_traits::vprod_with_sign(m_p2.x(), m_p2.y(),
            e.p2().x(), e.p2().y(), m_p1.x(), m_p1.y());
        area_type vxb = -vsb.first;
        
        if (vsb.second > 0) {
            res = !res;
        } else if (vsb.second == 0) {
            return STD_NS(make_pair)(true, e.p2());
        }

        if (res) {
            if (vxa < 0) {
                vxa = -vxa;
            }

            if (vxb < 0) {
                vxb = -vxb;
            }

            coord_type x = e.p1().x() + div_exact(e.dx (), vxa, vxa + vxb);
            coord_type y = e.p1().y() + div_exact(e.dy (), vxa, vxa + vxb);

            return STD_NS(make_pair)(true, point_type(x, y));
        } else {
            return STD_NS(make_pair)(false, point_type(0, 0));
        }
    }

    /* Returns 1 if the point is "left" of the edge, 0 if on
     * and -1 if the point is "right" of the edge.*/
    int side_of (const point_type& p) const {
        if (is_degenerate()) {
            /* undefined */
            return 0;
        } else {
            return coord_traits::vprod_sign (m_p2.x(), m_p2.y(), p.x(), p.y(), m_p1.x(), m_p1.y());
        }
    }

    /* Compute the projection of a point on the edge */
    STD_NS(pair)<bool, point_type> projected(const point_type& pt) const {
        return Edge<coord_type>(pt, pt.moveP(dy(), -dx())).crossed_by_point(*this);
    }

    /* the projected case of e on this egde */
    distance_type edge_projection(const Edge<coord_type>& e) const {
        if (is_degenerate() || e.is_degenerate()) {
            return 0;
        } else {
            area_type tmp = 0;
            area_type slen = sq_length();
            area_type l1 = coord_traits::sprod(m_p2.x(), m_p2.y(), e.m_p1.x(), e.m_p1.y(), m_p1.x(), m_p1.y()); 
            area_type l2 = coord_traits::sprod(m_p2.x(), m_p2.y(), e.m_p2.x(), e.m_p2.y(), m_p1.x(), m_p1.y());

            if (l1 > l2) {
                tmp = l1;

                l1 = l2;
                l2 = tmp;
            }

            if (l2 < 0 || l1 > slen) {
                return coord_traits::rounded_distance(0);
            } else {
                if (l2 > slen) {
                    l2 = slen;
                }

                if (l1 < 0) {
                    l1 = 0;
                } 
                
                return coord_traits::rounded_distance((l2-l1) / GLOBAL_NS(sqrt)(slen));
            }
        }
    }

    /* Compute cut point of two lines  */
    STD_NS(pair)<bool, point_type> cut_point(const Edge<coord_type>& e) const {
        STD_NS(pair)<area_type, int> vsa = coord_traits::vprod_with_sign(e.dx(), e.dy(), dx(), dy(), 0, 0);
        area_type vxa= vsa.first;
        
        if (vsa.second != 0) {
            area_type vxb = coord_traits::vprod(e.p1().x(), e.p1().y(), p2().x(), p2().y(), p1().x(), p1().y());
            coord_type x = div_exact(e.dx(), vxb, vxa);
            coord_type y = div_exact(e.dy(), vxb, vxa);
        
            return STD_NS(make_pair)(true, e.p1().moveP(-x, -y));
        } else {
            return STD_NS(make_pair)(false, point_type(0, 0));
        }
    }    

private:
    point_type m_p1;
    point_type m_p2;
};


template<typename T>
typename NS(base)::coord_traits<T>::area_type vprod(const Edge<T>& e1, const Edge<T>& e2) {
    return NS(base)::coord_traits<T>::vprod(e1.dx(), e1.dy(), e2.dx(), e2.dy(), 0, 0);
}

template<typename T>
int vprod_sign(const Edge<T>& e1, const Edge<T>& e2) {
    return NS(base)::coord_traits<T>::vprod_sign(e1.dx(), e1.dy(), e2.dx(), e2.dy(), 0, 0);
}

template<typename T>
typename NS(base)::coord_traits<T>::area_type sprod(const Edge<T>& e1, const Edge<T>& e2) {
    return NS(base)::coord_traits<T>::sprod(e1.dx(), e1.dy(), e2.dx(), e2.dy(), 0, 0);
}

template<typename T>
int sprod_sign(const Edge<T>& e1, const Edge<T>& e2) {
    return NS(base)::coord_traits<T>::sprod_sign(e1.dx(), e1.dy(), e2.dx(), e2.dy(), 0, 0);
}


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Edge< NS(base)::Coord > baseEdge;

END_NS(base)

#endif

