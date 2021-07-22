#ifndef __DATANODE_H__
#define __DATANODE_H__
#include"globaldef.h"
#include"coordtype.h"
#include"region.h"
#include"polygon.h"


BEG_NS(alg)

enum NodeStatus {
    NODE_STATUS_INIT,
    NODE_STATUS_DOING,
    NODE_STATUS_DONE,
};

template<typename T>
struct Node { 
    int m_order;  /* 属于第几个子节点 */
    NodeStatus m_status; 
    NS(geo)::Region<T> m_region; 

    struct Node<T>* m_children;
};


END_NS(alg)


#endif

