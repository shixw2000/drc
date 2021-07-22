#include<list>
#include<vector>
#include"globaldef.h"
#include"point.h"
#include"contour.h"
#include"libexport.h"
#include"test01.h"


BEG_NS(test) 


typedef STD_NS(list)<value_type> typeList;


static const int MAX_LINE_SIZE = 1024 * 1024;
static char g_rdbuf[MAX_LINE_SIZE] = {0}; 

int line2nums(const char* line, int len, typeList& lists) {
    const char* psz = NULL;
    const char* pend = NULL;
    char* endptr = NULL;
    double val = .0;

    lists.clear();
    
    psz = line;
    pend = psz + len;
    while (psz < pend) {
        errno = 0;
        val = strtod(psz, &endptr);
        if (0 == errno) {
            if (endptr != psz) {
                lists.push_back(val);

                psz = endptr;
            } else {
                /* to the end */
                break;
            }
        } else {
            LOG_INFO("strtod| len=%d| psz=%s| val=%.2lf| errno=%d|",
                len, psz, val, errno);
            return -1;
        }
    }
    
    return lists.size();
}

static int createContour(const typeList& lists, typeVecContours& vecContour) {
    int size = 0; 
    typeContour* contour = NULL;
    typePoint* point = NULL;
    typeList::const_iterator itr;

    size = (int)(lists.size());

    /* size is even number, with point of <x, y> */
    if (0 < size && 0 == (size & 0x1)) {
        size >>= 1;
        
        point = new typePoint[size];

        itr = lists.begin();
        for (int i=0; i<size; ++i) {
            point[i].set_x(*(itr++));
            point[i].set_y(*(itr++)); 
        }

        contour = new typeContour(point, size);
        vecContour.push_back(contour); 

        return 0;
    } else {
        return -1;
    }
}

static int createEdges(const typeList& lists, typeVecEdges& vecEdges) {
    int size = 0; 
    double val[4]; 
    typeList::const_iterator itr;

    size = (int)(lists.size());

    /* size is even number, with edge of <x1, y1, x2, y2> */
    if (4 == size) { 
        itr = lists.begin();
        
        for (int i=0; i<size; ++i) {
            val[i] = *(itr++);
        }

        typeEdge edge(val[0], val[1], val[2], val[3]);
        vecEdges.push_back(edge); 

        return 0;
    } else {
        return -1;
    }
}

template<typename T>
int parseFileLine(const char szFile[], STD_NS(vector)<T>& data, 
    int (*callback)(const typeList& lists, STD_NS(vector)<T>& vecs)) {
    int ret = 0;
    int rdlen = 0;
    int lineno = 0;
    FILE* f = NULL;
    char* psz = NULL;
    typeList lists;

    f = fopen(szFile, "rb");
    CHK_NULL(f, szFile);

    while (!feof(f)) {
        psz = fgets(g_rdbuf, MAX_LINE_SIZE, f);
        if (NULL == psz) {
            //LOG_INFO("fgets| msg=read eof|");
            ret = 0;
            break;
        } 

        ++lineno;

        /* delete "\r\n" */
        rdlen = strnlen(g_rdbuf, MAX_LINE_SIZE);
        if (0 < rdlen && '\n' == g_rdbuf[rdlen-1]) {
            g_rdbuf[--rdlen] = '\0';

            if (0 < rdlen && '\r' == g_rdbuf[rdlen-1]) {
                g_rdbuf[--rdlen] = '\0';
            }
        }
        
        if (0 == rdlen) {
            continue;
        }

        ret = line2nums(g_rdbuf, rdlen, lists);
        if (0 < ret) {
            ret = callback(lists, data);
            if (0 != ret) {
                LOG_INFO("callback| lineno=%d| rdlen=%d| ret=%d|"
                    " err=callback error|", lineno, rdlen, ret);
                ret = -1;
                break;
            }
        } else {
            LOG_INFO("line2num| lineno=%d| rdlen=%d| ret=%d|"
                " err=invalid line|", lineno, rdlen, ret);

            ret = -1;
            break;
        } 
    } 

    fclose(f); 
    return ret;
}

int parseFile(const char szFile[], typeVecContours& vecContour) {
    int ret = 0;

    ret = parseFileLine(szFile, vecContour, &createContour);
    return ret;
}

int parseEdges(const char szFile[], typeVecEdges& vecEdges) {
    int ret = 0;

    ret = parseFileLine(szFile, vecEdges, &createEdges);
    return ret;
}

int testOpcSpaceNormal(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetSpace result;

    LOG_INFO("testOpcSpaceNormal_beg| file=[%s]| distance=%.2lf|", conf.m_name, conf.m_distance);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = chkOpcPolygonSpaceList(vecContour, conf.m_distance, result, conf.m_isPrnt);
    } 

    LOG_INFO("testOpcSpaceNormal_end|====\n");

    freeContours(vecContour);
    return ret; 
}

int testOpcSpaceSlow(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetSpace result;

    LOG_INFO("testOpcSpaceSlow_beg| file=[%s]| distance=%.2lf|", conf.m_name, conf.m_distance);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = chkOpcPolygonSpaceSlow(vecContour, conf.m_distance, result, true);
    } 

    LOG_INFO("testOpcSpaceSlow_end|====\n");
    
    freeContours(vecContour);

    return ret;
}

