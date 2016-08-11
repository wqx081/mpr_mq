//
//  EpubArchiveSource.h
//  libEpub
//
//  Created by MPR on 15/1/13.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#ifndef __libEpub__EpubArchiveSource__
#define __libEpub__EpubArchiveSource__

#include <stdio.h>
#include "TEpubSource.h"

class ZipArchiveReader;
class TEpubArchiveSource: public TEpubSource
{
public:
    explicit TEpubArchiveSource(const std::string& archivePath, const std::string& epubPath, unsigned char key[32], int keyType);
    explicit TEpubArchiveSource(const std::string& archivePath);
    
    virtual ~TEpubArchiveSource();
    
    virtual bool Open();
    virtual bool IsOpen() const;
    virtual void Close();
    
    virtual bool ExistItem(const std::string& itemPath);
    virtual int  ItemSize(const std::string& itemPath);
    virtual int  ReadAllForItem(const std::string& itemPath, void *buffer, int bufferLen);
    
private:
    ZipArchiveReader *m_zipReader;
    
    std::string m_archivePath;
    std::string m_epubPath;
    bool        m_needDecrypt;
    unsigned char m_key[32];
    int m_keyType;
};

#endif
