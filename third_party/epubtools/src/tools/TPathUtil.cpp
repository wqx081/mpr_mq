//
//  TPathUtil.cpp
//  libEpub
//
//  Created by MPR on 14/12/15.
//  Copyright (c) 2014年 mprtimes. All rights reserved.
//

#include "TPathUtil.h"
#include "TStringUtils.h"

std::string TPathUtil::GetAbsolutePath(const std::string& path, const std::string& basePath)
{
    if (!basePath.empty() && !path.empty()) {
        std::string complete_base_path = basePath;
        if (basePath.at(basePath.length() - 1) != '/') {
            complete_base_path = basePath + "/";
        }
        
        std::string path_body = path;
        if (path.at(0) == '/') {
            path_body = path.substr(1);
        }
        NormalizePath(path_body, complete_base_path);
        
        return complete_base_path + path_body;
    }
    
    return path;
}

std::string TPathUtil::GetParentPath(const std::string& path)
{
    if (path.length() > 0) {
        int index = (int)path.rfind('/');
        if (path[path.length() - 1] == '/' && path.length() > 1) {
            index = (int)path.rfind('/', path.length() - 2);
        }
        
        if (index != -1) {
            return path.substr(0, index + 1);
        }
    }
    return "";
}

std::string TPathUtil::GetReletivePath(const std::string& completePath, const std::string& base)
{
    if (base.empty()) {
        return completePath;
    }
    
    TStrings baseCoponents = TStringUtils::Split(base, "/");
    TStrings pathCoponents = TStringUtils::Split(completePath, "/");
    
    for (size_t i = 0; i < baseCoponents.size(); ++ i) {
        if (i < pathCoponents.size() && !baseCoponents.at(i).empty()) {
            if (baseCoponents.at(i) == pathCoponents.at(i)) {
                pathCoponents[i] = "";
            } else {
                pathCoponents.insert(pathCoponents.begin(), "..");
            }
        }
    }
    
    std::string reletivePath;
    for (size_t i = 0; i < pathCoponents.size(); ++ i) {
        if (pathCoponents.at(i).empty()) {
            continue;
        }
        
        if (!reletivePath.empty()) {
            reletivePath += "/";
        }
        reletivePath += pathCoponents.at(i);
    }
    return reletivePath;
}

std::string TPathUtil::CombinePath(const std::string& dir, const std::string& fileName)
{
    if (dir.empty()) {
        return fileName;
    }
    
    if (dir[dir.length() - 1] == '/') {
        return (dir + fileName);
    } else {
        return (dir + "/" + fileName);
    }
}

std::string TPathUtil::GetExtension(const std::string& path)
{
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return "";
}

std::string TPathUtil::GetLastComponent(const std::string& path)
{
    std::string::size_type pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}

std::string TPathUtil::StringByDeleteExtension(const std::string& path)
{
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        return path.substr(0, pos);
    }
    return path;
}

void TPathUtil::NormalizePath(std::string& path, std::string& basePath)
{
    if (path.empty() || basePath.empty()) {
        return;
    }
    
    int oblique_index = (int)path.find('/');
    if (oblique_index != -1) {
        std::string first_component = path.substr(0, oblique_index + 1);
        if (first_component == "./" || first_component == "../") {
            if (first_component == "../") {
                if (basePath.length() > 1) {
                    int index = (int)basePath.rfind('/', basePath.length() - 2);
                    if (index != -1) {
                        basePath = basePath.substr(0, index + 1);
                    } else {
                        basePath = "";
                    }
                } else {
                    basePath = "";
                }
            }
            
            path = path.substr(oblique_index + 1);
            
            NormalizePath(path, basePath);
        }
    }
}
