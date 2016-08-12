//
//  HtmlResourceFetcher.cpp
//  ePubSplitter
//
//  Created by tanwz on 16/4/20.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#include "HtmlResourceFetcher.h"
#include "gumbo.h"
#include "TStringUtils.h"
#include "TPathUtil.h"
#include <string.h>
#include <stdlib.h>


static inline std::string htmlResourceTypeWithFileExtension(const std::string& extension)
{
    std::string lowerExtension = TStringUtils::ToLower(extension);
    if (lowerExtension == "gif") {
        return "image/gif";
    }
    else if (lowerExtension == "jpe" ||
             lowerExtension == "jpeg" ||
             lowerExtension == "jpg") {
        return "image/jpeg";
    }
    else if (lowerExtension == "tif" ||
             lowerExtension == "tif") {
        return "image/tiff";
    }
    else if (lowerExtension == "png") {
        return "image/png";
    }
    else if (lowerExtension == "css") {
        return "text/css";
    }
    return "";
}


std::vector<std::string> HtmlResourceFetcher::FetchExternalRes(const std::string& content)
{
    std::vector<std::string> resources;
    
    GumboOutput *output = gumbo_parse(content.c_str());
    FetchResourceInAllChildNode(resources, output->root);
    
    return resources;
}

void HtmlResourceFetcher::FetchResourceInAllChildNode(std::vector<std::string>& resources, void *parentGumboNode)
{
    GumboNode *parent = (GumboNode *)parentGumboNode;
    if (parent && parent->type == GUMBO_NODE_ELEMENT) {
        
        GumboVector children = parent->v.element.children;
        for (size_t i = 0; i < children.length; ++ i) {
            GumboNode *child = (GumboNode *)children.data[i];
            FetchResourceInNode(resources, child);
            
            FetchResourceInAllChildNode(resources, child);
        }
    }
}

void HtmlResourceFetcher::FetchResourceInNode(std::vector<std::string>& resources, void *gumboNode)
{
    // link rel="stylesheet"
    // img src
    // svg image xlink:href
    
    GumboNode *node = (GumboNode *)gumboNode;
    if (node->type == GUMBO_NODE_ELEMENT) {
        std::string href;
        if (node->v.element.tag == GUMBO_TAG_LINK) {
            const char *attrRel = gumbo_get_attribute(&node->v.element.attributes, "rel")->value;
            const char *attrHref = gumbo_get_attribute(&node->v.element.attributes, "href")->value;
            if (!strcmp(attrRel, "stylesheet") && strlen(attrHref) > 0) {
                href = attrHref;
            }
        }
        else if (node->v.element.tag == GUMBO_TAG_IMG) {
            href = gumbo_get_attribute(&node->v.element.attributes, "src")->value;
        }
        else if (node->v.element.tag == GUMBO_TAG_IMAGE) {
            href = gumbo_get_attribute(&node->v.element.attributes, "href")->value;
        }
        
        if (!href.empty()) {
            resources.push_back(href);
        }
    }
}
