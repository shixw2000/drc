#include<math.h>
#include"coordtype.h"
#include"enumerator.h"
#include"nodetree.h"


BEG_NS(alg)


static const NodeTree::typePoint g_dirPoints[NODE_TREE_CHILD_NUM] = {
    NodeTree::typePoint(-1, -1),
    NodeTree::typePoint(1, -1),
    NodeTree::typePoint(-1, 1),
    NodeTree::typePoint(1, 1)
};
    
NodeTree::NodeTree() : m_root(NULL) {
    m_minPolygonCnt = AREA_CONTAIN_MIN_POLYGON;
    m_minWidth = AREA_DELIMIT_MIN_WIDTH;
    m_distance = 0;
    m_sq_distance = 0;
    m_maxLevel = 0;
    m_maxUsedLevel = 0;
    m_axisLen = 0;
    m_usedWidth = 0; 
}

NodeTree::~NodeTree() {
}

NodeTree::typeNodePtr NodeTree::createRootNode(const typeDimension& dim,
    const typePolygon* polygons, int size) {
    typeNodePtr ptr = NULL;
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

NodeTree::typeNodePtr NodeTree::createChildNode(const typeDimension& dim, int level) {
    typeNodePtr ptr = NULL;
    typePoint point;
    typePoint offsetPoint;
    value_type halfAxis;
    typeDimension subDim;

    point = dim.origin();    

    /* subdimension params */
    ++level;
    halfAxis = dim.width() / 2;
    subDim.setWidth(halfAxis);

    offsetPoint.set_x(halfAxis);
    offsetPoint.set_y(halfAxis);

    if (m_maxUsedLevel < level) {
        m_maxUsedLevel = level;
    }
    
    ptr = new typeNode[NODE_TREE_CHILD_NUM];
    for (int i=0; i<NODE_TREE_CHILD_NUM; ++i) {
        ptr[i].m_order = i;
        ptr[i].m_status = NODE_STATUS_INIT;
        ptr[i].m_children = NULL;

        
        subDim.setOrig(point.addP(g_dirPoints[i].multP(offsetPoint)));
        ptr[i].m_region.set(subDim, level);
    } 

    /* 保存所有的子节点链表 */
    m_children_lists.push_back(ptr);
  
    return ptr;
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
void NodeTree::partitionSegment(typeNodePtr node, typeSegment& segment, const typePolygon* polygon) {
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

/*****************************************************************************
 * Function      : NodeTree.procSpaceRule
 * Description   : 执行多边形间距检查入口
 * Input         : const typeBox* area          
                const typePolygon* polygons  
                int size                     
                value_type distance          
                typeChkResult& ret           
 * Output        : None
 * Return        : int
 * Others        : 
 * Record
 * 1.Date        : 20210427
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
int NodeTree::procSpaceRule(const typeBox* area, const typePolygon* polygons, int size, 
    value_type distance, typeChkResult& ret) {
    int minPolygon = AREA_CONTAIN_MIN_POLYGON;
    int maxDelimitCnt = AREA_MAX_DELIMIT_TIMES;
    int level = 0;
    value_type minWidth = AREA_DELIMIT_MIN_WIDTH;
    value_type delimitWidth = 0;
    value_type maxSize = 0; 

    /* 输入间距小于0,重置为绝对值 */
    if (0 > distance) {
        distance = fabs(distance);
    }

    /* 建议最小宽度不小于2倍的间距 */
    if (NS(base)::coord_traits<double>::less(AREA_MIN_WIDTH_TIMES_DISTANCE * distance, minWidth)) {
        delimitWidth = minWidth;
    } else {
        delimitWidth = AREA_MIN_WIDTH_TIMES_DISTANCE * distance;
    }

    maxSize = area->height() <  area->width() ? area->width() : area->height();
    if (maxSize > delimitWidth) {
        level = (int)ceil(log2((double)maxSize / delimitWidth));
    } else {
        level = 0;
    }
    
    if (level <= maxDelimitCnt) {
        minWidth = delimitWidth;
    } else {
        minWidth = delimitWidth * (1 << (level-maxDelimitCnt));
    }

    buildTree(area, polygons, size, minPolygon, minWidth);
    checkOpcRule(distance, ret); 
    deleteTree();

    return 0;
}

/*****************************************************************************
 * Function      : NodeTree.buildTree
 * Description   : build the root node by input params
 * Input         : const typeDimension* dim     
                const typePolygon* polygons  
                int size                     
                int minPolygonCnt            
                value_type minWidth          
                value_type distance          
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::buildTree(const typeBox* box, const typePolygon* polygons, int size,
    int minPolygonCnt, value_type minWidth) {
    value_type halfAxis = 0;
    typePoint po;
    typeListNodes lists;
    typeNodePtr head = NULL; 

    if (0 < minPolygonCnt) {
        m_minPolygonCnt = minPolygonCnt;
    }

    if (0 < minWidth) {
        m_minWidth = minWidth;
    }

    m_box = *box;
    m_usedWidth = m_box.height() <  m_box.width() ? m_box.width() : m_box.height();
    if (m_usedWidth > m_minWidth) { 
        m_maxLevel = (int)ceil(log2((double)m_usedWidth/ m_minWidth));
    } else {
        m_maxLevel = 0;
    }
    
    m_maxUsedLevel = 0;

    m_axisLen = m_minWidth * (1 << m_maxLevel);
    halfAxis = m_axisLen / 2;
    
    po = m_box.p1().moveP(halfAxis, halfAxis);
    m_dim.setOrig(po);
    m_dim.setWidth(halfAxis); 
    
    m_root = createRootNode(m_dim, polygons, size);
    if (needPartition(m_root)) { 
        lists.push_back(m_root); 
    }
        
    while (!lists.empty()) {
        head = lists.front();

        lists.pop_front();
        dealNode(head, lists);
    }

    LOG_INFO("build_tree| min_polygon=%d| min_width=%.2lf|"
        " area_width=%.2lf| area_height=%.2lf| area_used_width=%.2lf| max_axis_len=%.2lf|"
        " max_level=%d| max_used_level=%d|",
        m_minPolygonCnt, m_minWidth, 
        m_box.width(), m_box.height(), m_usedWidth, m_axisLen,
        m_maxLevel, m_maxUsedLevel);
}

bool NodeTree::needPartition(typeNodePtr node) const {
    if (node->m_region.level() < m_maxLevel && node->m_region.getPolygonCnt() > m_minPolygonCnt) { 
        return true; 
    } else {
        return false;
    }
}

void NodeTree::foreachChild(typeNodeFunc func) {
    typeNodePtr ptr = NULL;

    for (typeListNodes::iterator itr=m_children_lists.begin(); itr!=m_children_lists.end(); ++itr) {
        ptr = *itr;
        (this->*func)(ptr);
    }
}

void NodeTree::deleteChildren(typeNodePtr ptr) {
    if (NULL != ptr) {
        delete[] ptr;
    }
}

void NodeTree::foreachChild(typeNodeFuncConst func) const {
    const typeNode* ptr = NULL;

    for (typeListNodes::const_iterator itr=m_children_lists.begin(); itr!=m_children_lists.end(); ++itr) {
        ptr = *itr;
        (this->*func)(ptr);
    }
}

void NodeTree::deleteTree() {
    if (NULL != m_root) {
        delete m_root;
        m_root = NULL;

        foreachChild(&NodeTree::deleteChildren);

        m_children_lists.clear();
    }
}

void NodeTree::dispResult(const typeChkResult& ret) const {
    int cnt = 0;
    typeChkResult::const_iterator itr;

    LOG_INFO("dispResult| min_polygon=%d| min_width=%.2lf|"
        " used_width=%.2lf| axis_len=%.2lf| max_level=%d| used_level=%d|"
        " distance=%.2lf| sq_distance=%.2lf| size=%d", 
        m_minPolygonCnt, m_minWidth, 
        m_usedWidth, m_axisLen, m_maxLevel, m_maxUsedLevel,
        m_distance, m_sq_distance, (int)ret.size());
    
    for (itr = ret.begin(); itr != ret.end(); ++itr) {
        const typeItemResult& item = *itr;

        LOG_INFO("[%d]: polygon1=%d| point1[%d]=<%.2lf, %.2lf>|"
            " polygon2=%d| point2[%d]=<%.2lf, %.2lf>|"
            " sq_distance=%.2lf|",
            cnt++, 
            item.m_polygon1->id(), item.m_index1, item.m_p1->x(), item.m_p1->y(),
            item.m_polygon2->id(), item.m_index2, item.m_p2->x(), item.m_p2->y(),
            item.m_sq_distance);
    }
}

/*****************************************************************************
 * Function      : NodeTree.dealNode
 * Description   : check and partition a node to small subnodes
 * Input         : typeNodePtr node  
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::dealNode(typeNodePtr node, typeListNodes& lists) {
    typeAreaEnum enumerator(node->m_region);
    AreaPartition<value_type> visitor(*this, *node); 
    
    /* need to partition to small ones, then create four subnodes here */
    node->m_children = createChildNode(node->m_region.dimension(), node->m_region.level()); 

    enumerator.foreach(visitor);

    /* just queue large regions needed to be partition */
    for (int i=0; i<NODE_TREE_CHILD_NUM; ++i) { 
        if (needPartition(&node->m_children[i])) {
            lists.push_back(&node->m_children[i]);
        }
    }

    /* 清除中间节点保存的segments */
    node->m_region.clear();
}


/*****************************************************************************
 * Function      : NodeTree.checkOpcRule
 * Description   : search polygons in the QuadTree
 * Input         : typeChkResult ret  
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::checkOpcRule(value_type distance, typeChkResult& ret) {
    typeListNodes lists;
    typeNodePtr head = NULL; 
    typeBoundary* boundary = NULL;

    /*the min space of two polygons */
    m_distance = distance;
    m_sq_distance = m_distance * m_distance;
    
    boundary = new typeBoundary[m_maxUsedLevel + 1]; 

    lists.push_front(m_root);

    while (!lists.empty()) {
        head = lists.front();

        if (NODE_STATUS_DONE == head->m_status) {
            lists.pop_front();
        } else {
            checkNode(head, lists, boundary, ret);
        }
    }

    delete[] boundary;
}

