#ifndef __COORDTYPE_H__
#define __COORDTYPE_H__
#include<utility>
#include<math.h>
#include"globaldef.h" 


BEG_NS(base)

typedef double Coord;
typedef double DCoord;


/* used by short, int, long */
template<typename T>
struct coord_traits {
    typedef T coord_type;
    typedef unsigned long area_type;
    typedef unsigned long distance_type;

    template<typename X>
    static bool isZero(X v) {
        return 0 == v;
    }
    
    static bool equal (T c1, T c2) {
        return c1 == c2; 
    }

    static bool less (T c1, T c2) {
        return c1 < c2 ;
    } 

    template <typename X>
    static T rounded (X v)    { 
        return T(v); 
    }

    static distance_type rounded_distance(double v) {
        return distance_type(v);
    }

    static area_type sq_length (T ax, T ay, T bx, T by) {
        return (area_type(ax - bx) * (ax - bx) + area_type(ay - by) * (ay - by));
    }

    static double length1(T ax, T ay, T bx, T by) {
        return GLOBAL_NS(sqrt)(area_type(ax - bx) * (ax - bx) + area_type(ay - by) * (ay - by));
    }

    static double sqrt(T x) {
        return GLOBAL_NS(sqrt)(x);
    }

    /*****************************************************************************
     * Function      : sprod
     * Description   : The scalar product: (ax - cx) * (bx - cx) + (ay - cy) * (by - cy)
     * Input         :   
     * Output        : 
     * Return        : 
     * Others        : 
     * Record
     * 1.Date        : 20210508
     *   Author      : shixw
     *   Modification: Created function

    *****************************************************************************/
    static area_type sprod(T ax, T ay, T bx, T by, T cx, T cy) {
        return area_type(ax-cx) * (bx-cx) + area_type(ay-cy) * (by-cy);
    }

    /*****************************************************************************
     * Function      : sprod_sign
     * Description   : The sign of the scalar product (-1: negative, 0: zero, +1: positive)
     * Input         :   
     * Output        : 
     * Return        : 
     * Others        : 
     * Record
     * 1.Date        : 20210508
     *   Author      : shixw
     *   Modification: Created function

    *****************************************************************************/
    static int sprod_sign (T ax, T ay, T bx, T by, T cx, T cy) {
        area_type p1 = area_type(ax-cx) * (bx-cx);
        area_type p2 = -(area_type(ay-cy) * (by-cy));
        
        if (p1 > p2) {
            return 1;
        } else if (p1 == p2) {
            return 0;
        } else {
            return -1;
        }
    }

    static STD_NS(pair)<area_type, int> sprod_with_sign(T ax, T ay, T bx, T by, T cx, T cy) {
        area_type p1 = area_type(ax-cx) * (bx-cx);
        area_type p2 = -(area_type(ay-cy) * (by-cy));

        if (p1 < p2) {
            return STD_NS(make_pair)(p1 - p2, -1);
        } else if (p1 == p2) {
            return STD_NS(make_pair)(0, 0);
        } else {
            return STD_NS(make_pair)(p1 - p2, 1);
        } 
    }


    /*****************************************************************************
     * Function      : vprod
     * Description   : The vector product: (ax - cx) * (by - cy) - (ay - cy) * (bx - cx)
     * Input         :   
     * Output        : 
     * Return        : 
     * Others        : 
     * Record
     * 1.Date        : 20210508
     *   Author      : shixw
     *   Modification: Created function

    *****************************************************************************/
    static area_type vprod(T ax, T ay, T bx, T by, T cx, T cy) {
        return area_type(ax-cx) * (by-cy) - area_type(ay-cy) * (bx-cx);
    }

    /*****************************************************************************
     * Function      : vprod_sign
     * Description   : The sign of the vector product (-1: negative, 0: zero, +1: positive)
     * Input         :   
     * Output        : 
     * Return        : 
     * Others        : 
     * Record
     * 1.Date        : 20210508
     *   Author      : shixw
     *   Modification: Created function

    *****************************************************************************/
    static int vprod_sign (T ax, T ay, T bx, T by, T cx, T cy) {
        area_type p1 = area_type(ax-cx) * (by-cy);
        area_type p2 = area_type(ay-cy) * (bx-cx);
        if (p1 > p2) {
            return 1;
        } else if (p1 == p2) {
            return 0;
        } else {
            return -1;
        }
    }

    static STD_NS(pair)<area_type, int> vprod_with_sign(T ax, T ay, T bx, T by, T cx, T cy) {
        area_type p1 = area_type(ax-cx) * (by-cy);
        area_type p2 = area_type(ay-cy) * (bx-cx);

        if (p1 < p2) {
            return STD_NS(make_pair)(p1 - p2, -1);
        } else if (p1 == p2) {
            return STD_NS(make_pair)(0, 0);
        } else {
            return STD_NS(make_pair)(p1 - p2, 1);
        } 
    } 
};

template<>
struct coord_traits<double> {
    typedef double coord_type;
    typedef double area_type;
    typedef double distance_type;
    
    static double prec () {
        return 1e-5; 
    }

    static bool isZero(double v) {
        return v <= prec() && -v <= prec();
    }
    
    static bool equal (double c1, double c2) {        
        return (c1 - c2) <= prec () && (c2 - c1) <= prec(); 
    }

    static bool less (double c1, double c2) {
        return c1 <= c2 - prec ();
    } 

    template <typename X>
    static double rounded (X v)    { 
        return double (v); 
    }

    static distance_type rounded_distance(double v) {
        return distance_type(v);
    }

