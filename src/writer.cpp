#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdarg.h>
#include"writer.h"


BEG_NS(util)

static FileWriter* g_instance = NULL;

FileWriter* FileWriter::instance() { 
    return g_instance;
}

int FileWriter::init(const char szName[]) {
    int ret = 0;

    if (NULL != g_instance) {
        LOG_INFO("init_writer| old_file=%s| in_file=%s| err=already opened|",
            g_instance->m_name, szName);
        return -1;
    }
    
    g_instance = new FileWriter(szName);
    if (NULL != g_instance) {
        ret = g_instance->openFile();
        if (0 == ret) {
            return 0;
        } else {
            finish();
            return -1;
        }
    } else {
        return -1;
    }
}

void FileWriter::finish() {
    if (NULL != g_instance) {
        g_instance->closeFile();
        
        delete g_instance; 
        g_instance = NULL;
    }
}

FileWriter::FileWriter(const char szName[]) : m_fd(-1) {
    strncpy(m_name, szName, sizeof(m_name));
    m_name[sizeof(m_name)-1] = '\0';
}

FileWriter::~FileWriter() {
    closeFile();
}

int FileWriter::openFile() {

    m_fd = open(m_name, O_CREAT | O_TRUNC | O_WRONLY, 0640);
    if (0 <= m_fd) {
        return 0;
    } else {
        LOG_INFO("open_writer| file=%s| err=%s|", m_name, ERR_MSG);
        return -1;
    }
}

void FileWriter::closeFile() {
    if (0 <= m_fd) {
        close(m_fd);
        
        m_fd = -1;
    }
}

void FileWriter::format(const char format[], ...) {
    static const int MAX_SIZE = 10240;
    int ret = 0;
    int size = 0;
    char szbuf[MAX_SIZE] = {0};
    va_list ap;

    va_start(ap, format);
    size = vsnprintf(szbuf, MAX_SIZE, format, ap); 
    va_end(ap);

    if (size > MAX_SIZE) {
        size = MAX_SIZE;
    }
    
    ret = write(m_fd, szbuf, size);
    if (-1 == ret) {
        LOG_INFO("write_file| file=%s| buf_size=%d| error=%s|",
            m_name, size, ERR_MSG);
    }
}

END_NS(util)


