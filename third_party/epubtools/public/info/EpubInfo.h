//
//  EpubInfo.h
//  test
//
//  Created by tanwz on 16/8/3.
//  Copyright © 2016年 mpreader. All rights reserved.
//

#ifndef EpubInfo_h
#define EpubInfo_h

#include "CatalogItem.h"
#include "ISLIMark.h"
#include <stdio.h>
#include <string>
#include <vector>


class TXMLReader;
class EpubPackage;
class TEpubSource;
class EpubInfo
{
public:
    EpubInfo(const std::string& filePath);
    ~EpubInfo();
    
    int GetSectionCount() const;
    const std::vector<CatalogItem>& GetCatalogs() const;
    const std::vector<ISLIMark>& GetISLIMarks() const;
    
    //当buffer为NULL时，返回数据大小
    int GetMRAData(unsigned char *buffer);
    
private:
    void ParseNav();
    bool LoadNavGroup(TXMLReader &xmlReader, void *item, int level);
    bool CreateNavNode(TXMLReader &xmlReader, void *item, int level);
    
    void ParseNCX();
    void LoadNavPoint(TXMLReader &xmlReader, void *item, int level);
    
    void LoadISLIMarks();
    
private:
    EpubPackage *mPackage;
    TEpubSource *mEpubSource;
    std::vector<CatalogItem> mCatalogs;
    std::vector<ISLIMark> mISLIMarks;
};

#endif /* EpubInfo_h */
