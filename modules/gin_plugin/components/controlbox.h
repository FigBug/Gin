#pragma once

class ControlBox : public Component
{
public:
    ControlBox()
    {
        
    }
    
    Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1)
    {
        if (auto p = findParentComponentOfClass<GinAudioProcessorEditor>())
        {
            auto rc = p->getGridArea (0, 0);
            int cx = rc.getWidth();
            int cy = rc.getHeight();
            return Rectangle<int> (x * cx, y * cy, w * cx, h * cy);
        }
        
        return {};
    }
};
