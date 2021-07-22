/********************************************************************************

 **** Copyright (C), 2021, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : main.cpp
 * Author        : shixw
 * Date          : 2021-04-19
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2021-04-19
 *   Author      : shixw
 *   Modification: Created file

*************************************************************************************************************/
#include"globaldef.h"
#include"test01.h"
#include"libexport.h"


void usage(int argc, char* argv[]) {
    LOG_INFO("usage:"
        "\n%s 0 #test01"
        "\n\t 1 <distance> <data_file> <minPolygon> <minWidth> <loop>|"
        "\n\t 2 <distance> <data_file> <minPolygon> <minWidth> <bPrint:0/1>|"
        "\n\t 3 <distance> <data_file> <bPrint:0/1> #space normal"
        "\n\t 4 <distance> <data_file>| #space slow"
        "\n\t 5 <distance> <data_file> <bPrint:0/1> #pinch normal"
        "\n\t 6 <data_file> <bPrint:0/1> edge_cnt, edges... #CD normal"
        "\n\t 7 <data_file> <bPrint:0/1> <edge_file> #CD normal", argv[0]);
}

static int parseConf(int argc, char* argv[], NS(test)::CONFIG_PARAM& conf) {
    int ret = 0;

    if (2 > argc) {
        return -1;
    } 

    conf.m_opt = atoi(argv[1]);

    ret = -1;
    switch(conf.m_opt) {
    case 0:
        ret = 0;
        break;
    
    case 1:
        if (7 == argc) { 
            conf.m_distance = atof(argv[2]);
            conf.m_name = argv[3];
            conf.m_minPolygon = atoi(argv[4]);
            conf.m_minWidth = atof(argv[5]);
            conf.m_loop = atoi(argv[6]);

            ret = 0;
        }
        break;

    case 2:
        if (7 == argc) { 
            conf.m_distance = atof(argv[2]);
            conf.m_name = argv[3];
            conf.m_minPolygon = atoi(argv[4]);
            conf.m_minWidth = atof(argv[5]);
            conf.m_isPrnt = atoi(argv[6]);

            ret = 0;
        }
        
        break;

    case 3:
        if (5 == argc) { 
            conf.m_distance = atof(argv[2]);
            conf.m_name = argv[3];
            conf.m_isPrnt = atoi(argv[4]);

            ret = 0;
        }
        break;

    case 4:
        if (4 == argc) { 
            conf.m_distance = atof(argv[2]);
            conf.m_name = argv[3];

            ret = 0;
        }
        break;

    case 5:
    case 15:
        if (5 == argc) { 
            conf.m_distance = atof(argv[2]);
            conf.m_name = argv[3];
            conf.m_isPrnt = atoi(argv[4]);

            ret = 0;
        }
        break;

    case 6:
        if (5 <= argc) { 
            conf.m_name = argv[2];
            conf.m_isPrnt = atoi(argv[3]);
            conf.m_edge_cnt = atof(argv[4]);

            if (0 <= conf.m_edge_cnt && 
                10 > conf.m_edge_cnt && 
                argc == (conf.m_edge_cnt * 4 + 5)) {

                int n = 5;
                for (int i=0; i<conf.m_edge_cnt; ++i) {
                    conf.m_edges[i][0] = atof(argv[n++]);
                    conf.m_edges[i][1] = atof(argv[n++]);
                    conf.m_edges[i][2] = atof(argv[n++]);
                    conf.m_edges[i][3] = atof(argv[n++]);
                }

                ret = 0;
            } 
        }
        break;

    case 7:
        if (5 == argc) { 
            conf.m_name = argv[2];
            conf.m_isPrnt = atoi(argv[3]);
            conf.m_cdfile = argv[4];

            ret = 0;
        }
        break;
        
    default:
        LOG_INFO("opt=%d| err=invalid opt|", conf.m_opt);
        break;
    }

    return ret;
}

/*****************************************************************************
 * Function      : main
 * Description   : entrance of the program
 * Input         : int argc      
                char* argv[]  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20210419
 *   Author      : shixw
 *   Modification: Created function

*****************************************************************************/

int main(int argc, char* argv[]) {
    int ret = 0;
    NS(test)::CONFIG_PARAM conf;
    NS(test)::TestSuits tests;

    ret = parseConf(argc, argv, conf);
    if (0 != ret) {
        usage(argc, argv); 
        return ret;
    }

    ret = libInit();
    if (0 != ret) {
        return ret;
    }

    ret = tests.test(conf);

    libFinish();
    return ret;
}

