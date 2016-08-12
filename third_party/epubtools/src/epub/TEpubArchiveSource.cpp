//
//  TEpubArchiveSource.cpp
//  libEpub
//
//  Created by MPR on 15/1/13.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#include "TEpubArchiveSource.h"
#include "ZipArchiveReader.h"
#include <string.h>
#include <stdlib.h>

TEpubArchiveSource::TEpubArchiveSource(const std::string& archivePath, const std::string& epubPath, unsigned char key[32], int keyType)
    : m_archivePath(archivePath)
    , m_epubPath(epubPath)
    , m_needDecrypt(true)
    , m_keyType(keyType)
{
    memcpy(m_key, key, 32);
    m_zipReader = new ZipArchiveReader();
}

TEpubArchiveSource::TEpubArchiveSource(const std::string& archivePath)
    : m_archivePath(archivePath)
    , m_needDecrypt(false)
{
    m_zipReader = new ZipArchiveReader();
}

TEpubArchiveSource::~TEpubArchiveSource()
{
    delete m_zipReader;
}

bool TEpubArchiveSource::Open()
{
    if (m_needDecrypt) {
        TDecryptInputInfo decryptInfo{0};
        memcpy(&decryptInfo.key, m_key, 16);
        decryptInfo.algoType = m_keyType;
        
        return m_zipReader->Open(m_archivePath, m_epubPath, decryptInfo);
    }
    return m_zipReader->Open(m_archivePath, 1);
}

bool TEpubArchiveSource::IsOpen() const
{
    return m_zipReader->IsOpen();
}

void TEpubArchiveSource::Close()
{
    m_zipReader->Close();
}

bool TEpubArchiveSource::ExistItem(const std::string& itemPath)
{
    return m_zipReader->ExistItem(itemPath);
}

int TEpubArchiveSource::ItemSize(const std::string& itemPath)
{
    return (int)m_zipReader->ItemSize(itemPath);
}

int TEpubArchiveSource::ReadAllForItem(const std::string& itemPath, void *buffer, int bufferLen)
{
    return m_zipReader->ReadAllForItem(itemPath, buffer, bufferLen);
}
