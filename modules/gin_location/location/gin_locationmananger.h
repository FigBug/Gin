#pragma once

#if JUCE_IOS || JUCE_MAC

class LocationManager
{
public:
    LocationManager();
    ~LocationManager();

    void start();
    void stop();

    //==============================================================================
    enum Authorization
    {
        unknown,
        restricted,
        denied,
        authorizedAlways,
        authorizedWhenInUse,
    };

    Authorization getAuthorization();
    void requestAlwaysAuthorization();

    //==============================================================================
    struct Location
    {
        double      latitude    = 0.0;
        double      longitude   = 0.0;
        double      altitude    = 0.0;
        double      speed       = 0.0;
        double      heading     = 0.0;
        juce::Time  time;
    };

    //==============================================================================
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void locationChanged (const Location&) {};
    };

    void addListener (Listener*);
    void removeListener (Listener*);

    //==============================================================================
private:
    class Impl;
    std::unique_ptr<Impl>   impl;

    juce::ListenerList<Listener>  listeners;
};

#endif
