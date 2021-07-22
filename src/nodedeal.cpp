#include"nodedeal.h"
#include"datanode.h"
#include"polygon.h"
#include"segments.h"
#include"dimension.h"
#include"box.h"
#include"point.h"


BEG_NS(alg)

template<typename T>
void NodeDeal<T>::box2Dim(typeDimension& dim, const typeBox& box) {
    coord_type x = 0;
    coord_type y  = 0;
    coord_type halfAxis = 0;
    typePoint p(box.p1());

    x = box.width();
    y = box.height();
    halfAxis = (x > y ? x : y) / 2;
    p.move(halfAxis, halfAxis);
    dim.setOrig(p);
    dim.setWidth(halfAxis); 
    return;
}

template<typename T>
typename NodeDeal<T>::typeNode* NodeDeal<T>::createRootNode(const typeDimension& dim, const typePolygon* polygons, int size) {
    typeNode* ptr = NULL;
    typeSegments segments;
    typeSegment segpair;

    ptr = new typeNode;
    ptr->m_order = 0;
    ptr->m_status = NODE_STATUS_INIT;
    ptr->m_children = NULL;
    ptr->m_region.set(dim, 0); 
    
    for (int i=0; i<size; ++i) { 
        if (0 < polygons[i].vertices()) {
            segpair.reset();
            segpair.setfrom(0);
            segpair.setto(polygons[i].vertices() - 1);

            segments.reset();
            segments.setPolygon(&polygons[i]); 
            segments.merge(segpair);

            ptr->m_region.merge(segments);
        }
    }
    
    return ptr;
}

template class NodeDeal<NS(base)::Coord>;


END_NS(alg)

