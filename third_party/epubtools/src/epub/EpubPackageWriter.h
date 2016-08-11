//
//  EpubPackageWriter.h
//  ePubSplitter
//
//  Created by tanwz on 16/4/25.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#ifndef EpubPackageWriter_h
#define EpubPackageWriter_h

#include <stdio.h>
#include <string>


class EpubPackage;
class ZipArchiveWriter;
class EpubPackageWriter
{
public:
    EpubPackageWriter(EpubPackage *package);
    bool WriteToFile(const char *path);
    
private:
    bool WriteMimetype(ZipArchiveWriter *zipWriter);
    bool WriteContainer(ZipArchiveWriter *zipWriter);
    bool WriteOPF(ZipArchiveWriter *zipWriter);
    bool WriteResources(ZipArchiveWriter *zipWriter);
    
    bool WriteItem(ZipArchiveWriter *zipWriter, const char *itemPath);
    
    bool CreateOPFXML(std::string& content);
private:
    EpubPackage *mPackage;
};

#endif /* EpubPackageWriter_h */
