/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : segments.h
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
#ifndef __SEGMENTS_H__
#define __SEGMENTS_H__

#include<list>
#include<map>
#include"globaldef.h"
#include"polygon.h"
#include"box.h"
#include"enumerator.h"
#include"edge.h"


BEG_NS(geo)


/*****************************************************************************
 * Function      : Segment
 * Description   : indices of continous points in polygon as [from, to]
 * Input         : geo  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
class Segment {
public:
    Segment() : m_from(-1), m_to(-1) {}
    Segment(int from, int to) : m_from(from), m_to(to) {}

    int from() const {
        return m_from;
    }

    int to() const {
        return m_to;
    }

    void setfrom(int from) {
        m_from = from;

        if (-1 == m_to) {
            m_to = from;
        }
    }

    void setto(int to) {
        m_to = to;

        if (-1 == m_from) {
            m_from = to;
        }
    }

    void reset() {
        /* clear all info */
        m_from = m_to = -1;
    }

    bool isEmpty() const {
        return -1 == m_to;
    }

    void disp(const char info[]) const {
        LOG_INFO("%s_Segment| from=%d| to=%d|", info, m_from, m_to);
    }

private:
    int m_from; 
    int m_to;
};


/*****************************************************************************
 * Function      : Segments
 * Description   : sets of segment of the polygon, segment in here **never** overlap 
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
class Segments { 
public:
    typedef STD_NS(list)< Segment<T> > typeSegPairPool;
    typedef typename typeSegPairPool::iterator typeSegPairIter;
    typedef typename typeSegPairPool::const_iterator typeSegPairIterConst;
    
public: 
    Segments() : m_polygon(NULL) {} 
    
    explicit Segments(const Polygon<T>* polygon) : m_polygon(polygon) {
    }

    ~Segments() {m_indices.clear();}

    void setPolygon(const Polygon<T>* polygon) {
        m_polygon = polygon;
    }

    const Polygon<T>* getPolygon() const {
        return m_polygon;
    }

    const Point<T>& getPoint(int index) const {
        return m_polygon->get(index);
    }

    const Point<T>* getPointP(int index) const {
        return m_polygon->getP(index);
    }
    
    int id() const {
        return m_polygon->id();
    }
    
    bool isEmpty() const {
        return m_indices.empty();
    }

    void reset() {
        m_polygon = NULL;
        clearPairs();
    }

    void clearPairs() {
        m_indices.clear();
    }

    const typeSegPairPool& getPairs() const {
        return m_indices;
    }

    typeSegPairPool& getPairs() {
        return m_indices;
    }

    void merge(const Segments<T>& other);
    void merge(const Segment<T>& other);
    void swap(Segments<T>& other);

    void disp(const char info[]) const;
    
private:
    const Polygon<T>* m_polygon;
    typeSegPairPool m_indices;
};


template<typename T>
void Segments<T>::disp(const char info[])  const {
    int cnt = 0;
    typeSegPairIterConst itr;
    
    LOG_INFO("%s_Segments| polygon=%p| segments=%p| segment_size=%d|", 
        info, m_polygon, this, (int)m_indices.size()); 

    for (itr = m_indices.begin(); itr != m_indices.end(); ++itr,++cnt) {
        const Segment<T>& pair = *itr;

        
        LOG_INFO("[%d]: from=%d| to=%d|", cnt, pair.from(), pair.to());
    }
}


/*****************************************************************************
 * Function      : Segments.merge
 * Description   : 相同polygon的两个片段合并到本地
 * Input         : const Segments<T>& segs  
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210423
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
void Segments<T>::merge(const Segments<T>& other) {
    const typename Segments<T>::typeSegPairPool pairs = other.getPairs();
    typeSegPairIterConst itr;
    
    if (id() == other.id()) {
        for (itr = other.m_indices.begin(); itr != other.m_indices.end(); ++itr) {
            const Segment<T>& item = *itr;
            
            merge(item);
        }
    }
}


/*****************************************************************************
 * Function      : SegmentsEnum
 * Description   : 合并相同polygon的一个子片段到本地
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20210423
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
void Segments<T>::merge(const Segment<T>& other) {
    int skipto = -1;
    typeSegPairIter itr;
    typeSegPairIter itrEnd;

    for (itr = m_indices.begin(); itr != m_indices.end(); ++itr) {
        Segment<T>& item = *itr;
        
        if (other.to() < item.from() -1) {
            m_indices.insert(itr, other);

            return;
        } else if (other.from() <= item.to() + 1) {
            if (other.from() < item.from()) {
                item.setfrom(other.from());
            }

            if (item.to() < other.to()) {
                skipto = other.to();
                
                itrEnd = ++itr;
                while (m_indices.end() != itrEnd) {
                    const Segment<T>& skipItem = *itrEnd;

                    if (other.to() < skipItem.from()-1) {
                        break;
                    } else if (other.to() <= skipItem.to()) {
                        skipto = skipItem.to();
                        break;
                    } else {
                        ++itrEnd;
                    }
                }

                item.setto(skipto);
                if (itrEnd != itr) {
                    m_indices.erase(itr, itrEnd);
                }
            }
            
            return;
        } else {
            /* skip to next */
        }
    }

    /* to end */
    m_indices.insert(m_indices.end(), other);
}

