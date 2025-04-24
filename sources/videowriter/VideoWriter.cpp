#include "VideoWriter.h"

const char * vms::VideoWriter::TIMESTAMP_START_ATTR = "user.timestamp_start";
const char * vms::VideoWriter::TIMESTAMP_END_ATTR = "user.timestamp_end";
const char * vms::VideoWriter::CURRENT_FILE_ATTR = "user.current_file";

void vms::VideoWriter::init() {
    bool current_file_exists = false;
    m_next_file_suffix = 0;
    for (size_t i = 0; i < m_count_chunks; i++) {
        std::string pathname = "storage/" + m_prefix_file_storage + "-" + std::to_string(i) + ".avi";
        int fd = open(pathname.c_str(), O_CREAT | O_EXCL, S_IRWXU | S_IROTH);
        if (fd == -1) {
            if (errno != EEXIST) {
                LOGE << "Error created file " << pathname << ". " << std::string(strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else {
            std::string timestamp = std::to_string(vms::common::DateTimeUtil::getCurrentTime());
            char buffer[SIZE_BUFFER_EXTENDED_ATTRIBUTE];
            memset(buffer, '\0', SIZE_BUFFER_EXTENDED_ATTRIBUTE);
            memcpy(buffer, timestamp.c_str(), timestamp.size());
            if (setxattr(pathname.c_str(), TIMESTAMP_START_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
                LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
                exit(EXIT_FAILURE);
            }
            if (setxattr(pathname.c_str(), TIMESTAMP_END_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
                LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
                exit(EXIT_FAILURE);
            }
            close(fd);
        }
    }
    for (size_t i = 0; i < m_count_chunks; i++) {
        char buffer[SIZE_BUFFER_EXTENDED_ATTRIBUTE];
        std::string pathname = "storage/" + m_prefix_file_storage + "-" + std::to_string(i) + ".avi";
        if (!current_file_exists) {
            int ret = getxattr(pathname.c_str(), CURRENT_FILE_ATTR, buffer, 0);
            if (ret != -1) {
                current_file_exists = true;
                m_next_file_suffix = i;
            } else if (ret == -1 && errno != ENOATTR) {
                LOGE << "Error read extendeed attribute of file";
                exit(EXIT_FAILURE);
            }
        }
    }
    if (!current_file_exists) {
        char buffer[SIZE_BUFFER_EXTENDED_ATTRIBUTE];
        memset(buffer, '\0', SIZE_BUFFER_EXTENDED_ATTRIBUTE);
        std::string path = "storage/" + m_prefix_file_storage + "-0.avi";
        if (setxattr(path.c_str(), CURRENT_FILE_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
            LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

vms::VideoWriter::VideoWriter(const std::string & prefix_file_storage, int codec, double fps, int width, int height, int count_chunks) {
    m_count_chunks = count_chunks;
    m_prefix_file_storage = prefix_file_storage;
    m_codec = codec;
    m_width = width;
    m_height = height;
    m_fps = fps;
    init();
    m_current_path_to_storage_file = "storage/" + prefix_file_storage + "-" + std::to_string(m_next_file_suffix) + ".avi";
    m_video_writer = new cv::VideoWriter(m_current_path_to_storage_file, codec, fps, cv::Size(width, height));
}

void vms::VideoWriter::write(const cv::Mat & image) {
    m_video_writer->write(image);
    if (checkFileSizeIsTooLarge(m_current_path_to_storage_file)) {
        m_video_writer->release();
        delete m_video_writer;

        std::string timestamp = std::to_string(vms::common::DateTimeUtil::getCurrentTime());
        char buffer[SIZE_BUFFER_EXTENDED_ATTRIBUTE];
        memset(buffer, '\0', SIZE_BUFFER_EXTENDED_ATTRIBUTE);
        memcpy(buffer, timestamp.c_str(), timestamp.size());
        if (setxattr(m_current_path_to_storage_file.c_str(), TIMESTAMP_END_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
            LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (removexattr(m_current_path_to_storage_file.c_str(), CURRENT_FILE_ATTR) == -1) {
            LOGE << "Error remove extend attribute file. " << std::string(strerror(errno));
            exit(EXIT_FAILURE);
        }
        m_next_file_suffix = (m_next_file_suffix + 1) % m_count_chunks;
        m_current_path_to_storage_file = "storage/" + m_prefix_file_storage + "-" + std::to_string(m_next_file_suffix) + ".avi";
        memset(buffer, '\0', SIZE_BUFFER_EXTENDED_ATTRIBUTE);
        memcpy(buffer, timestamp.c_str(), timestamp.size());
        if (setxattr(m_current_path_to_storage_file.c_str(), TIMESTAMP_START_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
            LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
            exit(EXIT_FAILURE);
        }
        memset(buffer, '\0', SIZE_BUFFER_EXTENDED_ATTRIBUTE);
        if (setxattr(m_current_path_to_storage_file.c_str(), CURRENT_FILE_ATTR, buffer, SIZE_BUFFER_EXTENDED_ATTRIBUTE, 0) == -1) {
            LOGE << "Error set extend attribute file. " << std::string(strerror(errno));
            exit(EXIT_FAILURE);
        }
        m_video_writer = new cv::VideoWriter(m_current_path_to_storage_file, m_codec, m_fps, cv::Size(m_width, m_height));
    }
}

bool vms::VideoWriter::checkFileSizeIsTooLarge(const std::string& filename) {
    struct stat fi;
    stat(filename.c_str(), &fi);
    if (fi.st_size + RESERVE_SIZE_STORAGE_FILE_BYTE > MAX_SIZE_STORAGE_FILE_BYTE)
        return true;
    return false;
}