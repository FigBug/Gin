/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

struct BlockingData
{
    std::function<void ()> func;
};

static void* blockingCallback (void* param)
{
    auto data = (BlockingData*)param;

    if (data->func)
        data->func();

    delete data;
    return nullptr;
}

void callOnMainThreadBlocking ( std::function<void ()> func )
{
    auto data = new BlockingData();
    data->func = func;

    juce::MessageManager::getInstance()->callFunctionOnMessageThread (blockingCallback, data);
}
