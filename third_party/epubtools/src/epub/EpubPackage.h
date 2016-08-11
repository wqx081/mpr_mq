//
//  EpubPackage.h
//  ePubSplitter
//
//  Created by tanwz on 16/4/18.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#ifndef EpubPackage_h
#define EpubPackage_h

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "TXMLReader.h"


struct EPackageMetaItem
{
    std::string mProperty;
    std::string mRefines;
    std::string mID;
    std::string mScheme;
    std::string mLang;
    std::string mDirection;
    std::string mText;
    std::string mName;
    std::string mContent;
};

struct EPackageDCMEItem
{
    std::string mName;
    std::string mText;
    
    std::string mID;
    std::string mLang;
    std::string mDirection;
};

struct EPackageLinkItem
{
    std::string mHref;
    std::string mRel;
    std::string mID;
    std::string mRefines;
    std::string mMediaType;
};

struct EPackageManifestItem
{
    std::string mID;
    std::string mHref;
    std::string mMediaType;
    std::string mFallback;
    std::string mProperties;
    std::string mMediaOverlay;
};

struct EPackageSpineItem
{
    std::string mIDRef;
    std::string mLinear;
    std::string mID;
    std::string mProperties;
};

struct EPackageGuideItem
{
    enum Type
    {
        Unknow = -1,
        Cover,
        TitlePage,
        Toc,
        CopyRightPage,
    };
    
    static inline Type typeFromString(const std::string& str) {
        if (str == "cover")         return Cover;
        if (str == "title-page")    return TitlePage;
        if (str == "toc")           return Toc;
        if (str == "copyright-page")return CopyRightPage;
        
        return Unknow;
    }
    
    static inline std::string StringFromType(Type type) {
        switch (type) {
            case Cover:         return "cover";
            case TitlePage:     return "title-page";
            case Toc:           return "toc";
            case CopyRightPage: return "copyright-page";
            
            default:
                break;
        }
        
        return "";
    }
    
    std::string GetTypeName() const {
        return StringFromType(mType);
    }
    
    Type mType;
    std::string mTitle;
    std::string mHref;
};

class EpubPackageWriter;
class TEpubSource;
class EpubPackage
{
public:
    EpubPackage();
    EpubPackage(TEpubSource *source);
    ~EpubPackage();
    
    bool Open();
    bool IsOpened() const;
    void Close();
    
    int  GetSpineCount() const;
    
    /** useLiteMode: 
     * true  默认，只保留spine及其引用的资源
     * false 保留封面图片及toc及guide
     */
    EpubPackage * SubPackage(const std::set<int>& reservedSpines, bool useLiteMode = true);
    
    bool Save(const std::string& path);
    
    const std::string& GetNCXPath() const { return mNCXPath; }
    const std::string& GetNavPath() const { return mNavTocPath; }
    int   GetSectionIndex(const std::string& href);
    
private:
    bool ParseMetaInf();
    bool ParseOPF();
    
    bool ReadMetadata(TXMLReader *reader, XMLItem item);
    bool ReadManifest(TXMLReader *reader, XMLItem item);
    bool ReadSpine(TXMLReader *reader, XMLItem item);
    bool ReadGuide(TXMLReader *reader, XMLItem item);
    
    void BuildSpineIndexMap();
    
    EPackageManifestItem *CreateManifestItemByHref(const std::string& href);
    std::vector<std::string> FetchExternalResourceInSpine(int spine);
    std::string GetCoverImageHref();
    std::string ManifestMediaTypeByFileExtension(const std::string& extension);
    
private:
    std::string mVersion;
    std::string mUniqueIdentifier;
    
    //metadata
    std::vector<EPackageDCMEItem *> mMetaDCMES;
    std::vector<EPackageMetaItem *> mMetaItems;
    std::vector<EPackageLinkItem *> mMetaLinks;
    
    //manifest
    std::string mManifestID;
    std::map<std::string, EPackageManifestItem *> mManifestItemIDMap;
    std::map<std::string, EPackageManifestItem *> mManifestItemHrefMap;
    std::vector<EPackageManifestItem *> mManifestItems;
    
    //spine
    std::string mSpineID;
    std::string mSpineToc;
    std::string mSpinePageProgressionDirection;
    std::vector<EPackageSpineItem *> mSpineItems;
    
    std::map<std::string, int> mSpineIndexMap;
    
    //guide
    std::vector<EPackageGuideItem *> mGuideItems;
    
    //ncx, toc
    std::string mNCXPath;
    std::string mNavTocPath;
    
    TEpubSource *mEpubSource;
    std::string mOPFFilePath;
    std::string mOPFParentPath;
    
    friend class EpubPackageWriter;
};

#endif /* EpubPackage_h */
