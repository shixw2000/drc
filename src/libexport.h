#ifndef __LIBEXPORT_H__
#define __LIBEXPORT_H__
#include<vector>
#include<list>
#include<map>
#include"contour.h"
#include"edge.h"


typedef double value_type;
typedef double coord_type;
typedef double distance_type;

typedef opc::geo::Point<value_type> typePoint;
typedef opc::geo::Edge<value_type> typeEdge;
typedef std::vector<typeEdge> typeVecEdges;
typedef opc::geo::Contour<value_type> typeContour;
typedef std::vector<typeContour*> typeVecContours;
typedef std::vector<distance_type> typeVecDistance;
typedef std::map<int, distance_type> typeMapDistance; 


struct OPC_API RetChkSpace {
    int m_polygon1;      // polygon1 id
    int m_polygon2;      // polygon2 id
    value_type m_x1;
    value_type m_y1;
    value_type m_x2;
    value_type m_y2;
    double m_sq_distance;
    double m_distance;
};

typedef std::list<RetChkSpace> typeListRetSpace;


/* pinch result */
struct OPC_API RetChkPinch {
    int m_polygon;      // polygon id
    coord_type m_ex1;   // edge 1 p1
    coord_type m_ey1;
    coord_type m_ex2;   // edge 1 p2
    coord_type m_ey2;
    coord_type m_ox1;   // edge 2 p1
    coord_type m_oy1;
    coord_type m_ox2;   // edge 2 p2
    coord_type m_oy2;
    distance_type m_distance;  // min distance of pinch 
};

typedef std::list<RetChkPinch> typeListRetPinch;

extern int OPC_API libInit();
extern int OPC_API libFinish();

/*****************************************************************************
 * Function      : chkOpcPolygonSpaceList
 * Description   : 标准的用户检查多边形最小间距的函数
 * Input         : const typeVecContours& vecContour  
                value_type distance                
                typeVecRetSpace& ret               
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210428
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API chkOpcPolygonSpaceList(const typeVecContours& vecContour, 
    value_type distance, typeListRetSpace& ret, bool bPrint = false);

/*****************************************************************************
 * Function      : chkOpcPolygonSpaceSlow
 * Description   : 穷举法实现的较慢的用户检查多边形最小间距的函数
 * Input         : const typeVecContours& vecContour  
                value_type distance                
                typeVecRetSpace& ret               
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210428
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API chkOpcPolygonSpaceSlow(const typeVecContours& vecContour, 
    value_type distance, typeListRetSpace& ret, bool bPrint = false);

/*****************************************************************************
 * Function      : chkOpcPolygonSpaceExList
 * Description   : 用户检查多边形最小间距的函数，允许输入最小个数、最小宽度，可用于内部测试
 * Input         : const typeVecContours& vecContour  
                int minPolygon                     
                double minWidth                    
                double distance                    
                typeVecRetSpace& ret               
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210428
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API chkOpcPolygonSpaceExList(const typeVecContours& vecContour, int minPolygon, double minWidth,
    double distance, typeListRetSpace& ret, bool bPrint = false);

extern void OPC_API dispSpaceRet(const typeListRetSpace& ret);

extern void OPC_API freeContours(typeVecContours& vecContours);

/*****************************************************************************
 * Function      : chkOpcPolygonPinchList
 * Description   : pinch规则检查
 * Input         : const typeVecContours& vecContour  
                value_type distance                
                typeListRetPinch& ret              
                bool bPrint = false                
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210512
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API chkOpcPolygonPinchList(const typeVecContours& vecContour, 
    distance_type distance, typeListRetPinch& ret, bool bPrint = false);

/*****************************************************************************
 * Function      : chkOpcPolygonPinchListEx
 * Description   : pinch规则检查, 指定polygon的 pinch 距离
 * Input         : const typeMapDistance& mapDistance 
                const typeVecValues& vecDistance          
                typeListRetPinch& ret              
                bool bPrint = false                
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210625
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API chkOpcPolygonPinchListEx(const typeVecContours& vecContour, 
    const typeMapDistance& mapDistance, typeListRetPinch& ret, bool bPrint = false);

extern void OPC_API dispPinchRet(const typeListRetPinch& ret);


/* cd result */
struct OPC_API RetCalcCD {
    int m_polygon;      // polygon id
    coord_type m_x1;   // p1
    coord_type m_y1;
    coord_type m_x2;   // p2
    coord_type m_y2;
    distance_type m_length;  // critical dimension 
};

typedef std::list<RetCalcCD> typeCalcCDs;

struct OPC_API RetEdgeCD {
    typePoint m_p1;   // from
    typePoint m_p2;   // to
    typeCalcCDs m_cds;
};

typedef std::list<RetEdgeCD> typeListRetCD;

/*****************************************************************************
 * Function      : calcOpcPolygonCDList
 * Description   : 计算指定线段上的多边形CD
 * Input         : const typeVecContours& vecContour  
                const typePoint& p1                
                const typePoint& p2                
                typeListRetCD& ret                 
                bool bPrint = false                
 * Output        : None
 * Return        : extern
 * Others        : 
 * Record
 * 1.Date        : 20210604
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
extern int OPC_API calcOpcPolygonCDList(const typeVecContours& vecContour, 
    const typeVecEdges& edges, typeListRetCD& ret, bool bPrint = false);

extern void OPC_API dispCDRet(const typeListRetCD& ret);

#endif

