#include "Particle.h"

#include "LocationFusionRK.h"

SerialLogHandler logHandler(LOG_LEVEL_INFO);

SYSTEM_MODE(SEMI_AUTOMATIC);

#ifndef SYSTEM_VERSION_v620
SYSTEM_THREAD(ENABLED); // System thread defaults to on in 6.2.0 and later and this line is not required
#endif

void setup() {
    LocationFusionRK::instance().setup();

    Particle.connect();
}

void loop() {
    
}
