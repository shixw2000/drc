#include<list>
#include"libexport.h"
#include"point.h"
#include"nodetree.h"
#include"coordtype.h"
#include"utiltime.h"
#include"opcpinchrule.h"
#include"writer.h"


typedef NS(alg)::NodeTree typeTree;
typedef typeTree::typePolygon typePolygon;
typedef typeTree::typeBox typeBox;
typedef typeTree::typeItemResult typeItemResult;
typedef typeTree::typeChkResult typeChkResult;

struct PolygonObj {
    typePolygon* m_polygons;
    int m_size;
    typeBox m_area;
};


int OPC_API libInit() {
    int ret = 0;
    const char logname[] = "test_log.txt";

    ret = FILE_LOG_INIT(logname);
    return ret;
}

int OPC_API libFinish() {
    FILE_LOG_FINISH();
    return 0;
}

static int convertObj(const typeVecContours& vecContours, PolygonObj& stuObj) {    
    stuObj.m_polygons = NULL;
    stuObj.m_size = (int)vecContours.size();
    stuObj.m_area.reset();

    if (0 >= stuObj.m_size) {
        return 0;
    }

    stuObj.m_polygons = new typePolygon[stuObj.m_size];
    for (int i=0; i<stuObj.m_size; ++i) {
        stuObj.m_polygons[i].set(i+1, vecContours[i]);
    
        stuObj.m_area += stuObj.m_polygons[i].box();
    }

    return stuObj.m_size;
};

/* 结果集类型转换 */
static void convertSpaceRet(const typeChkResult& result, typeListRetSpace& ret) {
    int size = 0;
    int cnt = 0;
    RetChkSpace retItem;

    ret.clear();
    size = (int)result.size();
    if (0 < size) {
        
        for (typeChkResult::const_iterator itr=result.begin(); itr!=result.end(); ++itr, ++cnt) {
            const typeItemResult& item = *itr;

            retItem.m_polygon1 = item.m_polygon1->id();
            retItem.m_polygon2 = item.m_polygon2->id();
            retItem.m_x1 = item.m_p1->x();
            retItem.m_y1 = item.m_p1->y();
            retItem.m_x2 = item.m_p2->x();
            retItem.m_y2 = item.m_p2->y();

            retItem.m_sq_distance = item.m_sq_distance;
            retItem.m_distance = NS(base)::coord_traits<double>::sqrt(item.m_sq_distance); 

            ret.push_back(retItem);
        }
    }
}

static void freePolygonObj(PolygonObj& stuObj) { 
    if (NULL != stuObj.m_polygons) {
        delete[] stuObj.m_polygons;
        
        stuObj.m_polygons = NULL;
        stuObj.m_size = 0;
        stuObj.m_area.reset();
    } 
}

static int chkOpcPolygonSpace(const typeVecContours& vecContour, double distance, 
    PolygonObj& stuObj, typeChkResult& result, bool bPrint) {
    int size = 0;
    typeTree tree; 
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    result.clear();

    size = convertObj(vecContour, stuObj);
    if (0 < size) {
        for (int i=0; i<size; ++i) { 
            /* print the polygon to file */
            stuObj.m_polygons[i].disp();
        }
    } else {
        return 0;
    }

    LOG_INFO("====Input_params| distance=%.2lf| contour_size=%d| box={<%.2lf:%.2lf>, <%.2lf:%.2lf>}|"
        "width=%.2lf| height=%.2lf|",
        distance, size,
        stuObj.m_area.p1().x(), stuObj.m_area.p1().y(),
        stuObj.m_area.p2().x(), stuObj.m_area.p2().y(),
        stuObj.m_area.width(), stuObj.m_area.height());

    /* 统计时间*/
    begTm.mark();
    
    tree.procSpaceRule(&stuObj.m_area, stuObj.m_polygons, stuObj.m_size, distance, result);

    endTm.mark();  

    size = (int)result.size();
    
    LOG_INFO("=======calc| used_time=%lldms| distance=%.2lf| ret_size=%d|", 
        endTm.diffTimeMs(begTm), distance, size);
    if (0 < size && bPrint) {
        tree.dispResult(result);
    }
    
    return size;
}

