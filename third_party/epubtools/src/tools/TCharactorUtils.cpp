//
//  TCharactorUtils.cpp
//  libEpub
//
//  Created by tanwz on 15/12/8.
//
//

#include "TCharactorUtils.hpp"


bool TCharactorUtils::IsPunctuation(wchar_t ch)
{
    return (0x3002 == ch      //。
        || 0xff1f == ch   //？
        || 0xff01 == ch   //！
        || 0xff0c == ch   //，
        || 0x3001 == ch   //、
        || 0xff1b == ch   //；
        || 0xff1a == ch   //：
        || 0x201c == ch   //“
        || 0x201d == ch   //”
        || 0x2019 == ch   //’
        || 0x2018 == ch   //‘
        || 0xff08 == ch   //（
        || 0xff09 == ch   //）
        || 0x002d == ch   //-
        || 0xff5e == ch   //～
        || 0x300a == ch   //《
        || 0x300b == ch   //》
        || 0x3011 == ch   //【
        || 0x3011 == ch   //】
        || 0x2026 == ch   //…
        || 0x2014 == ch   //—
        
        || 0x002e == ch   //.
        || 0x003f == ch   //?
        || 0x0021 == ch   //!
        || 0x002c == ch   //,
        || 0x003a == ch   //:
        || 0x003b == ch   //;
        || 0x002d == ch   //-
        || 0x0028 == ch   //(
        || 0x0029 == ch   //)
        || 0x005b == ch   //[
        || 0x005d == ch   //]
        || 0x007b == ch   //{
        || 0x007d == ch   //}
        || 0x0022 == ch   //"
        || 0x0027 == ch   //'
        || 0x0060 == ch   //`
        || 0x00b7 == ch   //·
            );
}

bool TCharactorUtils::IsWhiteSpace(wchar_t ch)
{
    return (0x09 == ch
            || 0x20 == ch
            || 0x0c == ch //'\f'
            || 0x0a == ch //'\n'
            || 0x0d == ch //'\r'
            || 0x0b == ch //'\v'
            
            || 0x2002 == ch //&ensp;
            || 0x2003 == ch //&emsp;
            || 0x2009 == ch //&thinsp;
            || 0x3000 == ch //&idsp
            );
}

bool TCharactorUtils::IsNewLine(wchar_t ch)
{
    return (0x0a == ch
            || 0x0d == ch
            || 0x85 == ch
            );
}

bool TCharactorUtils::IsLatinLetter(wchar_t ch)
{
    return (ch >= 0x41 && ch <= 0x5a)
        || (ch >= 0x61 && ch <= 0x7a);
}

bool TCharactorUtils::IsDigital(wchar_t ch)
{
    return (ch >= '0' && ch <= '9');
}

bool TCharactorUtils::IsPinYin(wchar_t ch)
{
    return (ch == 0x0101    //ā
            || ch == 0x00e1 //á
            || ch == 0x01ce //ǎ
            || ch == 0x00e0 //à
            || ch == 0x014d //ō
            || ch == 0x00f3 //ó
            || ch == 0x01d2 //ǒ
            || ch == 0x00f2 //ò
            || ch == 0x0113 //ē
            || ch == 0x00e9 //é
            || ch == 0x011b //ě
            || ch == 0x00e8 //è
            || ch == 0x012b //ī
            || ch == 0x00ed //í
            || ch == 0x01d0 //ǐ
            || ch == 0x00ec //ì
            || ch == 0x016b //ū
            || ch == 0x00fa //ú
            || ch == 0x01d4 //ǔ
            || ch == 0x00f9 //ù
            || ch == 0x00fc //ü
            || ch == 0x01d6 //ǖ
            || ch == 0x01d8 //ǘ
            || ch == 0x01da //ǚ
            || ch == 0x01dc //ǜ
            || ch == 0x00ea //ê
            );
}

bool TCharactorUtils::IsForbiddenBeginOfLine(wchar_t ch)
{
    return (0x3002 == ch       //。
        || 0x003f == ch    //？
        || 0xff0c == ch    //，
        || 0x3001 == ch    //、
        || 0xff01 == ch    //！
        || 0xff1a == ch    //：
        || 0xff1b == ch    //；
        || 0xff09 == ch    //）
        || 0x300b == ch    //》
        || 0x3011 == ch    //】
        || 0x201d == ch    //“
        || 0x2019 == ch    //'
        || 0xff1f == ch    //？
        
        || 0x002e == ch   //.
        || 0x003f == ch   //?
        || 0x0021 == ch   //!
        || 0x002c == ch   //,
        || 0x003a == ch   //:
        || 0x003b == ch   //;
        || 0x002d == ch   //-
        || 0x0029 == ch   //)
        || 0x005d == ch   //]
        || 0x007d == ch   //}
        || 0x0022 == ch   //"
        || 0x0027 == ch   //'
        );
}

bool TCharactorUtils::IsForbiddenEndOfLine(wchar_t ch)
{
    return (0x201c == ch       //°±
        || 0x2018 == ch    //'
        || 0xff08 == ch    //£®
        || 0x300a == ch    //°∂
        || 0x3011 == ch   //°æ
        
        || 0x0028 == ch   //(
        || 0x005b == ch   //[
        || 0x007b == ch   //{
        || 0x0022 == ch   //"
        || 0x0027 == ch   //'
        || 0x0060 == ch   //`
        );
}
