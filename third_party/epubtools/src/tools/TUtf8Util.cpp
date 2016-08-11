#include "TUtf8Util.h"

unsigned int TUtf8Util::UniCharToUTF8( wchar_t UniChar, char *OutUTFString )
{
    unsigned int UTF8CharLength = 0;

    if (UniChar < 0x80)
    {  
        if ( OutUTFString )
            OutUTFString[UTF8CharLength++] = (char)UniChar;
        else
            UTF8CharLength++;
    }
    else if(UniChar < 0x800)
    {
        if ( OutUTFString )
        {
            OutUTFString[UTF8CharLength++] = char(0xc0 | ( UniChar >> 6 ));
            OutUTFString[UTF8CharLength++] = char(0x80 | ( UniChar & 0x3f ));
        }
        else
        {
            UTF8CharLength += 2;
        }
    }
    else if(UniChar < 0x10000 )
    {
        if ( OutUTFString )
        {
            OutUTFString[UTF8CharLength++] = 0xe0 | ( UniChar >> 12 );
            OutUTFString[UTF8CharLength++] = 0x80 | ( (UniChar >> 6) & 0x3f );
            OutUTFString[UTF8CharLength++] = 0x80 | ( UniChar & 0x3f );
        }
        else
        {
            UTF8CharLength += 3;
        }
    }
    else if( UniChar < 0x200000 ) 
    {
        if ( OutUTFString )
        {
            OutUTFString[UTF8CharLength++] = 0xf0 | ( (int)UniChar >> 18 );
            OutUTFString[UTF8CharLength++] = 0x80 | ( (UniChar >> 12) & 0x3f );
            OutUTFString[UTF8CharLength++] = 0x80 | ( (UniChar >> 6) & 0x3f );
            OutUTFString[UTF8CharLength++] = 0x80 | ( UniChar & 0x3f );
        }
        else
        {
            UTF8CharLength += 4;
        }

    }

    return UTF8CharLength;
}

unsigned int TUtf8Util::UTF8StrToUnicode( const char* UTF8String, unsigned int UTF8StringLength, wchar_t* OutUnicodeString, unsigned int UnicodeStringBufferSize )
{
    unsigned int UTF8Index = 0;
    unsigned int UniIndex = 0;

    while ( UTF8Index < UTF8StringLength )
    {
        unsigned char UTF8Char = UTF8String[UTF8Index];

        if ( UnicodeStringBufferSize != 0 && UniIndex >= UnicodeStringBufferSize )
            break;

        if ((UTF8Char & 0x80) == 0) 
        {
            const unsigned int cUTF8CharRequire = 1;

            // UTF8×ÖÂë²»×ã
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString )
            {
                wchar_t& WideChar = OutUnicodeString[UniIndex]; 

                WideChar = UTF8Char;
            }

            UTF8Index++;

        } 
        else if((UTF8Char & 0xE0) == 0xC0)  ///< 110x-xxxx 10xx-xxxx
        {
            const unsigned int cUTF8CharRequire = 2;

            // UTF8×ÖÂë²»×ã
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString )
            {
                wchar_t& WideChar = OutUnicodeString[UniIndex]; 
                WideChar  = (UTF8String[UTF8Index + 0] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 1] & 0x3F);
            }

            UTF8Index += cUTF8CharRequire;
        }
        else if((UTF8Char & 0xF0) == 0xE0)  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
        {
            const unsigned int cUTF8CharRequire = 3;

            // UTF8×ÖÂë²»×ã
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString )
            {
                wchar_t& WideChar = OutUnicodeString[UniIndex]; 

                WideChar  = (UTF8String[UTF8Index + 0] & 0x1F) << 12;
                WideChar |= (UTF8String[UTF8Index + 1] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 2] & 0x3F);
            }


            UTF8Index += cUTF8CharRequire;
        } 
        else if((UTF8Char & 0xF8) == 0xF0)  ///< 1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 
        {
            const unsigned int cUTF8CharRequire = 4;

            // UTF8×ÖÂë²»×ã
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString )
            {
                wchar_t& WideChar = OutUnicodeString[UniIndex]; 

                WideChar  = (UTF8String[UTF8Index + 0] & 0x0F) << 18;
                WideChar  = (UTF8String[UTF8Index + 1] & 0x3F) << 12;
                WideChar |= (UTF8String[UTF8Index + 2] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 3] & 0x3F);
            }

            UTF8Index += cUTF8CharRequire;
        } 
        else ///< 1111-10xx 10xx-xxxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 
        {
            const unsigned int cUTF8CharRequire = 5;

            // UTF8×ÖÂë²»×ã
            if ( UTF8Index + cUTF8CharRequire > UTF8StringLength )
                break;

            if ( OutUnicodeString )
            {
                wchar_t& WideChar = OutUnicodeString[UniIndex]; 

                WideChar  = (UTF8String[UTF8Index + 0] & 0x07) << 24;
                WideChar  = (UTF8String[UTF8Index + 1] & 0x3F) << 18;
                WideChar  = (UTF8String[UTF8Index + 2] & 0x3F) << 12;
                WideChar |= (UTF8String[UTF8Index + 3] & 0x3F) << 6;
                WideChar |= (UTF8String[UTF8Index + 4] & 0x3F);
            }

            UTF8Index += cUTF8CharRequire;
        }


        UniIndex++;
    }

    return UniIndex;
}

std::string TUtf8Util::UniStrToUTF8Str( wstring uniStr )
{
    string utf8Str;
    char buf[5] = {0};
    unsigned int len = 0;
    for(unsigned int i = 0; i < uniStr.size(); ++i){
        len = UniCharToUTF8(uniStr[i], buf);
        if(len > 0){
            utf8Str.append(buf, len);
        }
    }
    return utf8Str;
}

std::wstring TUtf8Util::UTF8StrToUniStr( string uniStr )
{
    if (uniStr.empty())
    {
        return L"";
    }
    wchar_t* pwcode = new wchar_t[uniStr.size() + 1];
    unsigned int len = UTF8StrToUnicode(uniStr.c_str(), (int)uniStr.size(), pwcode, (int)uniStr.size());
    pwcode[len] = 0;
    wstring ret = pwcode;

    delete[] pwcode;
    pwcode = 0;
    return ret;
}
