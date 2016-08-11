//
//  EpubPackage.cpp
//  ePubSplitter
//
//  Created by tanwz on 16/4/18.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#include "EpubPackage.h"
#include "TEpubSource.h"
#include "TPathUtil.h"
#include "TStringUtils.h"
#include "HtmlResourceFetcher.h"
#include "EpubPackageWriter.h"


#define META_INF_CONTAINER_PATH  "META-INF/container.xml"

#define PACKAGE_MEDIA_TYPE_NCX   "application/x-dtbncx+xml"
#define PACKAGE_MEDIA_TYPE_XHTML "application/xhtml+xml"

#define PACKAGE_NAV_PROPERTY     "nav"


EpubPackage::EpubPackage()
: mEpubSource(NULL)
{
    
}

EpubPackage::EpubPackage(TEpubSource *source)
: mEpubSource(source)
{
    
}

EpubPackage::~EpubPackage()
{
    if (IsOpened()) {
        Close();
    }
}

bool EpubPackage::Open()
{
    if (!mEpubSource->Open()) {
        return false;
    }
    
    //parse meta info(), and find opf
    if (!ParseMetaInf()) {
        return false;
    }
    
    //open opf
    return ParseOPF();
}

bool EpubPackage::IsOpened() const
{
    return mEpubSource && mEpubSource->IsOpen();
}

void EpubPackage::Close()
{
    for (size_t i = 0; i < mMetaDCMES.size(); ++ i) {
        delete mMetaDCMES.at(i);
    }
    
    for (size_t i = 0; i < mMetaItems.size(); ++ i) {
        delete mMetaItems.at(i);
    }

    for (size_t i = 0; i < mMetaLinks.size(); ++ i) {
        delete mMetaLinks.at(i);
    }
    
    for (size_t i = 0; i < mManifestItems.size(); ++ i) {
        delete mManifestItems.at(i);
    }
    
    for (size_t i = 0; i < mSpineItems.size(); ++ i) {
        delete mSpineItems.at(i);
    }
    
    for (size_t i = 0; i < mGuideItems.size(); ++ i) {
        delete mGuideItems.at(i);
    }
    
    mMetaDCMES.clear();
    mMetaItems.clear();
    mMetaLinks.clear();
    mManifestItems.clear();
    mSpineItems.clear();
    mGuideItems.clear();
    
    mManifestItemIDMap.clear();
}

int EpubPackage::GetSpineCount() const
{
    return (int)mSpineItems.size();
}

EpubPackage * EpubPackage::SubPackage(const std::set<int>& reservedSpines, bool useLiteMode)
{
    EpubPackage *subPackage = NULL;
    if (!reservedSpines.empty()) {
        subPackage = new EpubPackage();
        
        //copy something
        subPackage->mEpubSource = mEpubSource;
        subPackage->mVersion = mVersion;
        subPackage->mUniqueIdentifier = mUniqueIdentifier;
        
        subPackage->mManifestID = mManifestID;
        
        subPackage->mSpineID = mSpineID;
        subPackage->mSpineToc = mSpineToc;
        subPackage->mSpinePageProgressionDirection = mSpinePageProgressionDirection;

        subPackage->mOPFFilePath = mOPFFilePath;
        subPackage->mOPFParentPath = mOPFParentPath;
        
        
        //1, spineItem
        std::set<std::string> allResource;
        
        for (std::set<int>::iterator it = reservedSpines.begin(); it != reservedSpines.end(); ++ it) {
            if (*it >= 0 && *it < mSpineItems.size()) {
                subPackage->mSpineItems.push_back(new EPackageSpineItem(*mSpineItems.at(*it)));
                
                
                //1, 解析spine item, css 和image都要记录下来，并在manifest中记录
                std::vector<std::string> resources = FetchExternalResourceInSpine(*it);
                allResource.insert(resources.begin(), resources.end());
                
                //2，spine本身加入manifest
                if (mManifestItemIDMap.find(mSpineItems.at(*it)->mIDRef) != mManifestItemIDMap.end()) {
                    allResource.insert(mManifestItemIDMap[mSpineItems.at(*it)->mIDRef]->mHref);
                }
            }
        }
        
        //2, rebuild manifest
        if (!useLiteMode) {
            //封面图片要放到manifest中，并写到meta的cover中
            std::string coverResource = GetCoverImageHref();
            if (!coverResource.empty()) {
                allResource.insert(coverResource);
            }
            
            //toc, ncx都包含进去
            subPackage->mNCXPath = mNCXPath;
            subPackage->mNavTocPath = mNavTocPath;
            
            allResource.insert(mNCXPath);
            allResource.insert(mNavTocPath);
        }
        
        for (std::set<std::string>::iterator it = allResource.begin(); it != allResource.end(); ++ it) {
            subPackage->mManifestItems.push_back(CreateManifestItemByHref(*it));
        }
        
        for (size_t i = 0; i < subPackage->mManifestItems.size(); ++ i) {
            const std::string& id = subPackage->mManifestItems.at(i)->mID;
            const std::string& href = subPackage->mManifestItems.at(i)->mHref;
            subPackage->mManifestItemIDMap[id] = subPackage->mManifestItems.at(i);
            subPackage->mManifestItemHrefMap[href] = subPackage->mManifestItems.at(i);
        }
        
        subPackage->BuildSpineIndexMap();
        
        //3, guide
        if (!useLiteMode) {
            for (size_t i = 0; i < mGuideItems.size(); ++ i) {
                subPackage->mGuideItems.push_back(new EPackageGuideItem(*mGuideItems.at(i)));
            }
        }
    }
    
    return subPackage;
}

