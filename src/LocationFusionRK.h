#ifndef __LOCATIONFUSIONRK_H
#define __LOCATIONFUSIONRK_H

#include "Particle.h"

// Repository: https://github.com/rickkas7/LocationFusionRK
// License: MIT

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From global application setup you must call:
 * LocationFusionRK::instance().setup();
 */
class LocationFusionRK {
public:
    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use LocationFusionRK::instance() to instantiate the singleton.
     */
    static LocationFusionRK &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use LocationFusionRK::instance().setup();
     */
    void setup();

    /**
     * @brief Locks the mutex that protects shared resources
     * 
     * This is compatible with `WITH_LOCK(*this)`.
     * 
     * The mutex is not recursive so do not lock it within a locked section.
     */
    void lock() { os_mutex_lock(mutex); };

    /**
     * @brief Attempts to lock the mutex that protects shared resources
     * 
     * @return true if the mutex was locked or false if it was busy already.
     */
    bool tryLock() { return os_mutex_trylock(mutex); };

    /**
     * @brief Unlocks the mutex that protects shared resources
     */
    void unlock() { os_mutex_unlock(mutex); };


protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use LocationFusionRK::instance() to instantiate the singleton.
     */
    LocationFusionRK();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~LocationFusionRK();

    /**
     * This class is a singleton and cannot be copied
     */
    LocationFusionRK(const LocationFusionRK&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    LocationFusionRK& operator=(const LocationFusionRK&) = delete;

    /**
     * @brief Worker thread function
     * 
     * This method is called to perform operations in the worker thread.
     * 
     * You generally will not return from this method.
     */
    os_thread_return_t threadFunction(void);

    /**
     * @brief Mutex to protect shared resources
     * 
     * This is initialized in setup() so make sure you call the setup() method from the global application setup.
     */
    os_mutex_t mutex = 0;

    /**
     * @brief Worker thread instance class
     * 
     * This is initialized in setup() so make sure you call the setup() method from the global application setup.
     */
    Thread *thread = 0;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static LocationFusionRK *_instance;

};
#endif  /* __LOCATIONFUSIONRK_H */
