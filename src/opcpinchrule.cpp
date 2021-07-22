#include"opcpinchrule.h"
#include"edgefilter.h"
#include"segdelegate.h"
#include"writer.h"


BEG_NS(alg)

OpcPinchStrategy::OpcPinchStrategy() {
    m_root = NULL;
    m_result.m_ret = NULL;
    m_conditon.m_cond = NULL;
}

OpcPinchStrategy::~OpcPinchStrategy() {
    finish();
}

void OpcPinchStrategy::finish() {
    if (NULL != m_root) {
        delete m_root;
        m_root = NULL;
    }

    m_result.m_ret = NULL;
    m_conditon.m_cond = NULL;
}

void OpcPinchStrategy::process(int type, typeNodePtr node, const typePolygon* polygon, typeSegment* segment) { 
    distance_type distance = 0;
    typePolyDistanceItrConst itr;
    
    switch (type) {
    case 1:
        distance = *m_conditon.m_distance;
        chkPolygonWholePinch(distance, node, polygon, segment, m_result.m_retPinch);
        break;

    case 2:
        calcPolygonWholeCD(node, polygon, segment, m_result.m_retCD);
        break;

    case 3:
        itr = m_conditon.m_polyDistance->find(polygon->id());
        if (m_conditon.m_polyDistance->end() != itr) {
            distance = itr->second;
            chkPolygonWholePinch(distance, node, polygon, segment, m_result.m_retPinch);
        }
        
        break;

    default:
        break;
    }
}

int OpcPinchStrategy::procPinchRule(const typeBox* area, const typePolygon* polygons, int size, 
    distance_type distance, typePinchResult& result) {
    int ret = 0;
    
    m_area = *area;
    box2Dim(m_dim, m_area);
    
    m_root = createRootNode(m_dim, polygons, size);
    m_result.m_retPinch = &result; 
    m_conditon.m_distance = &distance;

    LOG_INFO("====Input_params| min_x=%.2lf| min_y=%.2lf| area_width=%.2lf| area_height=%.2lf|"
        " polygon_size=%d| distance=%.2lf|",
        m_area.x1(), m_area.y1(),
        m_area.width(), m_area.height(), size, distance); 
    
    ret = dealNode(1, m_root);

    finish();
    return ret;
}

int OpcPinchStrategy::procPinchRuleEx(const typeBox* area, const typePolygon* polygons, int size, 
    const typePolyDistance& polyDistance, typePinchResult& result) {
    int ret = 0;
    
    m_area = *area;
    box2Dim(m_dim, m_area);
    
    m_root = createRootNode(m_dim, polygons, size);
    m_result.m_retPinch = &result; 
    m_conditon.m_polyDistance = &polyDistance;

    LOG_INFO("====Input_params| min_x=%.2lf| min_y=%.2lf| area_width=%.2lf| area_height=%.2lf|"
        " polygon_size=%d|",
        m_area.x1(), m_area.y1(),
        m_area.width(), m_area.height(), size); 
    
    ret = dealNode(3, m_root);

    finish();
    return ret;
}

int OpcPinchStrategy::dealNode(int type, typeNodePtr node) {
    if (!node->m_region.isEmpty()) {
        NS(base)::baseAreaEnum enumerator(node->m_region);
        typeAreaHandler visitor(type, node, this);

        /* 调用process函数处理polygon片段 */
        enumerator.foreach(visitor); 
    }   

    return 0;
}

void OpcPinchStrategy::chkPolygonWholePinch(distance_type minDistance, typeNodePtr node, 
    const typePolygon* polygon, typeSegment* segment, typePinchResult* result) {
    bool bFound = false;
    bool bIn = false;
    distance_type distance = 0;
    StuPinchItem item;
    typeEdge edg1;
    typeEdge edg2;
    typeSegmentEdgeIterator itr1(polygon, segment);
    EdgeRelationFilter filter(minDistance);

    item.m_polygon = polygon;
    item.m_distance = minDistance + 1; // safety
    
    while (!itr1.atEnd()) {
        edg1 = *itr1;
        ++itr1;

        /* ignore too shorter lines */
        if (NS(base)::MIN_LINE_LENGTH > edg1.double_length()) {
            continue;
        }

        typeSegmentEdgeIterator itr2(itr1);

        while (!itr2.atEnd()) {
            edg2 = *itr2;
            ++itr2;

            /* ignore too shorter lines */
            if (NS(base)::MIN_LINE_LENGTH > edg2.double_length()) {
                continue;
            }

            bIn = filter.check(edg1, edg2, &distance);
            if (bIn && !(item.m_distance < distance)) {
                item.m_distance = distance;
                item.m_edg1 = edg1;
                item.m_edg2 = edg2;

                if (!bFound) {
                    bFound = true;
                }
            }
        } 
    } 

    if (bFound) {
        result->push_back(item);
    }
}