template<typename T>
void Segments<T>::swap(Segments<T>& other) { 
    m_indices.swap(other.m_indices);
}


template<typename T>
class SegmentsVisitor {
public:
    virtual ~SegmentsVisitor() {}
    virtual void visit(const Polygon<T>* polygon, Segment<T>& segment) = 0;
};

template<typename T>
class SegmentsEnum { 
public:
    explicit SegmentsEnum(Segments<T>& segs) : m_segs(segs) {} 
    
    void foreach(SegmentsVisitor<T>& v) {
        const Polygon<T>* polygon = m_segs.getPolygon();
        typename Segments<T>::typeSegPairPool pairs = m_segs.getPairs();
        typename Segments<T>::typeSegPairIter itr;

        for (itr = pairs.begin(); itr != pairs.end(); ++itr) {
            Segment<T>& pair = *itr;

            v.visit(polygon, pair);
        }
    }

private:
    Segments<T>& m_segs;
};

template<typename T>
class SegmentsVisitorEx {
public:
    virtual ~SegmentsVisitorEx() {}
    virtual void visit(const Polygon<T>* polygon1, Segment<T>& segment1, 
        const Polygon<T>* polygon2, Segment<T>& segment2) = 0;
};

template<typename T>
class SegmentsEnumEx { 
public:
    SegmentsEnumEx(Segments<T>& segs1, Segments<T>& segs2) : m_segs1(segs1), m_segs2(segs2) {} 
    
    void foreach(SegmentsVisitorEx<T>& v) {
        const Polygon<T>* polygon1 = m_segs1.getPolygon();
        const Polygon<T>* polygon2 = m_segs2.getPolygon();
        typename Segments<T>::typeSegPairPool pairs1 = m_segs1.getPairs();
        typename Segments<T>::typeSegPairIter itr1;
        typename Segments<T>::typeSegPairPool pairs2 = m_segs2.getPairs();
        typename Segments<T>::typeSegPairIter itr2;

        for (itr1 = pairs1.begin(); itr1 != pairs1.end(); ++itr1) {
            Segment<T>& seg1 = *itr1;

            for (itr2 = pairs2.begin(); itr2 != pairs2.end(); ++itr2) {
                Segment<T>& seg2 = *itr2;

                v.visit(polygon1, seg1, polygon2, seg2);
            }
        }
    }

private: 
    Segments<T>& m_segs1;
    Segments<T>& m_segs2;
}; 


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Segment< NS(base)::Coord > baseSegment;
typedef NS(geo)::Segments< NS(base)::Coord > baseSegments;

typedef NS(geo)::SegmentsVisitor< NS(base)::Coord > baseSegmentsVisitor;
typedef NS(geo)::SegmentsEnum< NS(base)::Coord > baseSegmentsEnum;

END_NS(base)

#endif

