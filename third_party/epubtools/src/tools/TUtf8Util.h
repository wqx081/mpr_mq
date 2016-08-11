#pragma once
#ifndef __TUTF8UTIL_H__
#define __TUTF8UTIL_H__
#include <string>
using namespace std;

class TUtf8Util{
public:
    static string UniStrToUTF8Str(wstring uniStr);
    static wstring UTF8StrToUniStr(string uniStr);
    static unsigned int UniCharToUTF8(wchar_t UniChar, char *OutUTFString);
    static unsigned int UTF8StrToUnicode( const char* UTF8String, unsigned int  UTF8StringLength, wchar_t* OutUnicodeString, unsigned int  UnicodeStringBufferSize );
};

#endif // __TUTF8UTIL_H__
