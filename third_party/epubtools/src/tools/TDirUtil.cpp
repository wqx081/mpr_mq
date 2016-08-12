//
//  TDirUtil.cpp
//  libEpub
//
//  Created by MPR on 15/1/13.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#include "TDirUtil.h"

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include "win_dirent.h"
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <assert.h>
#include <string.h>
#include <stdlib.h>


std::vector<std::string> TDirUtil::listAllFile(const std::string& dirPath)
{
    std::vector<std::string> fileList;
    if (dirPath.empty()) {
        return fileList;
    }
    
    std::string _dirPath = dirPath;
    if (_dirPath[_dirPath.length() - 1] == '/') {
        _dirPath.erase(_dirPath.length() - 1, 1);
    }
    
    innerListFile(_dirPath, "", fileList);
    
    return fileList;
}

void TDirUtil::innerListFile(const std::string& dirPath, const std::string& prefix, std::vector<std::string>& files)
{
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir = opendir(dirPath.c_str());
    if (pDir) {
        while (NULL != (ent = readdir(pDir))) {
            if (ent->d_type == DT_DIR) {
                //dir
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }
                
                std::string subDirPath = dirPath + "/" + ent->d_name;
                
                std::string _prefix = prefix;
                _prefix += ent->d_name;
                _prefix += "/";
                
                innerListFile(subDirPath, _prefix, files);
            } else {
                files.push_back(prefix + ent->d_name);
            }
        }
        closedir(pDir);
    }
}


