//
//  EpubSplitter.h
//  ePubSplitter
//
//  Created by tanwz on 16/4/18.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#ifndef EpubSplitter_h
#define EpubSplitter_h

#include <stdio.h>
#include <string>

class EpubPackage;
class TEpubSource;
class EpubSplitter
{
public:
    EpubSplitter(const std::string& filePath);
    ~EpubSplitter();
    
    int GetSectionCount();
    bool Split(int section, const std::string& outFilePath);
    
private:
    EpubPackage *mPackage;
    TEpubSource *mEpubSource;
};

#endif /* EpubSplitter_h */
