#ifndef VIDEOWRITER_H
#define VIDEOWRITER_H

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <attr/xattr.h>
#include <fcntl.h>
#include <errno.h>
#include <string>

#include "../common/plog/Log.h"
#include "../common/DateTimeUtil/DateTimeUtil.h"

namespace vms {
    class VideoWriter {
        
    private:
        cv::VideoWriter * m_video_writer = nullptr;
        int m_count_chunks;
        int m_next_file_suffix = 0;
        const int MAX_SIZE_STORAGE_FILE_BYTE = 1024 * 1024 * 128;
        const int RESERVE_SIZE_STORAGE_FILE_BYTE = 1024 * 1024 * 8; 
        
        std::string m_prefix_file_storage;
        std::string m_current_path_to_storage_file;
        
        int m_codec;
        int m_width;
        int m_height;
        double m_fps;
        
        bool checkFileSizeIsTooLarge(const std::string & filename);
        
        void init();
        
        static const int SIZE_BUFFER_EXTENDED_ATTRIBUTE = 40;
        
        static const char * TIMESTAMP_START_ATTR;
        static const char * TIMESTAMP_END_ATTR;
        static const char * CURRENT_FILE_ATTR;
        
    public:
        VideoWriter(const std::string & prefix_file_storage, int codec, double fps, int width, int height, int count_chunks);
        
        ~VideoWriter(){
            delete m_video_writer;
        }
        
        void write(const cv::Mat & image);
        
        void setNextFileSuffix(int num){
            m_next_file_suffix = num;
        }
    };
}

#endif

