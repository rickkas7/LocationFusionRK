#include "LocationFusionRK.h"

LocationFusionRK *LocationFusionRK::_instance;

// [static]
LocationFusionRK &LocationFusionRK::instance() {
    if (!_instance) {
        _instance = new LocationFusionRK();
    }
    return *_instance;
}

LocationFusionRK::LocationFusionRK() {
}

LocationFusionRK::~LocationFusionRK() {
}

void LocationFusionRK::setup() {
    os_mutex_create(&mutex);

    thread = new Thread("LocationFusionRK", [this]() { return threadFunction(); }, OS_THREAD_PRIORITY_DEFAULT, 3072);
}

os_thread_return_t LocationFusionRK::threadFunction(void) {
    while(true) {
        // Put your code to run in the worker thread here
        delay(1);
    }
}