int OpcPinchStrategy::procCDRule(const typeBox* area, const typePolygon* polygons, int size, 
    const STD_NS(vector)<typeEdge>& edges, typeCDResult& result) {
    int ret = 0;
    StuRetCD stuItem;

    result.clear();
    if (edges.empty()) {
        return 0;
    }
    
    m_area = *area;
    box2Dim(m_dim, m_area);
    
    m_root = createRootNode(m_dim, polygons, size);
  
    stuItem.m_cds.clear();
    for (int i=0; i<(int)edges.size(); ++i) {
        stuItem.m_edge = edges[i];
        
        result.push_back(stuItem);
    }

    m_result.m_retCD = &result; 

    LOG_INFO("====Input_params| min_x=%.2lf| min_y=%.2lf| area_width=%.2lf| area_height=%.2lf|"
        " polygon_size=%d| edges_size=%d|",
        m_area.x1(), m_area.y1(),
        m_area.width(), m_area.height(), size, 
        (int)result.size()); 

    ret = dealNode(2, m_root);

    finish();
    return ret;
}

void OpcPinchStrategy::calcPolygonWholeCD(typeNodePtr node, const typePolygon* polygon, 
    typeSegment* segment, typeCDResult* result) {
    typeBox box = polygon->box();
    
    for (typeCDResult::iterator itr=result->begin(); itr!=result->end(); ++itr) {
        StuRetCD& item = *itr;
        const typeEdge& edge = item.m_edge;

        /* edge is out of polygon */
        if (!box.touches(edge.box())) {
            continue;
        }

        calcPolygonCD(edge, node, polygon, segment, item.m_cds);
    }
}

void OpcPinchStrategy::calcPolygonCD(const typeEdge& edge, typeNodePtr node, const typePolygon* polygon, 
    typeSegment* segment, typeCDItems& result) {
    typeEdge other;
    STD_NS(pair)<bool, typePoint> curItem;
    typeBox box = edge.box();
    StuCDItem item;
    typeVecPoints pts;
    typeSegmentEdgeIterator itr(polygon, segment);

    while (!itr.atEnd()) {
        other = *itr;
        ++itr;

        if (!box.touches(other.box())) {
            continue;
        }

        curItem = edge.intersect_point(other);
        if (curItem.first) {
            pts.push_back(curItem.second);
        }
    }

    if (1 < (int)pts.size()) {
        unique(pts);
    }

    if (2 == (int)pts.size()) {
        item.m_polygon = polygon;
        item.m_point1 = pts[0];
        item.m_point2 = pts[1];
        item.m_length = NS(geo)::length1(item.m_point1, item.m_point2);

        result.push_back(item);
    } else if (0 < (int)pts.size()) {
        LOG_INFO("!!!!calc_CD| polygon=%d| edge=<%.2lf, %.2lf>-<%.2lf, %.2lf>| cutpoint_size=%d|",
            polygon->id(), 
            edge.x1(), edge.y1(), edge.x2(), edge.y2(),
            (int)pts.size());

        /* print unexpected points to file log, marked with distance == -1 */
        FILE_LOG("%d %d", polygon->id(), -1);
        
        for (int i=0; i<(int)pts.size(); ++i) {
            LOG_INFO("[%d]<%.2lf, %.2lf>", i, pts[i].x(), pts[i].y());

            FILE_LOG(" %.2lf %.2lf", pts[i].x(), pts[i].y());
        }

        FILE_LOG("\n");
    }
} 

void OpcPinchStrategy::unique(typeVecPoints& pts) {
    int cnt = 0;
    
    if (1 < (int)pts.size()) {
        /* sort by x */
        for (int i=1; i<(int)pts.size(); ++i) {
            for (int j=i-1; j>=0; --j) {
                if (less(pts[j+1], pts[j])) {
                    STD_NS(swap)(pts[j], pts[j+1]);
                } else {
                    break;
                }
            }
        }

        /* delete the overlap points */
        cnt = 1;
        for (int i=1; i<(int)pts.size(); ++i) {
            if (!NS(geo)::isOverlap(pts[i-1], pts[i])) {
                if (cnt != i) {
                    pts[cnt] = pts[i];
                }

                ++cnt;
            }
        }

        if (cnt < (int)pts.size()) {
            pts.erase(pts.begin() + cnt, pts.end());
        }
    }
}


END_NS(alg)


