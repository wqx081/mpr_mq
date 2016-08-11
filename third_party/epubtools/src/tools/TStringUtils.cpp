//
//  TStringUtils.cpp
//  libEpub
//
//  Created by MPR on 15/4/3.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#include "TStringUtils.h"
#include <algorithm>
#include <ctype.h>


void TStringUtils::Split(const std::string& str, const std::string& delim, std::vector<std::string> *ret)
{
    size_t last  = 0;
    size_t index = str.find(delim, last);
    while (index != std::string::npos)
    {
        ret->push_back(str.substr(last, index - last));
        last  = index + 1;
        index = str.find_first_of(delim, last);
    }
    if (index-last>0)
    {
        ret->push_back(str.substr(last, index - last));
    }
}

std::string TStringUtils::StringFromInt(int v)
{
    char buffer[33] = {0};
    sprintf(buffer, "%d", v);
    return buffer;
}

wchar_t charToLowerW(wchar_t in){
    if(in >= L'A' && in <= L'Z')
        return in - (L'Z' - L'z');
    return in;
}

char charToLower(char in){
    if(in >= 'A' && in <= 'Z')
        return in - ('Z'-'z');
    return in;
}

std::string TStringUtils::ToLower(const std::string& str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), charToLower);
    return lowerStr;
}

std::wstring TStringUtils::ToLower(const std::wstring& str)
{
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), charToLowerW);
    return lowerStr;
}

std::string & TStringUtils::ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](int c){return isspace(c); }));
    return s;
}

std::wstring & TStringUtils::ltrim(std::wstring &s)
{
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](wint_t c){
        return c == 0x20 //' '
            || c == 0x0c //'\f'
            || c == 0x0a //'\n'
            || c == 0x0d //'\r'
            || c == 0x09 //'\t'
            || c == 0x0b //'\v'
        || c == 0x3000;  //全角空格
    }));
//    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](wint_t c){ return iswspace(c);}));
    return s;
}

TStrings TStringUtils::Split(const TString& str, const TString& delim)
{
    TStrings ret;
    
    size_t last  = 0;
    size_t index = str.find(delim, last);
    while (index != std::string::npos)
    {
        ret.push_back(str.substr(last, index - last));
        last  = index + 1;
        index = str.find_first_of(delim, last);
    }
    if (index-last>0)
    {
        ret.push_back(str.substr(last, index - last));
    }
    
    return ret;
}

bool TStringUtils::Contains(const TStrings& strs, const TString& str)
{
    for (size_t i = 0; i < strs.size(); ++ i) {
        if (strs.at(i) == str) {
            return true;
        }
    }
    return false;
}

bool TStringUtils::StartWith(const TString& str, const TString& sub)
{
    if (str.size() >= sub.size()) {
        return str.substr(0, sub.size()) == sub;
    }
    return false;
}

TWString& TStringUtils::ReplaceAll(TWString& str, const TWString& toReplace, const TWString& newStr)
{
    if (!str.empty() && !toReplace.empty() && toReplace != newStr) {
        size_t pos = 0;
        while ((pos = str.find(toReplace, pos)) != TWString::npos) {
            str.replace(pos, toReplace.length(), newStr);
            pos += newStr.length();
        }
    }
    return str;
}