static int chkOpcPolygonSpaceEx(const typeVecContours& vecContour, int minPolygon, double minWidth,
    double distance, PolygonObj& stuObj, typeChkResult& result, bool bPrint) {
    int size = 0;
    typeTree tree;
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    result.clear();
    
    size = convertObj(vecContour, stuObj);
    if (0 >= size) {
        return 0;
    }

    LOG_INFO("====Input_params| min_polygon=%d| min_width=%.2lf| distance=%.2lf|"
        " contour_size=%d| box={<%.2lf:%.2lf>, <%.2lf:%.2lf>}|"
        "width=%.2lf| height=%.2lf|",
        minPolygon, minWidth, distance,
        size,
        stuObj.m_area.p1().x(), stuObj.m_area.p1().y(),
        stuObj.m_area.p2().x(), stuObj.m_area.p2().y(),
        stuObj.m_area.width(), stuObj.m_area.height());

    /* 统计时间*/
    begTm.mark();
    
    tree.buildTree(&stuObj.m_area, stuObj.m_polygons, stuObj.m_size, minPolygon, minWidth);    
    tree.checkOpcRule(distance, result);
    tree.deleteTree();
    
    endTm.mark(); 

    size = (int)result.size();
    
    LOG_INFO("=======calcEx| used_time=%lldms| distance=%.2lf| ret_size=%d|", 
        endTm.diffTimeMs(begTm), distance, size);
    
    if (0 < size && bPrint) {
        tree.dispResult(result);
    }
    
    return size;
}


void OPC_API freeContours(typeVecContours& vecContours) {
    typeContour* p = NULL;
    
    for (int i=0; i<(int)vecContours.size(); ++i) {
        p = vecContours[i];
        delete p;
    }

    vecContours.clear();
}

int OPC_API chkOpcPolygonSpaceSlow(const typeVecContours& vecContour, 
    value_type distance, typeListRetSpace& ret, bool bPrint) {
    bool bFound = false;
    int size = 0;
    value_type sq_d = distance * distance;
    value_type xy_len = 0;
    value_type sq_len = 0;
    value_type min_sq_len = 0;
    RetChkSpace item;
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    ret.clear();
    
    begTm.mark();
    
    for (int i=0; i<(int)vecContour.size(); ++i) {
        const typeContour* contour1 = vecContour[i];

        for (int m=i+1; m<(int)vecContour.size(); ++m) {
            const typeContour* contour2 = vecContour[m];

            bFound = false;
            min_sq_len = sq_d;
        
            for (int j=0; j<contour1->size(); ++j) {
                const typePoint& point1 = contour1->get(j); 

                for (int n=0; n<contour2->size(); ++n) {
                    const typePoint& point2 = contour2->get(n);

                    xy_len = NS(geo)::max_xy_length(point1, point2);
                    if (!(distance < xy_len)) {
                        sq_len = NS(geo)::sq_length(point1, point2);
                        if (!(min_sq_len < sq_len)) {
                            min_sq_len = sq_len;
                            
                            item.m_polygon1 = i+1;
                            item.m_polygon2 = m+1;

                            item.m_x1 = point1.x();
                            item.m_y1 = point1.y();
                            item.m_x2 = point2.x();
                            item.m_y2 = point2.y();
                            
                            item.m_sq_distance = sq_len; 
                            bFound = true;
                        }
                    }
                }
            }

            if (bFound) {
                item.m_distance = NS(base)::coord_traits<double>::sqrt(item.m_sq_distance); 
                
                ret.push_back(item);
            }
        }
    }

    endTm.mark(); 

    size = (int)ret.size();
    LOG_INFO("=======calc_slow| used_time=%lldms| distance=%.2lf| ret_size=%d|", 
        endTm.diffTimeMs(begTm), distance, size);
    
    if (0 < size && bPrint) {
        dispSpaceRet(ret);
    }

    return ret.size();
}


