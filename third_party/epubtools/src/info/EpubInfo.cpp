//
//  EpubInfo.cpp
//  test
//
//  Created by tanwz on 16/8/3.
//  Copyright © 2016年 mpreader. All rights reserved.
//

#include "EpubInfo.h"
#include "EpubPackage.h"
#include "TEpubArchiveSource.h"
#include "TXMLReader.h"
#include "TPathUtil.h"
#include "CatalogItem.h"
#include "ISLIMark.h"


static const char* MRA_FILE_PATH            = "META-INF/AuthorizationFile";
static const char* ISLI_CODES_FILE_PATH     = "META-INF/ISLI_MPRCodes.xml";

EpubInfo::EpubInfo(const std::string& filePath)
{
    mEpubSource = new TEpubArchiveSource(filePath);
    mPackage = new EpubPackage(mEpubSource);
    mOpenFlag = mPackage->Open();
    
    //init catalog
    ParseNav();
    if (mCatalogs.empty()) {
        ParseNCX();
    }
    
    //init isli marks
    LoadISLIMarks();
}

EpubInfo::~EpubInfo()
{
    delete mPackage;
    delete mEpubSource;
}

bool EpubInfo::IsValid() const
{
    return mOpenFlag;
}

int EpubInfo::GetSectionCount() const
{
    return mPackage->GetSpineCount();
}

const std::vector<CatalogItem>& EpubInfo::GetCatalogs() const
{
    return mCatalogs;
}

const std::vector<ISLIMark>& EpubInfo::GetISLIMarks() const
{
    return mISLIMarks;
}

int EpubInfo::GetMRAData(unsigned char *buffer)
{
    int size = mEpubSource->ItemSize(MRA_FILE_PATH);
    if (size > 0 && buffer) {
        size = mEpubSource->ReadAllForItem(MRA_FILE_PATH, buffer, size);
    }
    
    return size;
}

void EpubInfo::ParseNav()
{
    if (!mPackage->GetNavPath().empty())
    {
        int size = mEpubSource->ItemSize(mPackage->GetNavPath());
        if (size > 0)
        {
            std::string nav;
            nav.resize((int)size + 1);
            mEpubSource->ReadAllForItem(mPackage->GetNavPath(), &nav[0], (int)nav.size());
            nav[(int)size] = 0;
            
            TXMLReader xmlReader;
            xmlReader.LoadFromString(nav.c_str());
            XMLItem xml_item = xmlReader.FindItem("html/body");
            
            //find nav
            std::vector<XMLItem> nodeStack;
            bool hasFind = false;
            while (xml_item || !nodeStack.empty()) {
                while (xml_item) {
                    nodeStack.push_back(xml_item);
                    xml_item = xmlReader.GetChild(xml_item);
                }
                
                if (!nodeStack.empty()) {
                    xml_item = nodeStack.back();
                    nodeStack.pop_back();
                    
                    if (xmlReader.GetElement(xml_item) == "nav" && xmlReader.GetAttrString(xml_item, "epub:type") == "toc") {
                        hasFind = true;
                        break;
                    }
                    xml_item = xmlReader.GetNext(xml_item);
                }
            }
            
            if (hasFind) {
                XMLItem xml_child = xmlReader.GetChild(xml_item);
                
                while (xml_child != NULL)//遍历 nav 所有子节点
                {
                    if (xmlReader.GetElement(xml_child) == "ol") {
                        LoadNavGroup(xmlReader, xml_child, 0);
                    }
                    
                    xml_child = xmlReader.GetNext(xml_child);
                }
            }
        }
    }
}

bool EpubInfo::LoadNavGroup(TXMLReader &xmlReader, XMLItem item, int level)
{
    item = xmlReader.GetChild(item);
    
    while (item) {
        if (xmlReader.GetElement(item) == "li") {
            CreateNavNode(xmlReader, item, level);
        }
        item = xmlReader.GetNext(item);
    }
    return true;
}

