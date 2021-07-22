/********************************************************************************

 **** Copyright (C), 2021,                                               ****

 ********************************************************************************
 * File Name     : region.h
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
#ifndef __REGION_H__
#define __REGION_H__
#include<map>
#include"dimension.h"
#include"segments.h"
#include"box.h"
#include"enumerator.h"


BEG_NS(geo)


template<typename T>
class Area {
public:
    typedef STD_NS(pair)<int, Segments<T> > typePolygonSegmentsPair;
    typedef STD_NS(map)<int, Segments<T> > typePolygonSegments;
    typedef typename typePolygonSegments::iterator typePolygonSegmentsItr;
    typedef typename typePolygonSegments::const_iterator typePolygonSegmentsItrConst;
    
public: 
    Area() {}

    typePolygonSegments& getPool() {
        return m_pool;
    }

    const typePolygonSegments& getPool() const {
        return m_pool;
    }


    int getPolygonCnt() const {
        return m_pool.size();
    }

    bool isEmpty() const {
        return m_pool.empty();
    }

    bool hasPolygon(int id) const;

    void clear() {
        m_pool.clear();
    }

    void merge(const Segments<T>& other);
    void merge(const Area<T>& other);
    Area<T>& swap(Area<T>& other);

private:
    Area(const Area<T>&);
    Area<T>& operator=(const Area<T>&);
    
protected:
    typePolygonSegments m_pool;
};


template<typename T>
bool Area<T>::hasPolygon(int id) const {
    typePolygonSegmentsItrConst itr;

    itr = m_pool.find(id);
    if (m_pool.end() != itr) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
void Area<T>::merge(const Segments<T>& other) {
    int id = 0;
    typePolygonSegmentsItr itr;

    id = other.id();
    itr = m_pool.find(id);
    if (m_pool.end() != itr) {
        Segments<T>& segments = itr->second;
    
        segments.merge(other);
    } else {
        m_pool.insert(typePolygonSegmentsPair(id, other));
    }
} 

template<typename T>
void Area<T>::merge(const Area<T>& other) {
    typePolygonSegmentsItrConst itr;

    for (itr = other.m_pool.begin(); itr != other.m_pool.end(); ++itr) {
        merge(itr->second);
    }
}

template<typename T>
Area<T>& Area<T>::swap(Area<T>& other) { 
    m_pool.swap(other.m_pool);

    return *this;
}


template<typename T>
class AreaVisitor {
public:
    virtual ~AreaVisitor() {}
    virtual void visit(Segments<T>& segments) = 0;
};

template<typename T>
class AreaEnum {
public:
    explicit AreaEnum(Area<T>& area) : m_area(area) {} 

    void foreach(AreaVisitor<T>& v) {
        typename Area<T>::typePolygonSegments pool = m_area.getPool();
        typename Area<T>::typePolygonSegmentsItr itr;

        for (itr = pool.begin(); itr != pool.end(); ++itr) {
            Segments<T>& segments = itr->second;

            v.visit(segments);
        }
    }
    
private:
    Area<T>& m_area;
}; 


template<typename T>
class AreaVisitorEx {
public:
    virtual ~AreaVisitorEx() {}
    virtual void visit(Segments<T>& segments1, Segments<T>& segments2) = 0;
};

template<typename T>
class AreaEnumEx {
public:
    explicit AreaEnumEx(Area<T>& area) : m_area(area) {}

    void foreach(AreaVisitorEx<T>& v) {
        typename Area<T>::typePolygonSegments pool = m_area.getPool();
        typename Area<T>::typePolygonSegmentsItr itr1;
        typename Area<T>::typePolygonSegmentsItr itr2;

        for (itr1 = pool.begin(); itr1 != pool.end(); ) {
            Segments<T>& segments1 = itr1->second;
            
            ++itr1; 
            for (itr2 = itr1; itr2 != pool.end(); ++itr2) {
                Segments<T>& segments2 = itr2->second;
                
                v.visit(segments1, segments2);
            }
        }
    }
    
private:
    Area<T>& m_area;
}; 

template<typename T>
class AreaEnumEx2 {
public:
    AreaEnumEx2(Area<T>& area1, Area<T>& area2) : m_area1(area1), m_area2(area2) {}

    void foreach(AreaVisitorEx<T>& v) {
        typename Area<T>::typePolygonSegments pool1 = m_area1.getPool();
        typename Area<T>::typePolygonSegments pool2 = m_area2.getPool();
        typename Area<T>::typePolygonSegmentsItr itr1;
        typename Area<T>::typePolygonSegmentsItr itr2;

        for (itr1 = pool1.begin(); itr1 != pool1.end(); ++itr1) {
            Segments<T>& segments1 = itr1->second;
            
            for (itr2 = pool2.begin(); itr2 != pool2.end(); ++itr2) {
                Segments<T>& segments2 = itr2->second;
                
                v.visit(segments1, segments2);
            }
        }
    }
    
private:
    Area<T>& m_area1;
    Area<T>& m_area2;
}; 


template<typename T>
class Region : public Area<T> { 
public: 
    Region() : m_level(0) {}
    Region(const Dimension<T>& dim, int level) : m_dim(dim), m_level(level) {} 
    
    void set(const Dimension<T>& dim, int level) {
        m_dim = dim;
        m_level = level;
    }
    
    const Dimension<T>& dimension() const {
        return m_dim;
    }

    int level() const {
        return m_level;
    } 
    
private:
    Dimension<T> m_dim;
    int m_level; 
};


template<typename T>
class Border : public Area<T> {
public:
    void setBox(const Box<T>& box) {
        m_box = box;
    }
    
    Box<T>& box() {
        return m_box;
    }

    const Box<T>& box() const {
        return m_box;
    }

private:
    Box<T> m_box;
};


template<typename T>
struct Borders {
    Border<T> m_borders[BOUNDARY_DIR_NUM];
};

template<typename T>
struct Boundary{
    Borders<T> m_boundary[NODE_TREE_CHILD_NUM];
};


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::Area< NS(base)::Coord > baseArea;
typedef NS(geo)::AreaVisitor< NS(base)::Coord > baseAreaVisitor;
typedef NS(geo)::AreaVisitorEx< NS(base)::Coord > baseAreaVisitorEx;

typedef NS(geo)::AreaEnum< NS(base)::Coord > baseAreaEnum;
typedef NS(geo)::AreaEnumEx< NS(base)::Coord > baseAreaEnumEx;

typedef NS(geo)::Region< NS(base)::Coord > baseRegion;
typedef NS(geo)::Border< NS(base)::Coord > baseBorder;

typedef NS(geo)::Borders< NS(base)::Coord > baseBorders;
typedef NS(geo)::Boundary< NS(base)::Coord > baseBoundary;

END_NS(base)


#endif

