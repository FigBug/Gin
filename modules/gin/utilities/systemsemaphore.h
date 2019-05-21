/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/*
 * This is not finished / implemented
 */
class SystemSemaphore
{
public:
    SystemSemaphore (const String& name);
    ~SystemSemaphore();

    bool lock();
    bool unlock();

    bool isValid();

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemSemaphore)
};
