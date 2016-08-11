//
//  TStringUtils.h
//  libEpub
//
//  Created by MPR on 15/4/3.
//  Copyright (c) 2015年 mprtimes. All rights reserved.
//

#ifndef __libEpub__TStringUtils__
#define __libEpub__TStringUtils__

#include <stdio.h>
#include <string>
#include <vector>

typedef std::vector<std::string> TStrings;
typedef std::string TString;

typedef std::vector<std::wstring> TWStrings;
typedef std::wstring TWString;

class TStringUtils
{
public:
    static void Split(const std::string& str, const std::string& delim, TStrings *ret);
    static std::string StringFromInt(int v);
    static std::string ToLower(const std::string& str);
    static std::wstring ToLower(const std::wstring& str);
    static std::string &ltrim(std::string &s);
    static std::wstring &ltrim(std::wstring &s);
    
    static TStrings Split(const TString& str, const TString& delim);
    static bool Contains(const TStrings& strs, const TString& str);
    static bool StartWith(const TString& str, const TString& sub);
    
    static TWString& ReplaceAll(TWString& str, const TWString& toReplace, const TWString& newStr);
};

#endif /* defined(__libEpub__TStringUtils__) */