void OPC_API dispSpaceRet(const typeListRetSpace& ret) {
    int cnt = 0;
    
    for (typeListRetSpace::const_iterator itr=ret.begin(); itr!=ret.end(); ++itr) {
        const RetChkSpace& item = *itr;

        /* input edge param, with polygon id == 0 */
        FILE_LOG("%d %.2lf %.2lf %.2lf %.2lf %.2lf\n",
            item.m_polygon1, item.m_distance,
            item.m_x1, item.m_y1,
            item.m_x2, item.m_y2);

        ++cnt;
        LOG_INFO("[%d]: polygon1=%d| <x1,y1>=<%.2lf, %.2lf>|"
            " polygon2=%d| <x2,y2>=<%.2lf, %.2lf>|"
            " sq_distance=%.2lf| distance=%.2lf|",
            cnt, 
            item.m_polygon1, item.m_x1, item.m_y1,
            item.m_polygon2, item.m_x2, item.m_y2,
            item.m_sq_distance, item.m_distance);        
    }

    LOG_INFO("disp_result| total=%d|", cnt);
}


/* list rets */
int OPC_API chkOpcPolygonSpaceList(const typeVecContours& vecContour, 
    value_type distance, typeListRetSpace& ret, bool bPrint) {
    int size = 0;
    PolygonObj stuObj;
    typeChkResult result;

    ret.clear();
    
    size = chkOpcPolygonSpace(vecContour, distance, stuObj, result, false);
    if (0 < size) {
        convertSpaceRet(result, ret);

        if (bPrint) {
            dispSpaceRet(ret);
        }
    }

    freePolygonObj(stuObj);
    return size;
}
    
int OPC_API chkOpcPolygonSpaceExList(const typeVecContours& vecContour, int minPolygon, 
    double minWidth, double distance, typeListRetSpace& ret, bool bPrint) {
    int size = 0;
    PolygonObj stuObj;
    typeChkResult result;

    ret.clear();
    
    size = chkOpcPolygonSpaceEx(vecContour, minPolygon, minWidth, distance, stuObj, result, false);
    if (0 < size) {
        convertSpaceRet(result, ret);

        if (bPrint) {
            dispSpaceRet(ret);
        }
    }

    freePolygonObj(stuObj);
    
    return size;
}

/* pinch结果集类型转换 */
static void convertPinchRet(const NS(alg)::typePinchResult& result, typeListRetPinch& ret) {
    int size = 0;
    int cnt = 0;
    RetChkPinch retItem;

    ret.clear();
    size = (int)result.size();
    if (0 < size) {
        
        for (NS(alg)::typePinchResult::const_iterator itr=result.begin(); itr!=result.end(); ++itr, ++cnt) {
            const NS(alg)::StuPinchItem& item = *itr;

            retItem.m_polygon = item.m_polygon->id();
            retItem.m_ex1 = item.m_edg1.x1();
            retItem.m_ey1 = item.m_edg1.y1();
            retItem.m_ex2 = item.m_edg1.x2();
            retItem.m_ey2 = item.m_edg1.y2();

            retItem.m_ox1 = item.m_edg2.x1();
            retItem.m_oy1 = item.m_edg2.y1();
            retItem.m_ox2 = item.m_edg2.x2();
            retItem.m_oy2 = item.m_edg2.y2();

            retItem.m_distance = item.m_distance;
            ret.push_back(retItem);
        }
    }
}