    static area_type sq_length(double ax, double ay, double bx, double by) {
        return (double)((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    }

    static double length1(double ax, double ay, double bx, double by) {
        return GLOBAL_NS(sqrt)((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    }

    static double sqrt(double x) {
        return GLOBAL_NS(sqrt)(x);
    }

    static area_type sprod(coord_type ax, coord_type ay, coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        return area_type(ax-cx) * (bx-cx) + area_type(ay-cy) * (by-cy);
    }
    
    static int sprod_sign (coord_type ax, coord_type ay, coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        area_type dx1 = ax - cx, dy1 = ay - cy;
        area_type dx2 = bx - cx, dy2 = by - cy;
        area_type pa = (sqrt(dx1 * dx1 + dy1 * dy1) + sqrt(dx2 * dx2 + dy2 * dy2)) * prec();
        area_type p1 = dx1 * dx2;
        area_type p2 = -dy1 * dy2;

        if (p1 <= p2 - pa) {
            return -1;
        } else if (p1 < p2 + pa) {
            return 0;
        } else {
            return 1;
        }  
    }

    static STD_NS(pair)<area_type, int> sprod_with_sign(coord_type ax, coord_type ay, 
        coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        area_type dx1 = ax - cx, dy1 = ay - cy;
        area_type dx2 = bx - cx, dy2 = by - cy;     
        area_type pa = (sqrt(dx1 * dx1 + dy1 * dy1) + sqrt(dx2 * dx2 + dy2 * dy2)) * prec();
        area_type p1 = dx1 * dx2;
        area_type p2 = -dy1 * dy2;

        if (p1 <= p2 - pa) {
            return STD_NS(make_pair)(p1 - p2, -1);
        } else if (p1 < p2 + pa) {
            return STD_NS(make_pair)(0, 0);
        } else {
            return STD_NS(make_pair)(p1 - p2, 1);
        } 
    }

    static area_type vprod(coord_type ax, coord_type ay, coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        return area_type(ax-cx) * (by-cy) - area_type(ay-cy) * (bx-cx);
    }

    static int vprod_sign(coord_type ax, coord_type ay, coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        area_type dx1 = ax - cx, dy1 = ay - cy;
        area_type dx2 = bx - cx, dy2 = by - cy;
        area_type pa = (sqrt(dx1 * dx1 + dy1 * dy1) + sqrt(dx2 * dx2 + dy2 * dy2)) * prec();
        area_type p1 = dx1 * dy2;
        area_type p2 = dy1 * dx2;
        
        if (p1 <= p2 - pa) {
            return -1;
        } else if (p1 < p2 + pa) {
            return 0;
        } else {
            return 1;
        }
    }

    static STD_NS(pair)<area_type, int> vprod_with_sign(coord_type ax, coord_type ay, 
        coord_type bx, coord_type by, coord_type cx, coord_type cy) {
        area_type dx1 = ax - cx, dy1 = ay - cy;
        area_type dx2 = bx - cx, dy2 = by - cy;
        area_type pa = (sqrt(dx1 * dx1 + dy1 * dy1) + sqrt(dx2 * dx2 + dy2 * dy2)) * prec();
        area_type p1 = dx1 * dy2;
        area_type p2 = dy1 * dx2;

        if (p1 <= p2 - pa) {
            return STD_NS(make_pair)(p1 - p2, -1);
        } else if (p1 < p2 + pa) {
            return STD_NS(make_pair)(0, 0);
        } else {
            return STD_NS(make_pair)(p1 - p2, 1);
        }
    } 
};

/* tan(0.1) ~ 0 */
static double angle_prec() {
    static const double angle_prec = GLOBAL_NS(tan)(0.1 * M_PI / 180);
    
    return angle_prec;
}

/* ctan(theta) ~ 0 */
template<typename T>
bool isOrthogonal(T ax, T ay, T bx, T by, T cx, T cy) {
    double dx1 = ax - cx, dy1 = ay - cy;
    double dx2 = bx - cx, dy2 = by - cy;
    double dot = dx1 * dx2 + dy1 * dy2;  //r1*r2*cos
    double cross = dx1 * dy2 - dy1 * dx2; //r1*r2*sin 
    double pb = cross * angle_prec();  
    
    if (dot < 0) {
        dot = -dot;
    }

    if (pb < 0) {
        pb = -pb;
    } 

    if (dot > pb) {
        return false;
    } else {
        return true;
    }
}

/* tan(theta) ~ 0 */
template<typename T>
bool isParallel(T ax, T ay, T bx, T by, T cx, T cy) {
    double dx1 = ax - cx, dy1 = ay - cy;
    double dx2 = bx - cx, dy2 = by - cy;
    double cross = dx1 * dy2 - dy1 * dx2; //r1*r2*sin
    double dot = dx1 * dx2 + dy1 * dy2;  //r1*r2*cos
    double pa = dot * angle_prec();

    if (cross < 0) {
        cross = -cross;
    }

    if (pa < 0) {
        pa = -pa;
    }

    if (cross > pa) {
        return false;
    } else {
        return true;
    }
}

class AtomicInt {
public:
    static unsigned int next();

private:
    static unsigned int m_generator;
};

extern bool sprod_angle_filter(double ax, double ay, double bx, double by);

typedef coord_traits<Coord> baseTraits;
typedef coord_traits<DCoord> baseDTraits;

static const double MIN_LINE_LENGTH = 5;
static const double MIN_LINE_LENGTH_SQUARE = MIN_LINE_LENGTH * MIN_LINE_LENGTH;
static const int MIN_LINE_POINT_CNT = 5;

END_NS(base)

#endif

