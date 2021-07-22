#ifndef __WRITER_H__
#define __WRITER_H__
#include"globaldef.h"


BEG_NS(util)

class FileWriter {
public:
    static int init(const char szName[]);
    static void finish();

    /*** need init first ***/
    static FileWriter* instance();
    
    void format(const char format[], ...); 

private:
    explicit FileWriter(const char szName[]);
    ~FileWriter();

    int openFile();
    void closeFile();
    
private:
    char m_name[256];
    int m_fd;
};


END_NS(util)


#define FILE_LOG_INIT(name) NS(util)::FileWriter::init(name)
#define FILE_LOG(formats,arg...) NS(util)::FileWriter::instance()->format(formats, ##arg)
#define FILE_LOG_FINISH() NS(util)::FileWriter::finish()

#endif

