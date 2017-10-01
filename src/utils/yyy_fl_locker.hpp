#pragma once

#include <FL/Fl.H>

namespace YYY {

struct FlLocker {
    FlLocker() { Fl::lock(); }
    ~FlLocker() { Fl::unlock(); }
};

} // namespace YYY
