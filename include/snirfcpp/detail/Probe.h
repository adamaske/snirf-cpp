#pragma once
#include <map>
#include <vector>
#include "snirfcpp/detail/Vec.h"

namespace SNIRFCPP {

    using ChannelID    = uint32_t;
    using ChannelValue = double;
    using ChannelData  = std::vector<ChannelValue>;

    using OptodeID = uint32_t;

    enum OptodeType {
        SOURCE,
        DETECTOR,
    };

    struct Optode {
        OptodeType type;
        OptodeID   id;

        Vec2 position_2D;
        Vec3 position_3D;
    };
    using OptodeMap = std::map<OptodeID, Optode>;

    struct Channel {
        ChannelID id;

        OptodeID source_id;   // 1-indexed
        OptodeID detector_id;

        ChannelData hbo_data;
        ChannelData hbr_data;
        ChannelData hbt_data;
    };
    using ChannelMap = std::map<ChannelID, Channel>;

    struct Probe {
        ChannelMap  channels;
        OptodeMap   sources;
        OptodeMap   detectors;

        std::vector<int> wavelengths;
    };

} // namespace SNIRFCPP
