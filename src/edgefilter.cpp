#include<algorithm>
#include<cmath>
#include<limits>
#include"edgefilter.h"


BEG_NS(alg)


bool EdgeRelationFilter::isEdgeFilter(const edge_type& a, const edge_type& b) const {
    bool bFilter = false;

    /* only use edges that perpendicular with x or y axis */
    if (true/*a.is_ortho() && b.is_ortho()*/) {
        bFilter = NS(base)::sprod_angle_filter(a.dx(), a.dy(), b.dx(), b.dy());
    } else {
        bFilter = true;
    }
    
    return bFilter;
}

bool EdgeRelationFilter::check(const edge_type& a, const edge_type& b, distance_type* output) const {
    bool in1 = false;
    bool in2 = false;
    distance_type min_distance1 = 0;
    distance_type min_distance2 = 0;
    double d_out = std::numeric_limits<double>::max();

    if (a.is_degenerate() || b.is_degenerate()) {
        return false;
    }

    /* check if the edges have an angle less than 90 */
    if (isEdgeFilter(a, b)) {
        return false;
    }

    /* box check */
    if (!a.box().enlarged(m_d).touches(b.box())) {
        return false;
    } 

    in1 = euclidian_near_part_of_edge(m_d, a, b, &min_distance1);
    in2 = euclidian_near_part_of_edge(m_d, b, a, &min_distance2); 
    if (in1 || in2) {
        if (in1) {
            if (d_out > min_distance1) {
                d_out = min_distance1;
            }
        } 
        
        if (in2) {
            if (d_out > min_distance2) {
                d_out = min_distance2;
            }
        }
        
        *output = traits_type::rounded_distance(d_out);
        return true;
    } else { 
        return false;
    }
}


/* Returns the part of the "other" edge which is on the inside side of e and within distance d */
bool EdgeRelationFilter::euclidian_near_part_of_edge(distance_type d, 
    const edge_type& e, const edge_type& other, distance_type* output) const {
    double distance = 0;
    
    if (e.is_degenerate()) {
        return false;
    }

    edge_type g(other);
    int s1 = e.side_of(g.p1());
    int s2 = e.side_of(g.p2());

    /* if the edges are collinear.*/
    if (s1 == 0 && s2 == 0) {
        return false;
    } else if (s1 > 0 && s2 > 0) {
        //  keep only part of other which is on the "inside" side of e
        return false; 
    } else if (s2 > 0) {
        g.set_p2( g.cut_point(e).second );
    } else if (s1 > 0) {
        g.set_p1( g.cut_point(e).second );
    }

    //  Handle the case of point vs. edge
    if (g.is_degenerate()) {

        point_type o = g.p1();

        if (e.side_of(o) >= 0) {
            return false;
        }

        area_type a = e.sq_length ();
        area_type b = NS(geo)::sprod(edge_type(e.p1(), o), e); 
        area_type c = NS(geo)::sq_length(e.p1(), o) - (area_type)d * d;

        double s = b * b - a * c;
        if (s >= 0) {
            double ratio = b / a;
            
            if (!traits_type::less(ratio, 0) && !traits_type::less(1, ratio)) {
                if (output) {
                    *output = e.abs_distance(o);
                }
                
                return true;
            }
        }

        return false; 
    }

    double l1 = std::numeric_limits<double>::max();
    double l2 = -std::numeric_limits<double>::max();

    //  handle the parallel case
    if (e.parallel (g)) {
        distance = e.abs_distance(g.p1 ());
        
        if (distance <= d && e.edge_projection(g) > 0) {
            if (NULL != output) {
                *output = distance;
            }
            
            return true;
        } else { 
            return false;
        }
        
    } else {
        double ratio = d / e.double_length(); 
        point_type p(e.dy(), -e.dx());
        edge_type en(point_type(0,0), p); // perpendicular edge
        Dpoint_type dp(e.p1().x() + p.x() * ratio, e.p1().y() + p.y() * ratio);
        NS(geo)::Edge<double> e1d(Dpoint_type(g.p1()), dp);
        
        double det = NS(geo)::vprod(g, e);
        double lp1 = NS(geo)::vprod<double>(e1d, NS(geo)::Edge<double>(e)) / det;
        double lp2 = NS(geo)::vprod(edge_type(g.p1(), e.p1()), e) /det;
   
        if (lp1 > lp2) {
            std::swap (lp1, lp2);
        }

        if (NS(geo)::sprod_sign (e, g) == 0) {
            if (g.side_of(e.p1 ()) * g.side_of(e.p2 ()) <= 0) {
                l1 = lp1;
                l2 = lp2;
            }
        } else {
            double det = NS(geo)::vprod(g, en);
            double lt1 = NS(geo)::vprod (edge_type(g.p1(), e.p1()), en) / det;
            double lt2 = NS(geo)::vprod (edge_type(g.p1(), e.p2()), en) / det;

            if (lt1 > lt2) {
                std::swap (lt1, lt2);
            }

            double ll1 = std::max(lp1, lt1);
            double ll2 = std::min(lp2, lt2);
            if (ll1 <= ll2) {
                l1 = ll1;
                l2 = ll2;
            }
        }

    }

    l1 = std::max (0.0, l1);
    l2 = std::min (1.0, l2);

    if (l1 >= l2) {
        return false;
    } else {
        if (output) {
            point_type crossP1 = g.p1().moveP(g.dx() * l1, g.dy() * l1);
            point_type crossP2 = g.p1().moveP(g.dx() * l2, g.dy() * l2);
            distance_type min_distance1 = 0;
            distance_type min_distance2 = 0;

            min_distance1 = e.abs_distance(crossP1);
            min_distance2 = e.abs_distance(crossP2);
            if (min_distance1 < min_distance2) {
                *output = min_distance1;
            } else {
                *output = min_distance2;
            }
        }
        
        return true;
    }
}


END_NS(alg)