bool EpubInfo::CreateNavNode(TXMLReader &xmlReader, XMLItem item, int level)
{
    item = xmlReader.GetChild(item);
    std::string eleName;
    
    mCatalogs.push_back(CatalogItem());
    size_t nodeIndex = mCatalogs.size() - 1;
    mCatalogs.back().level = level;
    
    while (item) {
        eleName = xmlReader.GetElement(item);
        
        if (eleName == "span") {
            mCatalogs.at(nodeIndex).title = xmlReader.GetValue(item);
        }
        else if (eleName == "a") {
            mCatalogs.at(nodeIndex).title = xmlReader.GetValue(item);
            
            std::string href = xmlReader.GetAttrString(item, "href");
            href = TPathUtil::GetAbsolutePath(href, TPathUtil::GetParentPath(mPackage->GetNavPath()));
            mCatalogs.at(nodeIndex).href = href;
            
            size_t anchorIndex = href.find('#');
            if (anchorIndex != std::string::npos) {
                href = href.substr(0, anchorIndex);
            }

            int orderIndex = mPackage->GetSectionIndex(href);
            if (orderIndex < 0) {
                orderIndex = 0;
            }
            mCatalogs.at(nodeIndex).sectionIndex = orderIndex;
        }
        else if (eleName == "ol") {
            LoadNavGroup(xmlReader, item, level + 1);
        }
        
        item = xmlReader.GetNext(item);
    }
    
    //当src为空时，取它第一个子节点作resource与src
    CatalogItem& navItem = mCatalogs.at(nodeIndex);
    if (navItem.href.empty()) {
        for (size_t i = nodeIndex + 1; i < mCatalogs.size(); ++ i) {
            const CatalogItem& curItem = mCatalogs.at(i);
            if (!curItem.href.empty() && curItem.level > navItem.level) {
                navItem.href         = curItem.href;
                navItem.sectionIndex = curItem.sectionIndex;
                break;
            }
        }
    }
    
    return true;
}

void EpubInfo::ParseNCX()
{
    std::string ncxPath = mPackage->GetNCXPath();
    int size = mEpubSource->ItemSize(ncxPath);
    if (!ncxPath.empty() && size > 0)
    {
        std::string ncxContent(size + 1, '\0');
        mEpubSource->ReadAllForItem(ncxPath, &ncxContent[0], size);
        
        TXMLReader xmlReader;
        xmlReader.LoadFromString(ncxContent.c_str());
        
        XMLItem item = xmlReader.FindItem("ncx/navMap");
        if (xmlReader.GetElement(item) == "navMap") {
            item = xmlReader.GetChild(item);
        }
        
        while (item) {
            LoadNavPoint(xmlReader, item, 0);
            
            item = xmlReader.GetNext(item);
        }
    }
}

void EpubInfo::LoadNavPoint(TXMLReader &xmlReader, XMLItem item, int level)
{
    if (item && xmlReader.GetElement(item) == "navPoint") {
        item = xmlReader.GetChild(item);
        
        mCatalogs.push_back(CatalogItem());
        CatalogItem &catalog = mCatalogs.back();
        
        while (item) {
            if (xmlReader.GetElement(item) == "navLabel") {
                XMLItem textItem = xmlReader.FindItem(item, "text");
                if (textItem) {
                    catalog.title = xmlReader.GetValue(textItem);
                }
            } else if(xmlReader.GetElement(item) == "content") {
                std::string src = xmlReader.GetAttrString(item, "src");
                src = TPathUtil::GetAbsolutePath(src, TPathUtil::GetParentPath(mPackage->GetNavPath()));
                catalog.href = src;
                src = src.substr(0, src.find_first_of('#'));
                
                int orderIndex = mPackage->GetSectionIndex(src);
                if (orderIndex < 0) {
                    orderIndex = 0;
                }
                catalog.sectionIndex= orderIndex;
                catalog.level = level;
            } else if(xmlReader.GetElement(item) == "navPoint") {
                LoadNavPoint(xmlReader, item, level + 1);
            }
            
            item = xmlReader.GetNext(item);
        }
    }
}

void EpubInfo::LoadISLIMarks()
{
    if (mEpubSource->ExistItem(ISLI_CODES_FILE_PATH)) {
        int size = mEpubSource->ItemSize(ISLI_CODES_FILE_PATH);
        std::string isli_code_data(size + 1, 0);
        
        mEpubSource->ReadAllForItem(ISLI_CODES_FILE_PATH, &isli_code_data[0], size);
        TXMLReader xmlReader;
        xmlReader.LoadFromString(isli_code_data.c_str());
        
        XMLItem item = xmlReader.FindElement("ISLI-MPRCodes");
        if (item)
        {
            item = xmlReader.GetChild(item);
            while (item) {
                std::string elementName = xmlReader.GetElement(item);
                if (elementName == "ISLI-MPRCode") {
                    mISLIMarks.push_back(ISLIMark());
                    ISLIMark& mark = mISLIMarks.back();
                    
                    mark.sourceID = xmlReader.GetAttrString(item, "code");
                    mark.startCFI = xmlReader.GetAttrString(item, "scfi_begin");
                    mark.endCFI   = xmlReader.GetAttrString(item, "scfi_end");
                }
                
                item = xmlReader.GetNext(item);
            }
        }
    }
}