bool EpubPackage::Save(const std::string& path)
{
    if (mSpineItems.empty()) {
        return false;
    }
    
    EpubPackageWriter writer(this);
    return writer.WriteToFile(path.c_str());
}

int EpubPackage::GetSectionIndex(const std::string& href)
{
    if (mSpineIndexMap.find(href) != mSpineIndexMap.end()) {
        return mSpineIndexMap[href];
    }
    return -1;
}

bool EpubPackage::ParseMetaInf()
{
    if (mEpubSource)
    {
        int size = mEpubSource->ItemSize(META_INF_CONTAINER_PATH);
        if (size <= 0)
        {
            return false;
        }
        std::string container;
        container.resize((int)size + 1);
        mEpubSource->ReadAllForItem(META_INF_CONTAINER_PATH, &container[0], (int)container.size());
        container[(int)size] = 0;
        
        TXMLReader xmlReader;
        xmlReader.LoadFromString(container.c_str());
        XMLItem item = xmlReader.FindItem("*/rootfiles/rootfile");
        if (item)
        {
            mOPFFilePath = xmlReader.GetAttrString(item, "full-path");
            mOPFParentPath = TPathUtil::GetParentPath(mOPFFilePath);
            return true;
        }
    }
    
    return false;
}

bool EpubPackage::ParseOPF()
{
    if (!mOPFFilePath.empty())
    {
        int size = mEpubSource->ItemSize(mOPFFilePath);
        if (size <= 0)
        {
            return false;
        }
        std::string opf;
        opf.resize((int)size + 1);
        mEpubSource->ReadAllForItem(mOPFFilePath, &opf[0], (int)opf.size());
        opf[size] = 0;
        
        TXMLReader xmlReader;
        xmlReader.LoadFromString(opf.c_str());
        
        XMLItem item = xmlReader.FindElement("package");
        if (item)
        {
            mUniqueIdentifier = xmlReader.GetAttrString(item, "unique-identifier");
            mVersion = xmlReader.GetAttrString(item, "version");
            
            item = xmlReader.GetChild(item);
            while (item) {
                std::string elementName = xmlReader.GetElement(item);
                if (elementName == "metadata") {
                    ReadMetadata(&xmlReader, item);
                }
                else if (elementName == "manifest") {
                    ReadManifest(&xmlReader, item);
                }
                else if (elementName == "spine") {
                    ReadSpine(&xmlReader, item);
                }
                else if (elementName == "guide") {
                    ReadGuide(&xmlReader, item);
                }
                
                item = xmlReader.GetNext(item);
            }
            
            BuildSpineIndexMap();
        
            //ncx
            if (mManifestItemIDMap.find(mSpineToc) != mManifestItemIDMap.end()) {
                EPackageManifestItem *item = mManifestItemIDMap[mSpineToc];
                if (item->mMediaType == PACKAGE_MEDIA_TYPE_NCX) {
                    mNCXPath = item->mHref;
                }
            }
            
            return true;
        }
    }
    
    return false;
}