/*****************************************************************************
 * Function      : NodeTree.checkNode
 * Description   : just allow to push nodes to list
 * Input         : typeNodePtr node      
                typeListNodes& lists  
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210421
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::checkNode(typeNodePtr node, typeListNodes& lists, typeBoundary* boundary, typeChkResult& ret) {

    if (NODE_STATUS_INIT == node->m_status) {
        if (NULL == node->m_children) {
            searchLeaf(node, boundary, ret); 

            /* complete by setting end status for the leaf */
            node->m_status = NODE_STATUS_DONE;
        } else {
            for (int i=NODE_TREE_CHILD_NUM-1; i>=0; --i) {
                lists.push_front(&node->m_children[i]);
            } 

            /* continue the internode by setting doing status */
            node->m_status = NODE_STATUS_DOING;
        }
    } else {
        /* check boundaries of the four internodes */
        searchInterNode(node, boundary, ret); 

        /* complete by setting end status for internode */
        node->m_status = NODE_STATUS_DONE;
    }
}

void NodeTree::searchInterNode(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret) {

    searchBoundary(node, boundary, ret);
    
    /* get boundary of the node */
    mergeBoundary(node, boundary);
}

void NodeTree::searchLeaf(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret) {
    int order = node->m_order;
    int level = node->m_region.level();
    
    /* only dealing leaf contains datas */
    if (!node->m_region.isEmpty()) {
        typeAreaEnumEx enumerator(node->m_region);
        NodeChk<value_type> visitor(*this, ret);

        enumerator.foreach(visitor);
    
        /* get boundary of the leaf */
        getBoundary(node, boundary[level].m_boundary[order]);
    }
}

