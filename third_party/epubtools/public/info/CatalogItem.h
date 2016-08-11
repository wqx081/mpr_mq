//
//  TCatalogItem.h
//  test
//
//  Created by tanwz on 16/8/3.
//  Copyright © 2016年 mpreader. All rights reserved.
//

#ifndef TCatalogItem_h
#define TCatalogItem_h

#include <string>


struct CatalogItem
{
    std::string title;
    std::string href;
    int         level;
    int         sectionIndex;
};


#endif /* TCatalogItem_h */
