#ifndef __EDGEFILTER_H__
#define __EDGEFILTER_H__
#include"globaldef.h"
#include"point.h"
#include"edge.h"
#include"edgepair.h"
#include"vectors.h"


BEG_NS(alg)

typedef NS(base)::baseTraits traits_type;
typedef traits_type::coord_type coord_type;
typedef traits_type::area_type area_type;
typedef traits_type::distance_type distance_type;


class OPC_API EdgeRelationFilter {
public: 
    typedef NS(base)::basePoint point_type;
    typedef NS(base)::baseVectors vectors_type;
    typedef NS(base)::baseEdge edge_type;
    typedef NS(base)::baseEdgePair edgepair_type; 
    typedef NS(base)::baseDTraits Dtraits_type;
    typedef NS(base)::baseDPoint Dpoint_type;
    typedef NS(base)::baseDVectors Dvectors_type;
    

    explicit EdgeRelationFilter(distance_type d) : m_d(d) {}

    distance_type distance() const {
        return m_d;
    }
    
    bool check(const edge_type& a, const edge_type& b, distance_type* output) const;

protected:
    bool euclidian_near_part_of_edge(distance_type d, 
        const edge_type& e, const edge_type& other, distance_type* output) const;

    bool isEdgeFilter(const edge_type& a, const edge_type& b) const;

private:
    const distance_type m_d;
};

END_NS(alg) 

#endif