bool EpubPackage::ReadMetadata(TXMLReader *reader, XMLItem item)
{
    item = reader->GetChild(item);
    if (!item)
    {
        return false;
    }
    
    do
    {
        std::string element_name = reader->GetElement(item);
        
        if (element_name.length() > 3 && element_name.substr(0, 3) == "dc:") {
            EPackageDCMEItem *dcmes = new EPackageDCMEItem;
            dcmes->mName = element_name;
            dcmes->mText = reader->GetValue(item);
            dcmes->mID   = reader->GetAttrString(item, "id");
            dcmes->mLang = reader->GetAttrString(item, "xml:lang");
            dcmes->mDirection = reader->GetAttrString(item, "dir");
            
            mMetaDCMES.push_back(dcmes);
        }
        else if (element_name == "meta"){
            EPackageMetaItem *meta = new EPackageMetaItem;
            meta->mText     = reader->GetValue(item);
            meta->mProperty = reader->GetAttrString(item, "property");
            meta->mRefines  = reader->GetAttrString(item, "refines");
            meta->mID       = reader->GetAttrString(item, "id");
            meta->mScheme   = reader->GetAttrString(item, "scheme");
            meta->mLang     = reader->GetAttrString(item, "xml:lang");
            meta->mDirection= reader->GetAttrString(item, "dir");
            meta->mName     = reader->GetAttrString(item, "name");
            meta->mContent  = reader->GetAttrString(item, "content");
            
            mMetaItems.push_back(meta);
        }
        else if (element_name == "link"){
            EPackageLinkItem *link = new EPackageLinkItem;
            link->mHref      = reader->GetAttrString(item, "href");
            link->mRel       = reader->GetAttrString(item, "rel");
            link->mID        = reader->GetAttrString(item, "id");
            link->mRefines   = reader->GetAttrString(item, "refines");
            link->mMediaType = reader->GetAttrString(item, "media-type");
            
            mMetaLinks.push_back(link);
        }
    } while ((item = reader->GetNext(item)), (0 != item));
    
    return true;
}

bool EpubPackage::ReadManifest(TXMLReader *reader, XMLItem item)
{
    mManifestID = reader->GetAttrString(item, "id");
    
    item = reader->GetChild(item);
    if (!item) {
        return false;
    }
    
    do
    {
        std::string element_name = reader->GetElement(item);
        if (element_name != "item") {
            continue;
        }
        
        std::string id = reader->GetAttrString(item, "id");
        std::string href = reader->GetAttrString(item, "href");
        std::string media_type = reader->GetAttrString(item, "media-type");
        std::string properties = reader->GetAttrString(item, "properties");
        
        if (id.empty() || href.empty()) {
            continue;
        }
        std::string absoluteHref = TPathUtil::GetAbsolutePath(href, mOPFParentPath);
        
        EPackageManifestItem *manifestItem = new EPackageManifestItem;
        manifestItem->mID           = id;
        manifestItem->mHref         = absoluteHref;
        manifestItem->mMediaType    = media_type;
        manifestItem->mFallback     = reader->GetAttrString(item, "fallback");
        manifestItem->mProperties   = properties;
        manifestItem->mMediaOverlay = reader->GetAttrString(item, "media-overlay");
        
        mManifestItems.push_back(manifestItem);
        
        if (mManifestItemIDMap.find(id) == mManifestItemIDMap.end()) {
            mManifestItemIDMap[id] = manifestItem;
        }
        if (mManifestItemHrefMap.find(absoluteHref) == mManifestItemHrefMap.end()) {
            mManifestItemHrefMap[absoluteHref] = manifestItem;
        }
        
        if (media_type == PACKAGE_MEDIA_TYPE_XHTML && properties == PACKAGE_NAV_PROPERTY) {
            mNavTocPath = absoluteHref;
        }
        
    } while ((item = reader->GetNext(item)), (0 != item));
    
    return true;
}

bool EpubPackage::ReadSpine(TXMLReader *reader, XMLItem item)
{
    mSpineID  = reader->GetAttrString(item, "id");
    mSpineToc = reader->GetAttrString(item, "toc");
    mSpinePageProgressionDirection = reader->GetAttrString(item, "page-progression-direction");
    
    item = reader->GetChild(item);
    if (!item) {
        return false;
    }
    
    do
    {
        if (!reader->IsElementNode(item)) {
            continue;
        }
        
        std::string element_name =reader->GetElement(item);
        if (element_name != "itemref") {
            continue;
        }
        
        EPackageSpineItem *spineItem = new EPackageSpineItem;
        spineItem->mIDRef         = reader->GetAttrString(item, "idref");
        spineItem->mLinear        = reader->GetAttrString(item, "linear");
        spineItem->mID            = reader->GetAttrString(item, "id");
        spineItem->mProperties    = reader->GetAttrString(item, "properties");
        mSpineItems.push_back(spineItem);
        
    } while ((item = reader->GetNext(item)), (0 != item));
    
    return true;
}

