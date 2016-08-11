#pragma once
#ifndef __TXMLReader_H__
#define __TXMLReader_H__

#include <string>
#include <map>

typedef void* XMLItem;

class TXMLReader
{
public:
    TXMLReader();
    ~TXMLReader();

    bool        LoadFromString(const char* str);
    bool        LoadFromFile(const char* file_name);

    bool        IsElementNode(XMLItem item);

    XMLItem     GetParent();
    XMLItem     GetParent(XMLItem item);
    XMLItem     GetChild(XMLItem item);
    XMLItem     GetNext(XMLItem item);
    std::string GetElement(XMLItem item);

    XMLItem     FindItem(const std::string& path);
    XMLItem     FindItem(XMLItem item, const std::string& path);
    
    XMLItem     FindElement(const std::string& elementName);

    std::string GetValue(XMLItem item);
    std::string GetAttrString(XMLItem item, const std::string& key);
    int         GetAttrInteger(XMLItem item, const std::string& key);
    float       GetAttrFloat(XMLItem item, const std::string& key);
    std::map<std::string, std::string> GetAllAttrs(XMLItem item);

private:
    XMLItem     mParent;
};

#endif // __TXMLReader_H__
