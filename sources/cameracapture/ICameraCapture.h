#ifndef ICAMERACAPTURE_H
#define ICAMERACAPTURE_H

#include <tuple>

namespace vms {
    namespace cameracapture {

        class ICameraCapture {
            
        protected:
            
        public:
            virtual std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> fetchNextFrame() = 0;
            virtual void setDecodeToMatOpenCV(bool) = 0;
            
        };
    }
}

#endif

