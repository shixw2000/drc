#include"coordtype.h"


BEG_NS(base)


unsigned int AtomicInt::m_generator = 0;

unsigned int AtomicInt::next()  {
    return __sync_add_and_fetch(&m_generator, 1);
}

/* only calculate the edges with angle between 180-20 and 180+20 */
bool sprod_angle_filter(double ax, double ay, double bx, double by) {
    static const double EDGE_FILTER_MAX_ANGLE = 20.;
    static const double filter_angle = GLOBAL_NS(tan)(EDGE_FILTER_MAX_ANGLE * M_PI / 180);
    double dot = ax * bx + ay * by;
    double cross = ax * by - ay * bx;
    
    if (dot < 0) {
        double pb = (-dot) * filter_angle;
        
        if (cross <= pb && -cross <= pb) {
            /* calculate distance of this edge */
            return false;
        } 
    } 

    /* ignore this edge */
    return true;
}


END_NS(base)