void OPC_API dispPinchRet(const typeListRetPinch& ret) {
    int cnt = 0;
    
    for (typeListRetPinch::const_iterator itr=ret.begin(); itr!=ret.end(); ++itr) {
        const RetChkPinch& item = *itr;

        ++cnt;
        LOG_INFO("[%d]: polygon=%d| edg1=<%.2lf, %.2lf>-<%.2lf, %.2lf>|"
            " edg2=<%.2lf, %.2lf>-<%.2lf, %.2lf>|"
            " distance=%.2lf|",
            cnt, item.m_polygon, 
            item.m_ex1, item.m_ey1,
            item.m_ex2, item.m_ey2,
            item.m_ox1, item.m_oy1,
            item.m_ox2, item.m_oy2,
            item.m_distance);

        /* print results to file log */
        FILE_LOG("%d %.2lf"
            " %.2lf %.2lf %.2lf %.2lf"
            " %.2lf %.2lf %.2lf %.2lf\n",
            item.m_polygon, item.m_distance,
            item.m_ex1, item.m_ey1,
            item.m_ex2, item.m_ey2,
            item.m_ox1, item.m_oy1,
            item.m_ox2, item.m_oy2);
    }

    LOG_INFO("disp_result| total=%d|", cnt);
}

int OPC_API chkOpcPolygonPinchList(const typeVecContours& vecContour, 
    distance_type distance, typeListRetPinch& ret, bool bPrint) {
    int size = 0; 
    NS(alg)::typePinchResult result;
    NS(alg)::OpcPinchStrategy strategy;
    PolygonObj stuObj;
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    ret.clear();
    size = convertObj(vecContour, stuObj);
    if (0 < size) {
        for (int i=0; i<size; ++i) {
            /* normalize polygon */
            stuObj.m_polygons[i].normalize();

            /* extract line of polygon */
            stuObj.m_polygons[i].extractLine();

            /* print the polygon to file */
            stuObj.m_polygons[i].disp(); 
        }
    } else {
        return 0;
    }

    /* 统计时间*/
    begTm.mark();
    
    strategy.procPinchRule(&stuObj.m_area, stuObj.m_polygons, stuObj.m_size, distance, result);

    endTm.mark();
    
    size = (int)result.size(); 
    LOG_INFO("=======calc| used_time=%lldms| distance=%.2lf| ret_size=%d|", 
        endTm.diffTimeMs(begTm), distance, size);
    
    if (0 < size) {
        convertPinchRet(result, ret);
        
        if (bPrint) {
            dispPinchRet(ret);
        }
    }

    freePolygonObj(stuObj);
    return size;
}

int OPC_API chkOpcPolygonPinchListEx(const typeVecContours& vecContour, 
    const typeMapDistance& mapDistance, typeListRetPinch& ret, bool bPrint) {
    int size = 0; 
    NS(alg)::typePinchResult result;
    NS(alg)::OpcPinchStrategy strategy;
    PolygonObj stuObj;
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    ret.clear();
    size = convertObj(vecContour, stuObj);
    if (0 < size) {
        for (int i=0; i<size; ++i) {
            /* normalize polygon */
            stuObj.m_polygons[i].normalize();

            /* extract line of polygon */
            stuObj.m_polygons[i].extractLine();

            /* print the polygon to file */
            stuObj.m_polygons[i].disp(); 
        }
    } else {
        return 0;
    }

    /* 统计时间*/
    begTm.mark();
    
    strategy.procPinchRuleEx(&stuObj.m_area, stuObj.m_polygons, stuObj.m_size, mapDistance, result);

    endTm.mark();
    
    size = (int)result.size(); 
    LOG_INFO("=======calc| used_time=%lldms| distance_size=%d| ret_size=%d|", 
        endTm.diffTimeMs(begTm), (int)mapDistance.size(), size);
    
    if (0 < size) {
        convertPinchRet(result, ret);
        
        if (bPrint) {
            dispPinchRet(ret);
        }
    }

    freePolygonObj(stuObj);
    return size;
}


