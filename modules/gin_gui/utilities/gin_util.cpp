/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "gin_platform.h"

juce::Colour goldenRatioColor (int idx)
{
    double h = std::fmod (idx * 0.618033988749895, 1);
    return juce::Colour (float (h), 0.8f, 0.95f, 1.0f);
}

void asyncDownload (const juce::URL& url, std::function<void (const juce::String&)> callback)
{
    new AsyncDownload (url, [callback] (AsyncDownload* dl, const juce::MemoryBlock& mb, bool ok)
    {
        if (ok)
            callback (mb.toString());
        else
            callback ({});

        delete dl;
    });
}

void asyncDownload (const juce::URL& url, std::function<void (const juce::MemoryBlock&)> callback)
{
    new AsyncDownload (url, [callback] (AsyncDownload* dl, const juce::MemoryBlock& mb, bool ok)
    {
        if (ok)
            callback (mb);
        else
            callback ({});

        delete dl;
    });
}

juce::Image createDesktopSnapshot()
{
#ifdef JUCE_WINDOWS
    HDC hScreenDC = GetDC (nullptr); // Get the screen device context
    HDC hMemoryDC = CreateCompatibleDC (hScreenDC); // Create a compatible memory device context

    int width = GetDeviceCaps (hScreenDC, HORZRES);
    int height = GetDeviceCaps (hScreenDC, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap (hScreenDC, width, height);
    HBITMAP hOldBitmap = static_cast<HBITMAP> (SelectObject (hMemoryDC, hBitmap));

    BitBlt (hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    hBitmap = static_cast<HBITMAP>(SelectObject (hMemoryDC, hOldBitmap)); // Reselect the original bitmap
    DeleteDC (hMemoryDC);
    DeleteDC (hScreenDC);

    auto img = gin::windows::hBitmapToImage (hBitmap);

    DeleteObject (hBitmap);

    return img;
#else
    jassertfalse; //todo
    return {};
#endif
}

juce::Image createSnapshotOfNativeWindow (juce::Component& c)
{
#ifdef JUCE_WINDOWS
    if (auto peer = c.getPeer())
    {
        if (auto hWnd = (HWND)peer->getNativeHandle())
        {
            RECT rect;
            GetWindowRect (hWnd, &rect);

            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            HDC hScreenDC = GetDC (NULL);
            HDC hCompatibleDC = CreateCompatibleDC (hScreenDC);
            HBITMAP hBitmap = CreateCompatibleBitmap (hScreenDC, width, height);

            SelectObject (hCompatibleDC, hBitmap);

            // Get the window's client area DC
            HDC hWindowDC = GetWindowDC (hWnd); 

            BitBlt (hCompatibleDC, 0, 0, width, height, hWindowDC, 0, 0, SRCCOPY);

            auto img = gin::windows::hBitmapToImage (hBitmap);

            ReleaseDC (NULL, hScreenDC);
            ReleaseDC (hWnd, hWindowDC); // Release the window DC
            DeleteDC (hCompatibleDC);
            DeleteObject (hBitmap);

            return img;
        }
    }
    return {};
#else
    juce::ignoreUnused (c);
    jassertfalse; // todo
    return {};
#endif
}
