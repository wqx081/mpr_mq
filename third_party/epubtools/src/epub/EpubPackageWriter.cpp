//
//  EpubPackageWriter.cpp
//  ePubSplitter
//
//  Created by tanwz on 16/4/25.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#include "EpubPackageWriter.h"
#include "EpubPackage.h"
#include "TEpubSource.h"
#include "ZipArchiveWriter.h"
#include "TPathUtil.h"
#include "TXMLWriter.h"
#include <stdlib.h>
#include <string.h>


EpubPackageWriter::EpubPackageWriter(EpubPackage *package)
: mPackage(package)
{
    
}

bool EpubPackageWriter::WriteToFile(const char *path)
{
    bool ret = false;
    ZipArchiveWriter *zipWriter = new ZipArchiveWriter();
    do {
        if (!zipWriter->Open(path, false)) {
            break;
        }
        
        if (!WriteMimetype(zipWriter)) {
            break;
        }
        
        if (!WriteContainer(zipWriter)) {
            break;
        }
        
        if (!WriteOPF(zipWriter)) {
            break;
        }
        
        if (!WriteResources(zipWriter)) {
            break;
        }
        ret = true;
    } while (false);
    
    zipWriter->Close();
    delete  zipWriter;
    
    return ret;
}

bool EpubPackageWriter::WriteMimetype(ZipArchiveWriter *zipWriter)
{
    const char *mime = "application/epub+zip";
    return (zipWriter->AddFileItem((void *)mime, strlen(mime), "mimetype") >= 0);
}

bool EpubPackageWriter::WriteContainer(ZipArchiveWriter *zipWriter)
{
    return WriteItem(zipWriter, "META-INF/container.xml");
}

bool EpubPackageWriter::WriteOPF(ZipArchiveWriter *zipWriter)
{
    std::string opfContent;
    if (!CreateOPFXML(opfContent)) {
        return false;
    }
    return (zipWriter->AddFileItem(&opfContent[0], opfContent.length(), mPackage->mOPFFilePath) >= 0);
}

bool EpubPackageWriter::WriteResources(ZipArchiveWriter *zipWriter)
{
    for (size_t i = 0; i < mPackage->mManifestItems.size(); ++ i) {
        if (!WriteItem(zipWriter, mPackage->mManifestItems.at(i)->mHref.c_str())) {
            return false;
        }
    }
    return true;
}

bool EpubPackageWriter::WriteItem(ZipArchiveWriter *zipWriter,const char *itemPath)
{
    if (!mPackage->mEpubSource->ExistItem(itemPath) || mPackage->mEpubSource->ItemSize(itemPath) <= 0) {
        //TODO:做文件大小限制
        return false;
    }
    
    int  bufferSize = mPackage->mEpubSource->ItemSize(itemPath);
    char *buffer    = (char *)malloc(bufferSize);
    mPackage->mEpubSource->ReadAllForItem(itemPath, buffer, bufferSize);
    return (zipWriter->AddFileItem(buffer, bufferSize, itemPath) >= 0);
}

