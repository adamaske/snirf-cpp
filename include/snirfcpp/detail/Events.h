#pragma once
#include <string>
#include <vector>

namespace SNIRFCPP {

    struct EventMarker {
        double onset;    // seconds
        double duration; // seconds
        double value;    // optional
    };

    struct Event {
        std::string name;
        std::vector<EventMarker> markers;
    };

    struct EventsContainer {
        std::vector<Event> events;
    };

} // namespace SNIRFCPP