/* CD结果集类型转换 */
static void convertCDRet(const NS(alg)::typeCDResult& result, typeListRetCD& ret) {
    int size = 0;
    RetEdgeCD retEdge;
    RetCalcCD retItem;

    ret.clear();
    size = (int)result.size();
    if (0 < size) {
        
        for (NS(alg)::typeCDResult::const_iterator itrEdge=result.begin(); itrEdge!=result.end(); ++itrEdge) {
            const NS(alg)::StuRetCD& itemEdge = *itrEdge;

            retEdge.m_p1 = itemEdge.m_edge.p1();
            retEdge.m_p2 = itemEdge.m_edge.p2();
            retEdge.m_cds.clear();

            for (NS(alg)::typeCDItems::const_iterator itrCD=itemEdge.m_cds.begin();
                itrCD!=itemEdge.m_cds.end(); ++itrCD) {
                const NS(alg)::StuCDItem& itemCD = *itrCD;

                retItem.m_polygon = itemCD.m_polygon->id();
                retItem.m_x1 = itemCD.m_point1.x();
                retItem.m_y1 = itemCD.m_point1.y();
                retItem.m_x2 = itemCD.m_point2.x();
                retItem.m_y2 = itemCD.m_point2.y();

                retItem.m_length = itemCD.m_length;
                
                retEdge.m_cds.push_back(retItem);
            }
                
            ret.push_back(retEdge);
        }
    }
}

int OPC_API calcOpcPolygonCDList(const typeVecContours& vecContour, 
    const typeVecEdges& edges, typeListRetCD& ret, bool bPrint) {
    int size = 0; 
    NS(alg)::typeCDResult result;
    NS(alg)::OpcPinchStrategy strategy;
    PolygonObj stuObj;
    NS(util)::UtilTime begTm;
    NS(util)::UtilTime endTm;

    ret.clear();
    size = convertObj(vecContour, stuObj);
    if (0 < size) {
        for (int i=0; i<size; ++i) {
            /* normalize polygon */
            stuObj.m_polygons[i].normalize();

            /* print the polygon to file */
            stuObj.m_polygons[i].disp();
        }
    } else {
        return 0;
    }

    /* 统计时间*/
    begTm.mark();
    
    strategy.procCDRule(&stuObj.m_area, stuObj.m_polygons, stuObj.m_size, edges, result);

    endTm.mark();
    
    size = (int)result.size(); 
    LOG_INFO("=======calc| used_time=%lldms| ret_size=%d|", endTm.diffTimeMs(begTm), size);
    
    if (0 < size) {
        convertCDRet(result, ret);
        
        if (bPrint) {
            dispCDRet(ret);
        }
    }

    freePolygonObj(stuObj);
    return size;
}

void OPC_API dispCDRet(const typeListRetCD& ret) {
    int cnt = 0; 
    int size = 0;
    
    for (typeListRetCD::const_iterator itrEdge=ret.begin(); itrEdge!=ret.end(); ++itrEdge) {
        const RetEdgeCD& itemEdge = *itrEdge;

        ++cnt;
        size = (int)itemEdge.m_cds.size();

        /* input edge param, with polygon id == 0 */
        FILE_LOG("%d %d %.2lf %.2lf %.2lf %.2lf\n",
            0, 0,
            itemEdge.m_p1.x(), itemEdge.m_p1.y(),
            itemEdge.m_p2.x(), itemEdge.m_p2.y());

        
        LOG_INFO("[%d]edg=<%.2lf, %.2lf>-<%.2lf, %.2lf>| cd_size=%d|",
            cnt, 
            itemEdge.m_p1.x(), itemEdge.m_p1.y(),
            itemEdge.m_p2.x(), itemEdge.m_p2.y(),
            size);

        int n = 0;
        for (typeCalcCDs::const_iterator itrCD=itemEdge.m_cds.begin(); itrCD!= itemEdge.m_cds.end(); ++itrCD) {
            const RetCalcCD& itemCD = *itrCD;

            ++n;
        
            /* print results to file log */
            FILE_LOG("%d %.2lf %.2lf %.2lf %.2lf %.2lf\n",
                itemCD.m_polygon, itemCD.m_length,
                itemCD.m_x1, itemCD.m_y1,
                itemCD.m_x2, itemCD.m_y2);

            LOG_INFO(" <%d>%d %.2lf %.2lf %.2lf %.2lf %.2lf",
                n,
                itemCD.m_polygon, itemCD.m_length,
                itemCD.m_x1, itemCD.m_y1,
                itemCD.m_x2, itemCD.m_y2);
        } 
    }

    LOG_INFO("disp_result| total=%d|", cnt);
}


