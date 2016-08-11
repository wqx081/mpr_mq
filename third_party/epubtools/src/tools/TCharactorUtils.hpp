//
//  TCharactorUtils.hpp
//  libEpub
//
//  Created by tanwz on 15/12/8.
//
//

#ifndef TCharactorUtils_hpp
#define TCharactorUtils_hpp

#include <stdio.h>

class TCharactorUtils
{
public:
    static bool IsPunctuation(wchar_t ch);
    static bool IsWhiteSpace(wchar_t ch);
    static bool IsNewLine(wchar_t ch);
    static bool IsLatinLetter(wchar_t ch);
    static bool IsDigital(wchar_t ch);
    static bool IsPinYin(wchar_t ch);
    static bool IsForbiddenBeginOfLine(wchar_t ch);
    static bool IsForbiddenEndOfLine(wchar_t ch);

public:
    static const wchar_t Disc   = 0x25AA;
    static const wchar_t Circle = 0x2022;
    static const wchar_t Square = 0x25a0;
    static const wchar_t Star   = 0x22c6;
};

//const wchar_t TCharactorUtils::Disc   = 0x25AA;
//const wchar_t TCharactorUtils::Circle = 0x2022;
//const wchar_t TCharactorUtils::Square = 0x25a0;
//const wchar_t TCharactorUtils::Star   = 0x22c6;

#endif /* TCharactorUtils_hpp */