void NodeTree::calcSegmentDist(const typeSegment& segment_src, const typePolygon* polygon_src, 
    const typeSegment& segment_dst, const typePolygon* polygon_dst, 
    typeChkResult& ret) {
    bool needChg = false;
    value_type xy_len = 0;
    value_type sq_distance = 0;
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

void NodeTree::segment2Boundaries(const typeSegment& segment, 
    const typePolygon* polygon, typeBorders& borders) {
    for (int i=0; i<BOUNDARY_DIR_NUM; ++i) {
        segment2Boundary(segment, polygon, borders.m_borders[i]);
    }
}
    
void NodeTree::segment2Boundary(const typeSegment& segment, const typePolygon* polygon, typeBorder& border) {
    typeSegments tmpSegs;
    typeSegment tmpPair; 
    
    for (int i=segment.from(); i<=segment.to(); ++i) {
        const typePoint& point = polygon->get(i);
        
        if (border.box().contain(point)) {
            tmpPair.setto(i);
        } else {
            if (!tmpPair.isEmpty()) {
                tmpSegs.merge(tmpPair);
                tmpPair.reset();
            }
        }
    }

    if (!tmpPair.isEmpty()) { 
        tmpSegs.merge(tmpPair);
        tmpPair.reset();
    }

    if (!tmpSegs.isEmpty()) {
        tmpSegs.setPolygon(polygon);
        
        border.merge(tmpSegs);
        tmpSegs.reset();
    }
}

void NodeTree::getBoundary(typeNodePtr node, typeBorders& borders) {
    const typeDimension& dim = node->m_region.dimension();   
    typeAreaEnum enumerator(node->m_region);
    AreaBoundary<value_type> visitor(*this, borders);
    
    getBoundaryFromDim(dim, borders); 

    enumerator.foreach(visitor); 
}

/*****************************************************************************
 * Function      : NodeTree.getBoundaryFromDim
 * Description   : 获取矩形的四个矩形边界=========================
 * Input         : const typeDimension& dim  
                typeBorders& borders      
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210422
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::getBoundaryFromDim(const typeDimension& dim, typeBorders& borders) {
    value_type w = dim.width();
    value_type h = m_distance;
    typePoint orig = dim.origin();
    typeBox box; 
    
    box.set_p1(orig.moveP(-w, -w));
    box.set_p2(orig.moveP(w, -w+h));
    borders.m_borders[0].setBox(box);

    box.set_p1(orig.moveP(w-h, -w));
    box.set_p2(orig.moveP(w, w));
    borders.m_borders[1].setBox(box);

    box.set_p1(orig.moveP(-w, -w));
    box.set_p2(orig.moveP(-w + h, w));
    borders.m_borders[2].setBox(box);

    box.set_p1(orig.moveP(-w, w - h));
    box.set_p2(orig.moveP(w, w));
    borders.m_borders[3].setBox(box); 
    
    borders.m_borders[0].clear();
    borders.m_borders[1].clear();
    borders.m_borders[2].clear();
    borders.m_borders[3].clear();
}

void NodeTree::mergeBoundary(typeNodePtr node, typeBoundary* boundary) {
    int order = node->m_order;
    int level = node->m_region.level();
    typeBorders* pself = NULL;
    typeBoundary* pchild = NULL;

    pself = &boundary[level].m_boundary[order];
    pchild = &boundary[level+1];

    pself->m_borders[0].swap(pchild->m_boundary[0].m_borders[0]).merge(pchild->m_boundary[1].m_borders[0]);
    pself->m_borders[1].swap(pchild->m_boundary[1].m_borders[1]).merge(pchild->m_boundary[3].m_borders[1]);
    pself->m_borders[2].swap(pchild->m_boundary[0].m_borders[2]).merge(pchild->m_boundary[2].m_borders[2]);
    pself->m_borders[3].swap(pchild->m_boundary[2].m_borders[3]).merge(pchild->m_boundary[3].m_borders[3]);

    /* clear all child data */
    for (int i=0; i<NODE_TREE_CHILD_NUM; ++i) {
        for (int j=0; j<BOUNDARY_DIR_NUM; ++j) {
            pchild->m_boundary[i].m_borders[j].clear();
        }
    }
}