bool EpubPackageWriter::CreateOPFXML(std::string& content)
{
    TXMLWriter writer;
    XMLItem root = writer.GetRoot();
    XMLItem packageItem = writer.CreateElement(root, "package");
    writer.SetAttr(packageItem, "xmlns", "http://www.idpf.org/2007/opf");
    writer.SetAttr(packageItem, "unique-identifier", mPackage->mUniqueIdentifier.c_str());
    writer.SetAttr(packageItem, "version", mPackage->mVersion.c_str());
    
    //metadata
    XMLItem metadataItem = writer.CreateElement(packageItem, "metadata");
    writer.SetAttr(metadataItem, "xmlns:dc", "http://purl.org/dc/elements/1.1/");
    writer.SetAttr(metadataItem, "xmlns:opf", "http://www.idpf.org/2007/opf");
    
    //metadata/dcme
    for (size_t i = 0; i < mPackage->mMetaDCMES.size(); ++ i) {
        EPackageDCMEItem *item = mPackage->mMetaDCMES.at(i);
        
        XMLItem dcmeItem = writer.CreateElement(metadataItem, item->mName.c_str());
        writer.SetAttr(dcmeItem, "id", item->mID.c_str());
        if (!item->mText.empty()) {
            writer.CreateText(dcmeItem, item->mText.c_str());
        }
    }
    //metadata/meta
    for (size_t i = 0; i < mPackage->mMetaItems.size(); ++ i) {
        EPackageMetaItem *item = mPackage->mMetaItems.at(i);
        
        XMLItem metaItem = writer.CreateElement(metadataItem, "meta");
        writer.SetAttr(metaItem, "refines", item->mRefines.c_str());
        writer.SetAttr(metaItem, "property", item->mProperty.c_str());
        
        if (!item->mID.empty()) {
            writer.SetAttr(metaItem, "id", item->mID.c_str());
        }
        if (!item->mScheme.empty()) {
            writer.SetAttr(metaItem, "scheme", item->mScheme.c_str());
        }
        if (!item->mContent.empty()) {
            writer.SetAttr(metaItem, "content", item->mContent.c_str());
        }
        if (!item->mName.empty()) {
            writer.SetAttr(metaItem, "name", item->mName.c_str());
        }
        
        if (!item->mText.empty()) {
            writer.CreateText(metaItem, item->mText.c_str());
        }
    }
    //metadata/link
    for (size_t i = 0; i < mPackage->mMetaLinks.size(); ++ i) {
        EPackageLinkItem *item = mPackage->mMetaLinks.at(i);
        
        XMLItem linkItem = writer.CreateElement(metadataItem, "link");
        writer.SetAttr(linkItem, "href", item->mHref.c_str());
        writer.SetAttr(linkItem, "rel",  item->mRel.c_str());
        writer.SetAttr(linkItem, "type", item->mMediaType.c_str());
        if (!item->mID.empty()) {
            writer.SetAttr(linkItem, "id", item->mID.c_str());
        }
        if (!item->mRefines.empty()) {
            writer.SetAttr(linkItem, "refines", item->mRefines.c_str());
        }
    }
    
    //manifest
    XMLItem manifestItem = writer.CreateElement(packageItem, "manifest");
    for (size_t i = 0; i < mPackage->mManifestItems.size(); ++ i) {
        EPackageManifestItem *item = mPackage->mManifestItems.at(i);
        
        XMLItem maniItem = writer.CreateElement(manifestItem, "item");
        writer.SetAttr(maniItem, "href", TPathUtil::GetReletivePath(item->mHref, mPackage->mOPFParentPath).c_str());
        writer.SetAttr(maniItem, "id", item->mID.c_str());
        writer.SetAttr(maniItem, "media-type", item->mMediaType.c_str());
        if (!item->mProperties.empty()) {
            writer.SetAttr(maniItem, "properties", item->mProperties.c_str());
        }
    }
    
    //spine
    XMLItem spineItem = writer.CreateElement(packageItem, "spine");
    writer.SetAttr(spineItem, "toc", mPackage->mSpineToc.c_str());
    if (!mPackage->mSpineID.empty()) {
        writer.SetAttr(spineItem, "id", mPackage->mSpineID.c_str());
    }
    //spine/itemref
    for (size_t i = 0; i < mPackage->mSpineItems.size(); ++ i) {
        EPackageSpineItem *item = mPackage->mSpineItems.at(i);
        
        XMLItem itemref = writer.CreateElement(spineItem, "itemref");
        writer.SetAttr(itemref, "idref", item->mIDRef.c_str());
        if (!item->mID.empty()) {
            writer.SetAttr(itemref, "id", item->mID.c_str());
        }
        if (!item->mProperties.empty()) {
            writer.SetAttr(itemref, "properties", item->mProperties.c_str());
        }
    }
    
    //guide
    XMLItem guideItem = writer.CreateElement(packageItem, "guide");
    for (size_t i = 0; i < mPackage->mGuideItems.size(); ++ i) {
        EPackageGuideItem *item = mPackage->mGuideItems.at(i);
        
        XMLItem refer = writer.CreateElement(guideItem, "reference");
        writer.SetAttr(refer, "href", TPathUtil::GetReletivePath(item->mHref, mPackage->mOPFParentPath).c_str());
        writer.SetAttr(refer, "title", item->mTitle.c_str());
        writer.SetAttr(refer, "type", item->GetTypeName().c_str());
    }
    
    int size = writer.GetBufferSize();
    content.resize(size + 1, '\0');
    return writer.Write(&content[0], size);
}
