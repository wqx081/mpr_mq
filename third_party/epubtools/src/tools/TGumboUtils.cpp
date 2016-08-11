//
//  TGumboUtils.cpp
//  libEpub
//
//  Created by tanwz on 15/12/8.
//
//

#include "TGumboUtils.hpp"


void TGumboUtils::FindNode(GumboNode *parent, GumboTag tag, GumboNode **findedNode)
{
    if (*findedNode && (*findedNode)->v.element.tag == tag) {
        return;
    }
    
    if (parent && parent->type == GUMBO_NODE_ELEMENT) {
        GumboVector children = parent->v.element.children;
        for (size_t i = 0; i < children.length; ++ i) {
            GumboNode *child = (GumboNode *)children.data[i];
            if (child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == tag) {
                *findedNode = child;
                return;
            }
            
            FindNode(child, tag, findedNode);
        }
    }
}

std::map<std::string, std::string> TGumboUtils::GetNodeAttrs(const GumboNode *node)
{
    std::map<std::string, std::string> attrs;
    if (node && node->type == GUMBO_NODE_ELEMENT) {
        for (size_t i = 0; i < node->v.element.attributes.length; ++ i) {
            GumboAttribute *nodeAttr = (GumboAttribute *)node->v.element.attributes.data[i];
            attrs[nodeAttr->name] = nodeAttr->value;
        }
    }
    return attrs;
}

std::string TGumboUtils::GetNodeAttr(const GumboNode *node, const std::string& name)
{
    const std::map<std::string, std::string>& attrs = GetNodeAttrs(node);
    std::map<std::string, std::string>::const_iterator it = attrs.find(name);
    if (it != attrs.end()) {
        return it->second;
    }
    return "";
}

void TGumboUtils::GetImagePathInNode(std::string& path, const GumboNode *node)
{
    GumboNode *gumboNode = (GumboNode *)node;
    
    if (!node || !path.empty()) {
        return;
    }
    
    if (gumboNode->type == GUMBO_NODE_ELEMENT) {
        
        if (gumboNode->v.element.tag == GUMBO_TAG_IMAGE) {
            GumboVector attrs = gumboNode->v.element.attributes;
            for (int i = 0; i < int(attrs.length); ++ i) {
                GumboAttribute *attr = (GumboAttribute *)attrs.data[i];
                if (std::string(attr->name) == "xlink:href" || std::string(attr->name) == "href") {
                    path = attr->value;
                    return;
                }
            }
        } else if(gumboNode->v.element.tag == GUMBO_TAG_IMG) {
            GumboVector attrs = gumboNode->v.element.attributes;
            for (int i = 0; i < int(attrs.length); ++ i) {
                GumboAttribute *attr = (GumboAttribute *)attrs.data[i];
                if (std::string(attr->name) == "src") {
                    path = attr->value;
                    return;
                }
            }
        }
        else {
            for (int i = 0; i < int(gumboNode->v.element.children.length); ++ i) {
                GetImagePathInNode(path, (const GumboNode *)gumboNode->v.element.children.data[i]);
            }
        }
    }
}

void printNode(GumboNode *node, const char *prefix)
{
    if (node->type == GUMBO_NODE_ELEMENT) {
        printf("%stag(%s)\n", prefix, gumbo_normalized_tagname(node->v.element.tag));
    }
    else if (node->type == GUMBO_NODE_TEXT) {
        printf("%stext(%d)\n", prefix, (int)strlen(node->v.text.text));
    }
    else if (node->type == GUMBO_NODE_CDATA) {
        printf("%scdata\n", prefix);
    }
    else if (node->type == GUMBO_NODE_COMMENT) {
        printf("%scomment\n", prefix);
    }
    else if (node->type == GUMBO_NODE_WHITESPACE) {
        printf("%swhitespace\n", prefix);
    }
}

void printAllChildNode(GumboNode *node, const char *prefix)
{
    if (node->type == GUMBO_NODE_ELEMENT) {
        GumboVector children = node->v.element.children;
        for (int i = 0; i <children.length; ++ i) {
            GumboNode *child = (GumboNode *)children.data[i];
            printNode(child, prefix);
            
            char head[128] = {0};
            strcpy(head, "   ");
            printAllChildNode(child, strcat(head, prefix));
        }
    }
}

void TGumboUtils::PrintNode(GumboNode *node)
{
    printAllChildNode(node, "   ");
}

