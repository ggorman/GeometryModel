#include "XDEReader.h"

Handle(XCAFDoc_ShapeTool) readIGES(const char *filename)
{
    Handle(TDocStd_Document) document = new TDocStd_Document(Storage::Version());

    IGESCAFControl_Reader reader;
    reader.SetNameMode(true);
    reader.SetLayerMode(true);
    reader.SetColorMode(true);

    reader.ReadFile(filename);
    reader.Transfer(document);

    return XCAFDoc_DocumentTool::ShapeTool(document->Main());
}

Handle(XCAFDoc_ShapeTool) readSTEP(const char *filename)
{
    Handle(TDocStd_Document) document = new TDocStd_Document(Storage::Version());

    STEPCAFControl_Reader reader;
    reader.SetNameMode(true);
    reader.SetLayerMode(true);
    reader.SetColorMode(true);

    reader.ReadFile(filename);
    reader.Transfer(document);

    return XCAFDoc_DocumentTool::ShapeTool(document->Main());
}

