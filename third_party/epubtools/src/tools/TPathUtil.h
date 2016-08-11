//
//  TPathUtil.h
//  libEpub
//
//  Created by MPR on 14/12/15.
//  Copyright (c) 2014年 mprtimes. All rights reserved.
//

#ifndef __libEpub__TPathUtil__
#define __libEpub__TPathUtil__

#include <stdio.h>
#include <string>

class TPathUtil
{
public:
    static std::string GetAbsolutePath(const std::string& path, const std::string& basePath);
    static std::string GetParentPath(const std::string& path);
    static std::string GetReletivePath(const std::string& completePath, const std::string& base);
    static std::string CombinePath(const std::string& dir, const std::string& fileName);
    static std::string GetExtension(const std::string& path);
    static std::string GetLastComponent(const std::string& path);
    static std::string StringByDeleteExtension(const std::string& path);
    
private:
    static void NormalizePath(std::string& path, std::string& basePath);
};

#endif
