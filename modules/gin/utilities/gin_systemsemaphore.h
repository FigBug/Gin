/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
 * A semaphore that works across processes and in one process
 * This is not finished / implemented
 */
class SystemSemaphore
{
public:
    SystemSemaphore (const juce::String& name);
    ~SystemSemaphore();

    bool lock();
    bool unlock();

    bool isValid();

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemSemaphore)
};
