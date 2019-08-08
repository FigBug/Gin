/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class Http
{
public:
    struct HttpResult
    {
        int statusCode = 0;
        StringArray headers;
        MemoryBlock data;
    };
    
    Http (URL url);
    
    HttpResult get();
    
private:
    

    URL url;
};
