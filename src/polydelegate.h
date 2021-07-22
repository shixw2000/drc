#ifndef __POLYDELEGATE_H__
#define __POLYDELEGATE_H__
#include<vector>
#include"globaldef.h"
#include"contour.h"
#include"box.h"
#include"coordtype.h"


BEG_NS(geo)

typedef STD_NS(vector)<int> typeVecIndex;

template<typename T>
class PolygonDelegate { 
public:
    PolygonDelegate() : m_contour(NULL) {} 

    ~PolygonDelegate() {
        m_contour = NULL;
        m_indices.clear();
    }

    bool isEmpty() const {
        return m_indices.empty(); 
    }

    void set(const Contour<T>* contour) {
        m_contour = contour;
        m_indices.clear();
        
        if (0 < m_contour->size()) {
            m_indices.resize(m_contour->size());
            for (int i=0; i<(int)m_indices.size(); ++i) {
                m_indices[i] = i;
            }
        }
    } 

    int vertices() const {
        return (int)m_indices.size();
    }

    const Point<T>& get(int index) const {
        return m_contour->get(m_indices[index]);
    }

    const Point<T>* getP(int index) const {
        return m_contour->getP(m_indices[index]);
    }

    void normalize();
    
    PolygonDelegate<T> normalized() const;

    typeVecIndex compress() const;

    void extractLine();
    void extractLineByCnt();

private:
    explicit PolygonDelegate(const Contour<T>* contour) {
        set(contour);
    }
    
    PolygonDelegate(const Contour<T>* contour, const typeVecIndex& indices) 
        : m_contour(contour), m_indices(indices) {}

    int getPos(int index) const {
        return m_indices[index];
    }

    /* forbidden functions */
    PolygonDelegate(const PolygonDelegate&);
    PolygonDelegate& operator=(const PolygonDelegate&);
    
private:
    const Contour<T>* m_contour;
    typeVecIndex m_indices;
};


/* a polygon should have more than or eq 3 points */
template<typename T>
typeVecIndex PolygonDelegate<T>::compress() const {
    typeVecIndex indices;
    int size = vertices();
    int next = 0;
    int last = 0;
    int curr = 0;
    int index = 0;
    const Point<T>* plast = NULL;
    const Point<T>* pcurr = NULL;
    const Point<T>* pnext = NULL;
    const Point<T>* pts = NULL;

    /* a polygon must have at least 3 points */
    if (3 <= size) { 
        pcurr = getP(0);

        /* get a next different point */
        next = 0;
        while (++next < size) {
            pnext = getP(next);
            if (!NS(geo)::equal(*pcurr, *pnext)) {
                break;
            }
        } 

        /* check a last different point */
        pts = pcurr; 
        index = getPos(0);
        
        last = size;
        while (--last > next) {
            plast = getP(last);
            
            if (!NS(geo)::equal(*pts, *plast)) { 
                if (!NS(geo)::isColinear(*plast, *pcurr, *pnext, true)) { 
                    break;
                } 

                pts = plast;
                index = getPos(last);
            } 
        }

        /* has more points */
        if (last > next) { 
            indices.reserve(size); 
            
            indices.push_back(index); 
        
            plast = pts; 
            pcurr = pnext;
            curr = next;
            
            pts = pcurr;
            index = getPos(curr);
            for (int i=curr; i<=last; ++i) {
                next = i + 1;
                if (next == size) {
                    next = 0;
                }
                
                pnext = getP(next);

                if (!NS(geo)::equal(*pts, *pnext)) {
                    if (!NS(geo)::isColinear(*plast, *pcurr, *pnext, true)) {
                        indices.push_back(index);

                        plast = pts;
                        pcurr = pnext;
                    }

                    pts = pnext;
                    index = getPos(next);
                } 
            }
        }
        
        if (3 > (int)indices.size()) {
            indices.clear();
        }
    } 

    return indices;
}

template<typename T>
void PolygonDelegate<T>::extractLine() {
    typeVecIndex indices;
    int size = vertices();
    int index = 0;
    bool bHold1 = false;
    bool bHold2 = false;
    const Point<T>* pcurr = NULL;
    const Point<T>* pnext = NULL;

    if (3 <= size) {
        pcurr = getP(size - 2);
        pnext = getP(size - 1);

        if (NS(base)::MIN_LINE_LENGTH_SQUARE > sq_length(*pcurr, *pnext)) {
            bHold1 = false;
        } else {
            bHold1 = true;
        }

        pcurr = pnext;
        index = getPos(size - 1);
        for (int i=0; i<size; ++i) {
            pnext = getP(i);
            
            if (NS(base)::MIN_LINE_LENGTH_SQUARE > sq_length(*pcurr, *pnext)) {
                bHold2 = false;
            } else {
                bHold2 = true;
            }

            if (bHold1 || bHold2) {
                indices.push_back(index);
            } 

            pcurr = pnext;
            index = getPos(i);
            bHold1 = bHold2;
        }

        if (3 > (int)indices.size()) {
            indices.clear();
        }
    }

    m_indices.swap(indices);
}

template<typename T>
void PolygonDelegate<T>::extractLineByCnt() {
    typeVecIndex indices;
    int size = vertices();
    int curr = 0;
    int next = 0;
    int cnt = 0;
    bool bHold1 = false;
    bool bHold2 = false;

    if (3 <= size) {
        curr = getPos(size - 2);
        next = getPos(size - 1);
        if (next > curr) {
            cnt = next - curr;
        } else {
            cnt = m_contour->size() + next - curr;
        }

        if (NS(base)::MIN_LINE_POINT_CNT > cnt) {
            bHold1 = false;
        } else {
            bHold1 = true;
        }

        curr = next;
        for (int i=0; i<size; ++i) {
            next = getPos(i);
            
            if (next > curr) {
                cnt = next - curr;
            } else {
                cnt = m_contour->size() + next - curr;
            }
            
            if (NS(base)::MIN_LINE_POINT_CNT > cnt) {
                bHold2 = false;
            } else {
                bHold2 = true;
            }

            if (bHold1 || bHold2) {
                indices.push_back(curr);
            } 

            curr = next;
            bHold1 = bHold2;
        }

        if (3 > (int)indices.size()) {
            indices.clear();
        }
    }

    m_indices.swap(indices);
}

template<typename T>
void PolygonDelegate<T>::normalize() {
    typeVecIndex indices;

    indices = compress();
    m_indices.swap(indices);
}

template<typename T>
PolygonDelegate<T> PolygonDelegate<T>::normalized() const {
    typeVecIndex indices;

    indices = compress();
    return PolygonDelegate<T>(m_contour, indices);
}


END_NS(geo)


BEG_NS(base)

typedef NS(geo)::PolygonDelegate< NS(base)::Coord > basePolygonDelegate;

END_NS(base)

#endif

