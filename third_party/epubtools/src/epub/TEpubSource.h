//
//  EpubStream.h
//  libEpub
//
//  Created by MPR on 15/1/13.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#ifndef libEpub_EpubStream_h
#define libEpub_EpubStream_h

#include <string>

class TEpubSource
{
public:
    virtual ~TEpubSource(){};
    
    virtual bool Open() = 0;
    virtual bool IsOpen() const = 0;
    virtual void Close() = 0;
    
    virtual bool ExistItem(const std::string& itemPath) = 0;
    virtual int  ItemSize(const std::string& itemPath) = 0;
    virtual int  ReadAllForItem(const std::string& itemPath, void *buffer, int bufferLen) = 0;
};

#endif