int testOpcSpaceEx(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetSpace result;

    LOG_INFO("testOpcSpaceEx_beg| file=[%s]| minpolygon=%d| minWidth=%.2lf| distance=%.2lf|", 
        conf.m_name, conf.m_minPolygon, conf.m_minWidth, conf.m_distance);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = chkOpcPolygonSpaceExList(vecContour, conf.m_minPolygon, conf.m_minWidth, 
            conf.m_distance, result, conf.m_isPrnt);
    } 

    LOG_INFO("testOpcSpaceEx_end|====\n");

    freeContours(vecContour);
    
    return ret;
}

int testOpcSpaceExLoop(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetSpace result;

    LOG_INFO("testOpcSpaceExLoop_beg| file=[%s]| minpolygon=%d| minWidth=%.2lf| distance=%.2lf| loop=%d|", 
        conf.m_name, conf.m_minPolygon, conf.m_minWidth, conf.m_distance, conf.m_loop);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        for (int i=0; i<conf.m_loop; ++i) {
            ret = chkOpcPolygonSpaceExList(vecContour, conf.m_minPolygon, conf.m_minWidth, conf.m_distance, result);
            result.clear();
        }
    } 

    LOG_INFO("testOpcSpaceExLoop_end|====\n");

    freeContours(vecContour);
    return ret;
}

int testOpcPinchNormal(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetPinch result;

    LOG_INFO("testOpcPinchNormal_beg| file=[%s]| distance=%.2lf|", conf.m_name, conf.m_distance);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = chkOpcPolygonPinchList(vecContour, conf.m_distance, result, conf.m_isPrnt);
    } 

    LOG_INFO("testOpcPinchNormal_end|====\n");

    freeContours(vecContour);
    return ret; 
}

int testOpcPinchEx(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeMapDistance mapDistance;
    typeListRetPinch result;

    LOG_INFO("testOpcPinchNormal_beg| file=[%s]| distance=%.2lf|", conf.m_name, conf.m_distance);

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        for (int i=1; i<=(int)vecContour.size(); ++i) {
            mapDistance[i] = conf.m_distance;
        }
        
        ret = chkOpcPolygonPinchListEx(vecContour, mapDistance, result, conf.m_isPrnt);
    } 

    LOG_INFO("testOpcPinchNormal_end|====\n");

    freeContours(vecContour);
    return ret; 
}

int testOpcCDNormal(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetCD result;
    typeVecEdges edges;
    typePoint p1;
    typePoint p2;
    typeEdge edge;

    LOG_INFO("testOpcCDNormal_beg| file=[%s]| edges_cnt=%d|", conf.m_name, conf.m_edge_cnt);

    for (int i=0; i<conf.m_edge_cnt; ++i) {
        p1.set_x(conf.m_edges[i][0]);
        p1.set_y(conf.m_edges[i][1]);
        p2.set_x(conf.m_edges[i][2]);
        p2.set_y(conf.m_edges[i][3]);

        edge.set_p1(p1);
        edge.set_p2(p2);
        edges.push_back(edge); 

        LOG_INFO("[%d]edge=<%.2lf, %.2lf>-<%.2lf, %.2lf>|", 
            i, p1.x(), p1.y(), p2.x(), p2.y()); 
    } 

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = calcOpcPolygonCDList(vecContour, edges, result, conf.m_isPrnt);
    } 

    LOG_INFO("testOpcCDNormal_end|====\n");

    freeContours(vecContour);
    return ret; 
}

int testOpcCDNormalByFile(const CONFIG_PARAM& conf) {
    int ret = 0;
    typeVecContours vecContour;
    typeListRetCD result;
    typeVecEdges edges; 

    ret = parseEdges(conf.m_cdfile, edges);
    if (0 == ret) {
        LOG_INFO("testOpcCDNormalByFile_beg| file=[%s]| edges_file=%s| totoal_edges=%d|",
            conf.m_name, conf.m_cdfile, (int)edges.size());
        
        for (int i=0; i<(int)edges.size(); ++i) {
            const typeEdge& edge = edges[i];

            LOG_INFO("[%d]edge=<%.2lf, %.2lf>-<%.2lf, %.2lf>|", 
                i, edge.x1(), edge.y1(), edge.x2(), edge.y2()); 
        } 
    } else {
        LOG_INFO("testOpcCDNormalByFile_err| file=[%s]| edges_file=%s| ret=%d| error=parse edges error|",
            conf.m_name, conf.m_cdfile, ret);
        
        return -1;
    } 

    ret = parseFile(conf.m_name, vecContour);
    if (0 == ret) {
        ret = calcOpcPolygonCDList(vecContour, edges, result, conf.m_isPrnt);
    } else {
        LOG_INFO("testOpcCDNormalByFile_err| file=[%s]| edges_file=%s| ret=%d| error=parse contours error|",
            conf.m_name, conf.m_cdfile, ret); 
    }

    LOG_INFO("testOpcCDNormalByFile_end|====\n");

    freeContours(vecContour);
    return ret; 
}

END_NS(test)

