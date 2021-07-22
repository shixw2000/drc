#ifndef __NODETREE_H__
#define __NODETREE_H__
#include<list>
#include<set>
#include"globaldef.h"
#include"box.h"
#include"datanode.h"


BEG_NS(alg)


/* 注意：排列的时候，必须使polygon1.id < polygon2.id，否则会有重复的点集 */
template<typename T>
struct ItemReturn {
    const NS(geo)::Polygon<T>* m_polygon1; //key1
    const NS(geo)::Point<T>* m_p1;
    int m_index1;
    const NS(geo)::Polygon<T>* m_polygon2; //key2
    const NS(geo)::Point<T>* m_p2;
    int m_index2;
    T m_sq_distance; //square distance
};

template<typename T>
struct ItemReturnCmp {
    bool operator()(const ItemReturn<T>& item1, const ItemReturn<T>& item2) const {
        return (item1.m_polygon1->id() < item2.m_polygon1->id())
            || ((item1.m_polygon1->id() == item2.m_polygon1->id())
                && item1.m_polygon2->id() < item2.m_polygon2->id());
    }
};

class NodeTree { 
public:
    typedef double value_type;
    typedef Node<value_type> typeNode;
    typedef typeNode* typeNodePtr;
    typedef NS(geo)::Point<value_type> typePoint;
    typedef NS(geo)::Box<value_type> typeBox;
    typedef NS(geo)::Region<value_type> typeRegion; 
    typedef NS(geo)::Dimension<value_type> typeDimension;
    typedef NS(geo)::Contour<value_type> typeContour; 
    typedef NS(geo)::Polygon<value_type> typePolygon;
    typedef NS(geo)::Segment<value_type> typeSegment; 
    typedef NS(geo)::Segments<value_type> typeSegments;
    typedef NS(geo)::Area<value_type> typeArea; 

    typedef NS(geo)::AreaEnum<value_type> typeAreaEnum;
    typedef NS(geo)::AreaEnumEx<value_type> typeAreaEnumEx;
    typedef NS(geo)::AreaEnumEx2<value_type> typeAreaEnumEx2;

    typedef NS(geo)::Border<value_type> typeBorder;
    typedef NS(geo)::Borders<value_type> typeBorders;
    typedef NS(geo)::Boundary<value_type> typeBoundary; 

    typedef STD_NS(list)<typeNodePtr> typeListNodes;
    typedef ItemReturn<value_type> typeItemResult;
    typedef STD_NS(set)<typeItemResult, ItemReturnCmp<value_type> > typeChkResult;
    
public:
    NodeTree();
    ~NodeTree();

    int procSpaceRule(const typeBox* area, const typePolygon* polygons, int size, 
        value_type distance, typeChkResult& ret);
    
    void dispResult(const typeChkResult& ret) const;

    /* 广度优先 */
    void buildTree(const typeBox* box, const typePolygon* polygons, int size,
        int minPolygonCnt, value_type minWidth);

    void deleteTree();

    /* 深度优先 */
    void checkOpcRule(value_type d, typeChkResult& ret);

    void segment2Boundaries(const typeSegment& segment, const typePolygon* polygon, typeBorders& borders);
    void segment2Boundary(const typeSegment& segment, const typePolygon* polygon, typeBorder& border);

    void calcSegmentDist(const typeSegment& segment1, const typePolygon* polygon1, 
        const typeSegment& segment2, const typePolygon* polygon2, 
        typeChkResult& ret);

    void partitionSegment(typeNodePtr node, typeSegment& segment, const typePolygon* polygon);

private:
    bool needPartition(typeNodePtr node) const;

    /* add begin by shixw, used by build tree 2021-04-21*/
    void dealNode(typeNodePtr node, typeListNodes& list);
    typeNodePtr createRootNode(const typeDimension& dim, const typePolygon* polygons, int size);
    typeNodePtr createChildNode(const typeDimension& dim, int level);


    /* add begin by shixw,  used by check rule 2021-04-21*/
    void checkNode(typeNodePtr node, typeListNodes& lists, typeBoundary* boundary, typeChkResult& ret);
    void searchInterNode(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret);
    void searchLeaf(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret);
    void searchBoundary(typeNodePtr node, typeBoundary* boundary, typeChkResult& ret);
        
