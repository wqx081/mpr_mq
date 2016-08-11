//
//  TXMLWriter.h
//  ePubSplitter
//
//  Created by tanwz on 16/4/19.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#ifndef TXMLWriter_h
#define TXMLWriter_h

#include <stdio.h>
#include <string>

typedef void* XMLItem;

class TXMLWriter
{
public:
    TXMLWriter();
    ~TXMLWriter();
    
    XMLItem GetRoot() const;
    
    XMLItem CreateElement(XMLItem parent, const char *name);
    
    XMLItem CreateText(XMLItem element, const char *text);
    
    void    SetAttr(XMLItem element, const char *name, const char *value);
    
    int     GetBufferSize() const;
    
    bool    Write(char *buffer, int bufferSize);
    
private:
    XMLItem rootElement;
};


#endif /* TXMLWriter_h */
