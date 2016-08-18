#include "XDEReader.h"

Handle(XCAFDoc_ShapeTool) readIGES(const char *filename)
{
    Handle(TDocStd_Document) document;

    IGESCAFControl_Reader().Perform(filename, document);

    return XCAFDoc_DocumentTool::ShapeTool(document->Main());
}

Handle(XCAFDoc_ShapeTool) readSTEP(const char *filename)
{
    Handle(TDocStd_Document) document;

    STEPCAFControl_Reader().Perform(filename, document);

    return XCAFDoc_DocumentTool::ShapeTool(document->Main());
}
