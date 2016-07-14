#ifndef XDEREADER_H
#define XDEREADER_H

#include <IGESCAFControl_Reader.hxx>
#include <Standard_Macro.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

Handle(XCAFDoc_ShapeTool) readIGES(const char *filename);
Handle(XCAFDoc_ShapeTool) readSTEP(const char *filename);

#endif
