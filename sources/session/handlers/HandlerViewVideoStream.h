#ifndef HANDLERVIEWVIDEOSTREAM_H
#define HANDLERVIEWVIDEOSTREAM_H

#include "../IHandlerResponse.h"
#include "../../frame.h"
#include "../../exception/SessionException.h"

#include <mutex>
#include <map>

extern std::mutex mutex_map_frames;
extern std::map<int, Frame> map_frames;

namespace vms {

    class HandlerViewVideoStream : public IHandlerResponse {
    private:
        int m_channel;
    public:

        HandlerViewVideoStream(int channel) : m_channel(channel) {
        }
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}


#endif

