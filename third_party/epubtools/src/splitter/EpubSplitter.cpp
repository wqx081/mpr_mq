//
//  EpubSplitter.cpp
//  ePubSplitter
//
//  Created by tanwz on 16/4/18.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#include "EpubSplitter.h"
#include "EpubPackage.h"
#include "TEpubArchiveSource.h"


EpubSplitter::EpubSplitter(const std::string& filePath)
{
    mEpubSource = new TEpubArchiveSource(filePath);
    mPackage = new EpubPackage(mEpubSource);
    mPackage->Open();
}

EpubSplitter::~EpubSplitter()
{
    mPackage->Close();
    delete mPackage;
    
    delete mEpubSource;
}

int EpubSplitter::GetSectionCount()
{
    return mPackage->GetSpineCount();
}

bool EpubSplitter::Split(int section, const std::string& outFilePath)
{
    bool ret = false;
    if (mPackage->IsOpened()) {
        std::set<int> spines;
        spines.insert(section);
        
        EpubPackage *subPackage = mPackage->SubPackage(spines);
        if (subPackage) {
            ret = subPackage->Save(outFilePath);
            delete subPackage;
        }
    }
    return ret;
}
