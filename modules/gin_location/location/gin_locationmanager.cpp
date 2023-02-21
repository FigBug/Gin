#if JUCE_IOS || JUCE_MAC
}

//==============================================================================
@interface LocationDelegate<CLLocationManagerDelegate> : NSObject {
    @public std::function<void (const gin::LocationManager::Location&)> onLocationChanged;
}

- (void)locationManagerDidChangeAuthorization:(CLLocationManager *)manager;
- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation*>*)locations;

@end

@implementation LocationDelegate

- (void)locationManagerDidChangeAuthorization:(CLLocationManager *)manager
{
}

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray<CLLocation*>*)locations
{
    for (CLLocation* src in locations)
    {
        gin::LocationManager::Location dst;

        dst.latitude    = src.coordinate.latitude;
        dst.longitude   = src.coordinate.longitude;
        dst.altitude    = src.altitude;
        dst.speed       = src.speed;
        dst.heading     = src.course;
        dst.time        = juce::Time (src.timestamp.timeIntervalSince1970 * 1000);

        onLocationChanged (dst);
    }
}

@end

namespace gin
{

//==============================================================================
class LocationManager::Impl
{
public:
    Impl (LocationManager& o)
    : owner (o)
    {
        delegate = [[LocationDelegate alloc] init];
        lm = [[CLLocationManager alloc] init];

        delegate->onLocationChanged = [this] (const gin::LocationManager::Location& v)
        {
            owner.listeners.call ([&] (LocationManager::Listener& l) { l.locationChanged (v); });
        };
    }

    ~Impl()
    {
        [lm release];
        [delegate release];
    }

    void start()
    {
        [lm startUpdatingLocation];
    }

    void stop()
    {
        [lm stopUpdatingLocation];
    }

    Authorization getAuthorization()
    {
        switch (lm.authorizationStatus)
        {
            case kCLAuthorizationStatusNotDetermined:
                return LocationManager::unknown;
            case kCLAuthorizationStatusRestricted:
                return LocationManager::restricted;
            case kCLAuthorizationStatusDenied:
                return LocationManager::denied;
            case kCLAuthorizationStatusAuthorizedAlways:
                return LocationManager::authorizedAlways;
#if JUCE_IOS
            case kCLAuthorizationStatusAuthorizedWhenInUse:
                return LocationManager::authorizedWhenInUse;
#endif
        }
    }

    void requestAlwaysAuthorization()
    {
        [lm requestAlwaysAuthorization];
    }

    LocationManager& owner;

    LocationDelegate* delegate = nullptr;
    CLLocationManager* lm = nullptr;
};

//==============================================================================
LocationManager::LocationManager ()
{
    impl = std::make_unique<Impl> (*this);
}

LocationManager::~LocationManager ()
{
}

LocationManager::Authorization LocationManager::getAuthorization()
{
    return impl->getAuthorization();
}

void LocationManager::requestAlwaysAuthorization()
{
    impl->requestAlwaysAuthorization();
}

void LocationManager::addListener (Listener* l)
{
    listeners.add (l);
}

void LocationManager::removeListener (Listener* l)
{
    listeners.remove (l);
}

void LocationManager::start()
{
    impl->start();
}

void LocationManager::stop()
{
    impl->stop();
}
#endif
