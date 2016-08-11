//
//  TXMLWriter.cpp
//  ePubSplitter
//
//  Created by tanwz on 16/4/19.
//  Copyright © 2016年 tanwz. All rights reserved.
//

#include "TXMLWriter.h"
#include "mxml.h"


TXMLWriter::TXMLWriter()
{
    rootElement = mxmlNewXML("1.0");
}

TXMLWriter::~TXMLWriter()
{
    mxmlDelete((mxml_node_t *)rootElement);
}

XMLItem TXMLWriter::GetRoot() const
{
    return rootElement;
}

XMLItem TXMLWriter::CreateElement(XMLItem parent, const char * name)
{
    return mxmlNewElement((mxml_node_t *)parent, name);
}

XMLItem TXMLWriter::CreateText(XMLItem element, const char * text)
{
    return mxmlNewText((mxml_node_t *)element, 1, text);
}

void TXMLWriter::SetAttr(XMLItem element, const char *name, const char *value)
{
    mxmlElementSetAttr((mxml_node_t *)element, name, value);
}

int TXMLWriter::GetBufferSize() const
{
    char buf;
    return mxmlSaveString((mxml_node_t *)rootElement, &buf, 1, MXML_NO_CALLBACK);
}

bool TXMLWriter::Write(char *buffer, int bufferSize)
{
    return (mxmlSaveString((mxml_node_t *)rootElement, buffer, bufferSize, MXML_NO_CALLBACK) > 0);
}
