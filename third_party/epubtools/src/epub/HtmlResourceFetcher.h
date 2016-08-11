//
//  HtmlResourceFetcher.h
//  ePubSplitter
//
//  Created by tanwz on 16/4/20.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#ifndef HtmlResourceFetcher_h
#define HtmlResourceFetcher_h

#include <stdio.h>
#include <vector>
#include <string>


class HtmlResourceFetcher
{
public:
    static std::vector<std::string> FetchExternalRes(const std::string& content);
    
private:
    static void FetchResourceInAllChildNode(std::vector<std::string>& resources, void *parentGumboNode);
    static void FetchResourceInNode(std::vector<std::string>& resources, void *gumboNode);
};

#endif /* HtmlResourceFetcher_h */
