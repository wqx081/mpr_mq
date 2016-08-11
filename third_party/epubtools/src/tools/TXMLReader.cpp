#include "TXMLReader.h"
#include "TUtf8Util.h"
#include "mxml.h"


TXMLReader::TXMLReader()
    : mParent(0)
{

}

TXMLReader::~TXMLReader()
{
    if (mParent)
    {
        mxmlRelease((mxml_node_t*)mParent);
        mParent = 0;
    }
}

bool TXMLReader::LoadFromString(const char* str)
{
    if (!str)
    {
        return false;
    }
    if (!mParent)
    {
        mParent = mxmlNewXML("1.0");
    }

    mxmlLoadString((mxml_node_t*)mParent, str, MXML_OPAQUE_CALLBACK);
//    mxmlLoadString((mxml_node_t*)mParent, str, MXML_TEXT_CALLBACK);

    return true;
}

bool TXMLReader::LoadFromFile(const char* file_name)
{
    if (!file_name)
    {
        return false;
    }
    if (!mParent)
    {
        mParent = mxmlNewXML("1.0");
    }

#ifdef _WIN32
    std::wstring wfile = TUtf8Util::UTF8StrToUniStr(file_name);
    FILE* fp = 0;
    _wfopen_s(&fp, wfile.c_str(), L"r");
#else
    FILE* fp = fopen(file_name, "r");
#endif
    if (fp)
    {
        mxmlLoadFile((mxml_node_t*)mParent, fp, MXML_OPAQUE_CALLBACK);
        fclose(fp);
        return true;
    }

    return false;
}

bool TXMLReader::IsElementNode(XMLItem item)
{
    return ((mxml_node_t*)item)->type == MXML_ELEMENT;
}

XMLItem TXMLReader::GetParent()
{
    return mParent;
}

XMLItem TXMLReader::GetParent(XMLItem item)
{
    return ((mxml_node_t*)item)->parent;
}

XMLItem TXMLReader::GetChild(XMLItem item)
{
    return ((mxml_node_t*)item)->child;
}

XMLItem TXMLReader::GetNext(XMLItem item)
{
    return ((mxml_node_t*)item)->next;
}

std::string TXMLReader::GetElement(XMLItem item)
{
    const char *element = mxmlGetElement((mxml_node_t *)item);
    if (element) {
        return element;
    }
    return "";
}

XMLItem TXMLReader::FindItem(const std::string& path)
{
    return FindItem(mParent, path);
}

XMLItem TXMLReader::FindItem(XMLItem item, const std::string& path)
{
    return mxmlFindPath((mxml_node_t*)item, path.c_str());
}

XMLItem TXMLReader::FindElement(const std::string& elementName)
{
    return mxmlFindElement((mxml_node_t*)mParent, (mxml_node_t*)mParent, elementName.c_str(), NULL, NULL, MXML_DESCEND_FIRST);
}

std::string TXMLReader::GetValue(XMLItem item)
{
    const char* ret = mxmlGetOpaque(((mxml_node_t*)item));
    if (ret)
    {
        return ret;
    }

    return "";
}

std::string TXMLReader::GetAttrString(XMLItem item, const std::string& key)
{
    const char* ret = mxmlElementGetAttr(((mxml_node_t*)item), key.c_str());
    if (ret)
    {
        return ret;
    }

    return "";
}

int TXMLReader::GetAttrInteger(XMLItem item, const std::string& key)
{
    std::string str  = GetAttrString(item, key);
    if (!str.empty())
    {
        return atoi(str.c_str());
    }

    return 0;
}

float TXMLReader::GetAttrFloat(XMLItem item, const std::string& key)
{
    std::string str  = GetAttrString(item, key);
    if (!str.empty())
    {
        return (float)atof(str.c_str());
    }

    return 0.0f;
}

std::map<std::string, std::string> TXMLReader::GetAllAttrs(XMLItem item)
{
    mxml_node_t *node = (mxml_node_t *)item;
    std::map<std::string, std::string> attrs;
    if ( node && node->type == MXML_ELEMENT ) {
        for (int i = 0; i < node->value.element.num_attrs; ++ i) {
            mxml_attr_t attr = node->value.element.attrs[i];
            if (attr.name && strlen(attr.name)) {
                attrs[attr.name] = attr.value;
            }
        }
    }
    return attrs;
}
