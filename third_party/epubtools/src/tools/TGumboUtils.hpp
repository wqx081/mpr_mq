//
//  TGumboUtils.hpp
//  libEpub
//
//  Created by tanwz on 15/12/8.
//
//

#ifndef TGumboUtils_hpp
#define TGumboUtils_hpp

#include "gumbo.h"
#include <stdio.h>
#include <string>
#include <map>


class TGumboUtils
{
public:
    static void FindNode(GumboNode *parent, GumboTag tag, GumboNode **findedNode);
    static std::map<std::string, std::string> GetNodeAttrs(const GumboNode *node);
    static std::string GetNodeAttr(const GumboNode *node, const std::string& name);
    static void GetImagePathInNode(std::string& path, const GumboNode *node);
    
    static void PrintNode(GumboNode *node);
};

#endif /* TGumboUtils_hpp */
