//
//  TDirUtil.h
//  libEpub
//
//  Created by MPR on 15/1/13.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#ifndef __libEpub__TDirUtil__
#define __libEpub__TDirUtil__

#include <stdio.h>
#include <vector>
#include <string>

class TDirUtil
{
public:
    static std::vector<std::string> listAllFile(const std::string& dirPath);
    
private:
    static void innerListFile(const std::string& dirPath, const std::string& prefix, std::vector<std::string>& files);
};

#endif