bool EpubPackage::ReadGuide(TXMLReader *reader, XMLItem item)
{
    item = reader->GetChild(item);
    if (!item) {
        return false;
    }
    
    do {
        std::string element = reader->GetElement(item);
        if (element != "reference") {
            continue;
        }
        
        EPackageGuideItem *guideItem = new EPackageGuideItem;
        
        guideItem->mType = EPackageGuideItem::typeFromString(reader->GetAttrString(item, "type"));
        guideItem->mTitle = reader->GetAttrString(item, "title");
        guideItem->mHref = TPathUtil::GetAbsolutePath(reader->GetAttrString(item, "href"), mOPFParentPath);
        mGuideItems.push_back(guideItem);
        
    } while ((item = reader->GetNext(item)), (0 != item));
    return true;
}

void EpubPackage::BuildSpineIndexMap()
{
    std::string href;
    for (size_t i = 0; i < mSpineItems.size(); ++ i) {
        if (mManifestItemIDMap.find(mSpineItems.at(i)->mIDRef) == mManifestItemIDMap.end()) {
            continue;
        }
        
        href = mManifestItemIDMap[mSpineItems.at(i)->mIDRef]->mHref;
        mSpineIndexMap[href] = (int)i;
    }
}

EPackageManifestItem *EpubPackage::CreateManifestItemByHref(const std::string& href)
{
    if (mManifestItemHrefMap.find(href) != mManifestItemHrefMap.end()) {
        return new EPackageManifestItem(*mManifestItemHrefMap[href]);
    }
    
    EPackageManifestItem *newItem = new EPackageManifestItem;
    newItem->mID = TPathUtil::GetLastComponent(href);
    newItem->mHref = href;
    newItem->mMediaType = ManifestMediaTypeByFileExtension(TPathUtil::GetExtension(href));
    
    return newItem;
}

std::vector<std::string> EpubPackage::FetchExternalResourceInSpine(int spine)
{
    do {
        if (spine < 0 || spine >= mSpineItems.size()) {
            break;
        }
        
        if (mManifestItemIDMap.find(mSpineItems.at(spine)->mIDRef) == mManifestItemIDMap.end()) {
            break;
        }
        
        const std::string& spineHref = mManifestItemIDMap[mSpineItems.at(spine)->mIDRef]->mHref;
        if (!mEpubSource->ExistItem(spineHref)) {
            break;
        }
        
        int size = mEpubSource->ItemSize(spineHref);
        if (size <= 0) {
            break;
        }
        std::string spineContent(size + 1, '\0');
        mEpubSource->ReadAllForItem(spineHref, &spineContent[0], size);
       
        std::vector<std::string> resources = HtmlResourceFetcher::FetchExternalRes(spineContent);
        for (size_t i = 0; i < resources.size(); ++ i) {
            std::string spineParentPath = TPathUtil::GetParentPath(spineHref);
            resources[i] = TPathUtil::GetAbsolutePath(resources.at(i), spineParentPath);
        }
        return resources;
        
    } while (false);
    
    return std::vector<std::string>();
}

std::string EpubPackage::GetCoverImageHref()
{
    //1，在meta中找
    std::string href;
    for (size_t i = 0; i < mMetaItems.size(); ++ i) {
        if (mMetaItems.at(i)->mName == "cover") {
            const std::string& content = mMetaItems.at(i)->mContent;
            if (mManifestItemIDMap.find(content) != mManifestItemIDMap.end()) {
                href = mManifestItemIDMap[content]->mHref;
                break;
            }
        }
    }
    
    //2，在manifest中找
    if (href.empty()) {
        for (size_t i = 0; i < mManifestItems.size(); ++ i) {
            if (TStringUtils::StartWith(TStringUtils::ToLower(mManifestItems.at(i)->mID), "cover")   ) {
                href = mManifestItems.at(i)->mHref;
                break;
            }
        }
    }
    return href;
}

std::string EpubPackage::ManifestMediaTypeByFileExtension(const std::string& extension)
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
    else if (lowerExtension == "xhtml") {
        return PACKAGE_MEDIA_TYPE_XHTML;
    }
    else if (lowerExtension == "toc") {
        return PACKAGE_MEDIA_TYPE_NCX;
    }
    return "";
}

