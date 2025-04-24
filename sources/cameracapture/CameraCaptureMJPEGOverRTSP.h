#ifndef CAMERACAPTUREMJPEGOVERRTSP_H
#define CAMERACAPTUREMJPEGOVERRTSP_H

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iterator>

#include <limits.h>
#include <thread>
#include <mutex>

#include "../exception/CameraCaptureException.h"
#include "Socket.h"
#include "../common/base64/base64.h"
#include "../common/plog/Log.h"
#include "ICameraCapture.h"
#include "CameraCaptureMJPEG.h"

namespace vms {
    namespace cameracapture {
        class CameraCaptureMJPEGOverRTSP : public ICameraCapture {
        private:
            std::string m_video_stream_address;
            std::string m_login;
            std::string m_password;
            
            cv::VideoCapture * m_capture = nullptr;
            
            int m_width = -1;
            int m_height = -1;
            
            bool m_decode_to_mat_opencv = true;
            
        public:
            CameraCaptureMJPEGOverRTSP(const std::string & video_stream_address, const std::string & login, const std::string & password);
            ~CameraCaptureMJPEGOverRTSP(){
                delete m_capture;
            }
            std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> fetchNextFrame();
            
            void setDecodeToMatOpenCV(bool state) override {
                m_decode_to_mat_opencv = state;
            }
        };
        
    }
}

#endif

