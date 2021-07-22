#ifndef __SEGDELEGATE_H__
#define __SEGDELEGATE_H__
#include"globaldef.h"
#include"segments.h"
#include"nodedeal.h"


BEG_NS(alg)


template<typename T>
class AreaHandler : public NS(geo)::AreaVisitor<T>, public NS(geo)::SegmentsVisitor<T> {
public:
    AreaHandler(int type, Node<T>* node, NodeDeal<T>* strategy)
        : m_type(type), m_node(node), m_strategy(strategy) {}
        
    virtual ~AreaHandler() {}
    
    virtual void visit(NS(geo)::Segments<T>& segments) {
        NS(geo)::SegmentsEnum<T> enumerator(segments);
        
        enumerator.foreach(*this);
    }

    virtual void visit(const NS(geo)::Polygon<T>* polygon, NS(geo)::Segment<T>& segment) {
        if (NULL != m_strategy) {
            m_strategy->process(m_type, m_node, polygon, &segment);
        }
    } 

private:
    int m_type;
    Node<T>* m_node;
    NodeDeal<T>* m_strategy;
};

template<typename T>
class AreaHandlerEx : public NS(geo)::AreaVisitorEx<T>, public NS(geo)::SegmentsVisitorEx<T> {
public:
    AreaHandlerEx(int type, Node<T>* node, NodeDeal<T>* strategy)
        : m_type(type), m_node(node), m_strategy(strategy) {}
        
    virtual ~AreaHandlerEx() {}
    
    virtual void visit(NS(geo)::Segments<T>& segments1, NS(geo)::Segments<T>& segments2) {
        NS(geo)::SegmentsEnumEx<T> enumerator(segments1, segments2);
        
        enumerator.foreach(*this);
    }

    virtual void visit(const NS(geo)::Polygon<T>* polygon1, NS(geo)::Segment<T>& segment1,
        const NS(geo)::Polygon<T>* polygon2, NS(geo)::Segment<T>& segment2) {
        if (NULL != m_strategy) {
            m_strategy->processEx(m_type, m_node, polygon1, &segment1, polygon2, &segment2);
        }
    } 

private:
    int m_type;
    Node<T>* m_node;
    NodeDeal<T>* m_strategy;
};


template<typename T>
class SegmentEdgeIterator {
    typedef NS(geo)::Edge<T> edge_type;
    typedef NS(geo)::Point<T> point_type;
    typedef NS(geo)::Polygon<T> polygon_type;
    typedef NS(geo)::Segment<T> segment_type;
    
public:
    SegmentEdgeIterator(const polygon_type* parent, const segment_type* segment) :
        m_parent(parent), m_segment(segment), m_cur(m_segment->from()) {}

    SegmentEdgeIterator(const polygon_type* parent, const segment_type* segment, int beg) :
        m_parent(parent), m_segment(segment), m_cur(beg) {}

    SegmentEdgeIterator(const SegmentEdgeIterator& other) : 
        m_parent(other.m_parent), m_segment(other.m_segment), m_cur(other.m_cur) {}

    bool atEnd() const {
        return m_cur > m_segment->to();
    } 
    
    SegmentEdgeIterator& operator++() {
        ++m_cur;
        return *this;
    }

    edge_type operator*() const {
        int second = m_cur + 1;

        if (second >= m_parent->vertices()) {
            second = 0;
        }
        
        point_type p1 = m_parent->get(m_cur);
        point_type p2 = m_parent->get(second);

        return edge_type(p1, p2);
    }
    
private: 
    const polygon_type* m_parent;
    const segment_type* m_segment;
    int m_cur; 
};


END_NS(alg)

#endif