    void getBoundaryFromDim(const typeDimension& dim, typeBorders& borders);
    void mergeBoundary(typeNodePtr node, typeBoundary* boundary); 
    void getBoundaryViolation(typeArea& area1,typeArea& area2, typeChkResult& ret); 
    void getBoundary(typeNodePtr node, typeBorders& borders);

private:
    typedef void (NodeTree::*typeNodeFunc)(typeNodePtr);
    typedef void (NodeTree::*typeNodeFuncConst)(const typeNode*) const;
    
    void foreachChild(typeNodeFuncConst func) const;
    void foreachChild(typeNodeFunc func);
    void deleteChildren(typeNodePtr ptr);
    
private: 
    int m_minPolygonCnt;    //input min polygon count per node 
    value_type m_minWidth;  //input min width of a node
    value_type m_distance;  //input min distance checked by
    value_type m_sq_distance; // m_distance* m_distance for comparing
    int m_maxLevel;         //may be the max level
    int m_maxUsedLevel;     //actually used max level
    value_type m_axisLen;    //min_width * 2 ^n, the max len of coordinate
    value_type m_usedWidth; //max len between width and height of box
    typeBox m_box;        //input box area
    typeDimension m_dim;    // squere coord
    typeNodePtr m_root;
    typeListNodes m_children_lists;
};


/*****************************************************************************
 * Function      : AreaPartition
 * Description   : 遍历一个区域内所有polygon片段
 * Input         : geo  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20210423
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/
template<typename T>
class AreaPartition : public NS(geo)::AreaVisitor<T>, public NS(geo)::SegmentsVisitor<T> {
public:
    AreaPartition(NodeTree& tree, Node<T>& node) : m_tree(tree), m_node(node) {}
    
    virtual void visit(NS(geo)::Segments<T>& segments) {
        NS(geo)::SegmentsEnum<T> enumerator(segments);

        enumerator.foreach(*this);
    } 

    virtual void visit(const NS(geo)::Polygon<T>* polygon, NS(geo)::Segment<T>& segment) {
        m_tree.partitionSegment(&m_node, segment, polygon);
    } 

private:
    NodeTree& m_tree;
    Node<T>& m_node; 
};


/*****************************************************************************
     * Function      : NodeTree.getAreaViolation
     * Description   : 检查区域内片段集中不同polygon子片段的最小间距，使用扩展枚举
     * Input         : const typeArea& area  
                    typeChkResult& ret    
     * Output        : None
     * Return        : void
     * Others        : 
     * Record
     * 1.Date        : 20210422
     *   Author      : shixw
     *   Modification: Created function

    *****************************************************************************/
template<typename T>
class NodeChk : public NS(geo)::AreaVisitorEx<T>, public NS(geo)::SegmentsVisitorEx<T> {
public:
    NodeChk(NodeTree& tree, NodeTree::typeChkResult& ret) 
        : m_tree(tree), m_ret(ret) {} 
    
    virtual void visit(NS(geo)::Segments<T>& segments1, NS(geo)::Segments<T>& segments2) {
        if (segments1.id() != segments2.id()) {
            NS(geo)::SegmentsEnumEx<T> enumerator(segments1, segments2);

            enumerator.foreach(*this);
        }
    }

    virtual void visit(const NS(geo)::Polygon<T>* polygon1, NS(geo)::Segment<T>& segment1, 
        const NS(geo)::Polygon<T>* polygon2, NS(geo)::Segment<T>& segment2) {
        
        m_tree.calcSegmentDist(segment1, polygon1, segment2, polygon2, m_ret);
    }

private:
    NodeTree& m_tree;
    NodeTree::typeChkResult& m_ret;
};


template<typename T>
class AreaBoundary : public NS(geo)::AreaVisitor<T>, public NS(geo)::SegmentsVisitor<T> {
public:
    AreaBoundary(NodeTree& tree, NS(geo)::Borders<T>& borders) : m_tree(tree), m_borders(borders) {}
    
    virtual void visit(NS(geo)::Segments<T>& segments) {
        NS(geo)::SegmentsEnum<T> enumerator(segments);

        enumerator.foreach(*this);
    } 

    virtual void visit(const NS(geo)::Polygon<T>* polygon, NS(geo)::Segment<T>& segment) {
        m_tree.segment2Boundaries(segment, polygon, m_borders);
    } 

private:
    NodeTree& m_tree;
    NS(geo)::Borders<T>& m_borders;
};


END_NS(alg)

#endif

