//
//  main.cpp
//  test
//
//  Created by tanwz on 16/8/3.
//  Copyright © 2016年 mpreader. All rights reserved.
//


#include "EpubSplitter.h"
#include "EpubInfo.h"
#include <iostream>
#include <vector>


void testSplitter()
{
    fprintf(stderr, "split start\n\n");
    EpubSplitter splitter("test.epub");
    int count = splitter.GetSectionCount();
    for (int i = 0; i < count; ++ i) {
        std::string name = std::to_string(i) + ".epub";
        if (!splitter.Split(i, name)) {
            fprintf(stderr, "split error(%d)\n", i);
        }
    }
    
    fprintf(stderr, "split end\n\n");
}

void testInfo()
{
    fprintf(stderr, "get info start\n\n");
    EpubInfo info("./test.epub");
    fprintf(stderr, "section count:%d\n", info.GetSectionCount());
    
    //catalog
    const std::vector<CatalogItem>& catalog = info.GetCatalogs();
    fprintf(stderr, "\ncatalog count:%d\n", (int)catalog.size());
    for (int i = 0 ; i < catalog.size(); ++ i) {
        const CatalogItem& item = catalog.at(i);
        fprintf(stderr, "catalog%d(title:%s, href:%s, section:%d, level:%d)\n", i, item.title.c_str(), item.href.c_str(), item.sectionIndex, item.level);
    }
    
    //isli
    const std::vector<ISLIMark>& marks = info.GetISLIMarks();
    fprintf(stderr, "\nmark count:%d\n", (int)marks.size());
    for (int i = 0; i < marks.size(); ++ i) {
        const ISLIMark& mark = marks.at(i);
        fprintf(stderr, "mark%d(\tsourceID:%s\n\tstarCFI:%s\n\tendCFI:%s\n)\n", i, mark.sourceID.c_str(), mark.startCFI.c_str(), mark.endCFI.c_str());
    }
    
    //mra
    int mraSize = info.GetMRAData(NULL);
    unsigned char * buffer = (unsigned char *)malloc(mraSize);
    int dataSize = info.GetMRAData(buffer);
    fprintf(stderr, "\nmra size:%d dataSize:%d\n", mraSize, dataSize);
    
    fprintf(stderr, "\nget info end\n\n");
}

int main(int argc, const char * argv[]) {
    // insert code here...
    testInfo();
    
    testSplitter();
    
    return 0;
}
