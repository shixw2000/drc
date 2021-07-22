#include"opcspacerule.h"
#include"segdelegate.h"


BEG_NS(alg)


void OpcSpaceStrategy::process(int type, typeNodePtr node, const typePolygon* polygon, typeSegment* segment) { 
    switch (type) {
    case 3:
        partitionSegment(node, *segment, polygon);
        break;

    default:
        break;
    }
}

void OpcSpaceStrategy::processEx(int type, typeNode* node, const typePolygon* polygon1, typeSegment* segment1,
        const typePolygon* polygon2, typeSegment* segment2) { 
    switch (type) {
    case 4:
        calcSegmentDist(*segment1, polygon1, *segment2, polygon2, *m_ret);
        break;

    default:
        break;
    }
}


/*****************************************************************************
 * Function      : NodeTree.partition
 * Description   : transfer all of segments to subnodes by location
 * Input         : typeNodePtr node          
                const typeSegments& segs  
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void OpcSpaceStrategy::partitionSegment(typeNodePtr node, typeSegment& segment, const typePolygon* polygon) {
    int lastArea = -1;
    int curArea = -1;
    const typePoint& orig = node->m_region.dimension().origin();
    typeSegments tmpSegs[NODE_TREE_CHILD_NUM];
    typeSegment tmpPair;
    
    for (int i=segment.from(); i<=segment.to(); ++i) {
        const typePoint& point = polygon->get(i);

        curArea = orig.locate(point);
        if (lastArea == curArea) { 
            tmpPair.setto(i);
        } else if (-1 == lastArea) {
            tmpPair.reset();
            tmpPair.setfrom(i);
            
            lastArea = curArea;
        } else {
            tmpSegs[lastArea].merge(tmpPair); 
            
            tmpPair.reset();
            tmpPair.setfrom(i);                

            lastArea = curArea;
        } 
    }

    /* add remained pairs */
    if (!tmpPair.isEmpty()) {
        tmpSegs[lastArea].merge(tmpPair); 
        tmpPair.reset();
    } 

    for (int i=0; i<NODE_TREE_CHILD_NUM; ++i) {
        if (!tmpSegs[i].isEmpty()) { 
            tmpSegs[i].setPolygon(polygon);
            node->m_children[i].m_region.merge(tmpSegs[i]);
        }
    } 
}


void OpcSpaceStrategy::calcSegmentDist(const typeSegment& segment_src, const typePolygon* polygon_src, 
    const typeSegment& segment_dst, const typePolygon* polygon_dst, 
    typeChkResult& ret) {
    bool needChg = false;
    coord_type xy_len = 0;
    area_type sq_distance = 0;
    typeItemResult result;
    typeChkResult::iterator itr;
    const typePolygon* polygon1 = NULL;
    const typePolygon* polygon2 = NULL;
    const typeSegment* segment1 = NULL;
    const typeSegment* segment2 = NULL;
    
    /* 注意排序，使polygon和segments一致 */
    if (polygon_src->id() < polygon_dst->id()) {
        polygon1 = polygon_src;
        polygon2 = polygon_dst;

        segment1 = &segment_src;
        segment2 = &segment_dst;
    } else {
        polygon1 = polygon_dst;
        polygon2 = polygon_src;

        segment1 = &segment_dst;
        segment2 = &segment_src;
    }

    result.m_polygon1 = polygon1;
    result.m_polygon2 = polygon2;
    result.m_p1 = NULL;
    result.m_p2 = NULL;
    
    /* if already input, then get last distance */
    itr = ret.find(result);
    if (ret.end() != itr) {
        result.m_sq_distance = (*itr).m_sq_distance;
    } else {
        result.m_sq_distance = m_sq_distance; //init user defined value
    } 

    /* 遍历子片段1上的点 */
    for (int i=segment1->from(); i<=segment1->to(); ++i) {
        const typePoint* p1 = polygon1->getP(i);

        /* 遍历片段2上的点 */
        for (int j=segment2->from(); j<=segment2->to(); ++j) {
            const typePoint* p2 = polygon2->getP(j);

            /* 先检查两点的xy最大距离 */
            xy_len = NS(geo)::max_xy_length(*p1, *p2);
            if (!(m_distance < xy_len)) {
                sq_distance = NS(geo)::sq_length(*p1, *p2);
                if (!(result.m_sq_distance < sq_distance)) {
                    result.m_sq_distance = sq_distance;

                    result.m_p1 = p1;
                    result.m_index1 = i;
                    result.m_p2 = p2;
                    result.m_index2 = j;
                    
                    needChg = true;
                }
            }
        }
    }

    if (needChg) { 
        if (ret.end() == itr) {
            ret.insert(result);
        } else {
            ret.erase(itr);
            ret.insert(result);
        }
    }
}


void OpcSpaceStrategy::buildTreeNode(typeNodePtr node) {
    typeAreaEnum enumerator(node->m_region);
    typeAreaHandler visitor(3, node, this); 

    enumerator.foreach(visitor);
}

void OpcSpaceStrategy::chkLeafNode(typeNodePtr node) {
    typeAreaEnumEx enumerator(node->m_region);
    typeAreaHandlerEx visitor(4, node, this);

    enumerator.foreach(visitor);
}

void OpcSpaceStrategy::chkInterNode(typeNodePtr node) {
    typeAreaEnumEx enumerator(node->m_region);
    typeAreaHandlerEx visitor(4, node, this);

    enumerator.foreach(visitor);
}

END_NS(alg) 

