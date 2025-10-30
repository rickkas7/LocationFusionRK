#ifndef __LOCATIONFUSIONRK_H
#define __LOCATIONFUSIONRK_H

#include "Particle.h"

// Repository: https://github.com/rickkas7/LocationFusionRK
// License: MIT


#ifndef SYSTEM_VERSION_v620
#error "The LocationFusionRK library requires Device OS 6.2.0 or later because it requires Variant and CloudEvent"
#endif

#include <vector>

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From global application setup you must call:
 * LocationFusionRK::instance().setup();
 */
class LocationFusionRK {
public:

#if Wiring_WiFi
    /**
     * @brief Class for holding information about a single Wi-Fi access point
     * 
     */
    class WAPEntry {
    public:
        WAPEntry();

        WAPEntry(const WiFiAccessPoint *wap);

        /**
         * @brief 
         * 
         * @param wap 
         */
        void fromWiFiAccessPoint(const WiFiAccessPoint *wap);

        /**
         * @brief Convert this object to JSON
         * 
         * @param writer JSONWriter to write the data to
         * @param wrapInObject true to wrap the data with writer.beginObject() and writer.endObject(). Default = true.
         */
        void toJsonWriter(JSONWriter &writer, bool wrapInObject = true) const;

        /**
         * @brief Save this data in a Variant object. Requires Device OS 6.2.0 or later.
         * 
         * @param obj Variant object to add to
         */
        void toVariant(Variant &obj) const;

        /**
         * @brief Convert to a string in 00-00-00-00-00-00 hex format
         * 
         * @return String 
         */
        String bssidString() const;

        uint8_t bssid[6];
        uint8_t channel;
        uint8_t reserved; 
        int rssi;
    };
#endif // Wiring_WiFi

#if Wiring_WiFi 
    /**
     * @brief Container for a list of Wi-Fi access points, along with methods for scanning and converting to JSON or Variant
     */
    class WAPList {
    public:
        void scan();

        size_t size() const { return wapArray.size(); };

        void appendEntry(const WAPEntry &entry);

        void appendEntry(const WiFiAccessPoint *wap);


        /**
         * @brief Convert this object to JSON
         * 
         * @param writer JSONWriter to write the data to
         * @param numToInclude Limit to this number of entries. 0 (default) is unlimited.
         */
        void toJsonWriter(JSONWriter &writer, int numToInclude = 0) const;

        /**
         * @brief Save this data in a Variant object. Requires Device OS 6.2.0 or later.
         * 
         * @param obj Variant object to add to
         * @param numToInclude Limit to this number of entries. 0 (default) is unlimited.
         */
        void toVariant(Variant &obj, int numToInclude = 0) const;

    protected:
        void scanCallback(WiFiAccessPoint* wap);

        static void scanCallbackStatic(WiFiAccessPoint* wap, void *context);

        std::vector<WAPEntry> wapArray;
    };
#endif // Wiring_WiFi

#if Wiring_Cellular
    class ServingTower {
    public:
        /**
         * @brief Get the current serving tower information.
         * 
         * @return int A system error code. SYSTEM_ERROR_NONE (0) is a success code, non-zero indicates an error. 
         * 
         * On;y available on cellular devices, and only works when connected to the cloud (breathing cyan).
         */
        int get();

        /**
         * @brief Get the result from the last get() call.
         * 
         * @return int A system error code. SYSTEM_ERROR_NONE (0) is a success code, non-zero indicates an error.  -1 if get() has not been called.
         */
        int getLastResult() const { return (int)cellularResult; };

        /**
         * @brief Convert this object to JSON
         * 
         * @param writer JSONWriter to write the data to
         * @param wrapInObject true (default) to surround with beginObject() and endObject()
         */
        void toJsonWriter(JSONWriter &writer, bool wrapInObject = true) const;

        /**
         * @brief Save this data in a Variant object. Requires Device OS 6.2.0 or later.
         * 
         * @param obj Variant object to add to
         */
        void toVariant(Variant &obj) const;
        /**
         * @brief Return the current CellularGlobalIdentity. Only valid after get() is called.
         * 
         * @return const CellularGlobalIdentity& 
         */
        const CellularGlobalIdentity &getCellularGlobalIdentity() const { return cgi; };

    protected:
        CellularGlobalIdentity cgi = {0};
        cellular_result_t cellularResult = -1;

    };
#endif // Wiring_Cellular
    /**
     * @brief How often to publish location 
     */
    enum class PublishFrequency {
        manual,
        once,
        periodic
    };

    /**
     * @brief This class is used internally for registerCommand
     */
    struct CmdHandler {
        String cmd;
        std::function<void(const Variant &data)> handler;
    };


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

    LocationFusionRK &withPublishManual() { publishFrequency = PublishFrequency::manual; return *this; };

    LocationFusionRK &withPublishOnce() { publishFrequency = PublishFrequency::once; return *this; };

    LocationFusionRK &withPublishPeriodic(std::chrono::milliseconds ms) { publishFrequency = PublishFrequency::periodic; publishPeriod = ms; return *this; };


    PublishFrequency getPublishFrequency() const { return publishFrequency; };


    LocationFusionRK &withAddWiFi(bool enable = true) { addWiFi = enable; return *this; };

    LocationFusionRK &withAddTower(bool enable = true) { addTower = enable; return *this; };

    LocationFusionRK &withAddToEventHandler(std::function<void(Variant &eventData, Variant &locVariant)> handler) { addToEventHandlers.push_back(handler); return *this; };
    

    LocationFusionRK &withCmdHandler(const char *cmd, std::function<void(const Variant &data)> handler);

    LocationFusionRK &withLocEnhancedHandler(std::function<void(const Variant &data)> handler) { locEnhancedHandlers.push_back(handler); return *this; };


    /**
     * @brief Request a publish now
     * 
     */
    void requestPublish() { manualPublishRequested = true; };

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

    void stateIdle();

    void stateConnected();

    void stateBuildPublish();

    void statePublishWait();

    void subscriptionHandler(const Variant &eventData);
    static void subscriptionHandlerStatic(CloudEvent event);


    void locEnhanced(const Variant &eventData);
    static void locEnhancedStatic(const Variant &eventData);

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

    PublishFrequency publishFrequency = PublishFrequency::manual;

    std::chrono::milliseconds publishPeriod = 5min;

    std::chrono::milliseconds publishFailureRetry = 1min;

    std::function<void(LocationFusionRK &)> stateHandler = &LocationFusionRK::stateIdle;

    bool addWiFi = false;
    bool addTower = false;
    std::vector<std::function<void(Variant &eventData, Variant &locVariant)>> addToEventHandlers;

    bool enableCmdFunction = true;
    std::vector<CmdHandler> commandHandlers;

    std::vector<std::function<void(const Variant &eventData)>> locEnhancedHandlers;

    bool manualPublishRequested = false;

    int publishCount = 0;

    CloudEvent event;
    Variant eventData;

    uint64_t nextPublishMs = 0;

    int locRequestId = 1;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static LocationFusionRK *_instance;

};
#endif  /* __LOCATIONFUSIONRK_H */
