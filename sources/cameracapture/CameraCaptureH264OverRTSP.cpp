#include "CameraCaptureH264OverRTSP.h"

vms::cameracapture::CameraCaptureH264OverRTSP::CameraCaptureH264OverRTSP(const std::string& video_stream_address, const std::string& login, const std::string& password) {
    m_video_stream_address = video_stream_address;
    m_login = login;
    m_password = password;
    try {
        m_capture = new cv::VideoCapture("rtsp://" + login + ":" + password + "@" + video_stream_address);
        m_capture->set(cv::CAP_PROP_FOURCC, CV_FOURCC('X', '2', '6', '4'));
        cv::Mat frame;
        if (m_capture->read(frame)) {
            m_width = frame.size().width;
            m_height = frame.size().height;
            m_capture->set(cv::CAP_PROP_FRAME_WIDTH, frame.size().width);
            m_capture->set(cv::CAP_PROP_FRAME_HEIGHT, frame.size().height);
        }
    } catch (cv::Exception & ex) {
        LOGE << ex.what();
        throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + m_video_stream_address + ". " + std::string(strerror(errno)));
    }
}

std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> vms::cameracapture::CameraCaptureH264OverRTSP::fetchNextFrame() {
    if (m_capture != nullptr && m_capture->isOpened()) {
        cv::Mat frame;
        if (m_capture->read(frame)) {
            if (m_width == -1 || m_height == -1) {
                m_width = frame.size().width;
                m_height = frame.size().height;
                m_capture->set(cv::CAP_PROP_FRAME_WIDTH, frame.size().width);
                m_capture->set(cv::CAP_PROP_FRAME_HEIGHT, frame.size().height);
            }
            std::vector<unsigned char> * bytes = new std::vector<unsigned char>();

            if (cv::imencode(".jpg", frame, *bytes)) 
                return std::make_tuple(std::shared_ptr<std::vector<unsigned char>>(bytes), frame);

            throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + m_video_stream_address + ". " + std::string(strerror(errno)));
        }
        throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + m_video_stream_address + ". " + std::string(strerror(errno)));
    }
    LOGE << "Connection is closed. CameraCapture for " << m_video_stream_address << ". Try to connect ...";
    throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + m_video_stream_address + ". ");
}