#include <string>

namespace vms {
    namespace httpvideostream {
        std::string base64_encode(char const*, unsigned int len);
        std::string base64_decode(std::string const& s);
    }
}

