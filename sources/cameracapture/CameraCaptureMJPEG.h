#ifndef CAMERACAPTUREMJPEG_H
#define CAMERACAPTUREMJPEG_H

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/opencv_modules.hpp"

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

#include <limits.h>
#include <thread>
#include <mutex>

#include "../exception/CameraCaptureException.h"
#include "Socket.h"
#include "../common/base64/base64.h"
#include "../common/plog/Log.h"
#include "ICameraCapture.h"

namespace vms {
    namespace cameracapture {

        class CameraCaptureMJPEG : public ICameraCapture {
        private:
            std::string m_video_stream_address;
            std::string m_login;
            std::string m_password;
            
            
            
            std::unique_ptr<vms::socketio::SocketIO<unsigned char, char>> m_socket;

            std::unique_ptr<unsigned char>  m_buffer;
            bool m_header_accept;
            std::string m_boundary;
            size_t m_current_position_buffer;
            std::string m_host;
            int m_port;
            
            static const size_t MAX_SIZE_INNER_BUFFER = 8192;
            
            bool m_decode_to_mat_opencv = true;

        public:
            CameraCaptureMJPEG(const std::string & video_stream_address, const std::string & login, const std::string & password, int port);
            std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> fetchNextFrame() override;
            
            static size_t MAX_SIZE_BUFFER;
            static size_t TIME_BETWEEN_CONNECTION_ATTEMPTS;
            
            void setDecodeToMatOpenCV(bool state) override {
                m_decode_to_mat_opencv = state;
            }
           
        };
    }
}

#endif