void NodeTree::searchBoundary(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret) {
    int level = node->m_region.level();
    typeBoundary* pb = NULL; // four child boundary
    
    pb = &boundary[level+1];

    /***=======采用倒L型，注意不要遗漏***/
    getBoundaryViolation(pb->m_boundary[0].m_borders[1], pb->m_boundary[1].m_borders[2], ret);
    getBoundaryViolation(pb->m_boundary[0].m_borders[1], pb->m_boundary[2].m_borders[0], ret);
    getBoundaryViolation(pb->m_boundary[0].m_borders[1], pb->m_boundary[3].m_borders[0], ret);
    
    /* 中心角部分已经比较过了*/
    getBoundaryViolation(pb->m_boundary[0].m_borders[3], pb->m_boundary[2].m_borders[0], ret);


    getBoundaryViolation(pb->m_boundary[1].m_borders[3], pb->m_boundary[2].m_borders[0], ret);
    getBoundaryViolation(pb->m_boundary[1].m_borders[3], pb->m_boundary[3].m_borders[0], ret);

    
    getBoundaryViolation(pb->m_boundary[2].m_borders[1], pb->m_boundary[3].m_borders[2], ret);
}


/*****************************************************************************
 * Function      : NodeTree.getBoundaryViolation
 * Description   : 检查相邻边界中不同polygon子片段的最小间距
                   
 * Input         : const typeArea& area1  
                const typeArea& area2  
                typeChkResult& ret     
 * Output        : None
 * Return        : void
 * Others        : 
 * Record
 * 1.Date        : 20210422
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
void NodeTree::getBoundaryViolation(typeArea& area1, typeArea& area2, typeChkResult& ret) { 
    if (area1.isEmpty() || area2.isEmpty()) {
        return;
    }

    typeAreaEnumEx2 enumerator(area1, area2);
    NodeChk<value_type> visitor(*this, ret);

    enumerator.foreach(visitor); 
}


END_NS(alg)


