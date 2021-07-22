#ifndef __EDGEPAIR_H__
#define __EDGEPAIR_H__
#include"globaldef.h"
#include"edge.h"
#include"box.h"


BEG_NS(geo)

template<typename T>
class OPC_API EdgePair {
public:
    typedef Edge<T> edge_type;
    typedef Box<T> box_type;

    EdgePair() : m_first(), m_second() {}

    EdgePair(const edge_type& first, const edge_type& second) :
        m_first(first), m_second(second) {}

    const edge_type& first() const {
        return m_first;
    }

    const edge_type& second() const {
        return m_second;
    } 

    edge_type& first() {
        return m_first;
    }

    edge_type& second() {
        return m_second;
    }

    const box_type box() const {
        return m_first.box() + m_second.box();
    }

    void set_first(const edge_type& e) {
        m_first = e;
    }

    void set_second(const edge_type& e) {
        m_second = e;
    }

private:
    edge_type m_first;
    edge_type m_second;
};

END_NS(geo) 

BEG_NS(base)

typedef NS(geo)::EdgePair< NS(base)::Coord > baseEdgePair;

END_NS(base)

#endif

