#ifndef GEOMETRYMODEL_H
#define GEOMETRYMODEL_H

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <iostream>
#include <Standard_TypeDef.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ShapeTool.hxx>

enum FileType { IGES_FILE, STEP_FILE };

class GeometryModel
{
public:
    GeometryModel(Handle(XCAFDoc_ShapeTool) file_info);

    Standard_Boolean load_second_shape(const TopoDS_Shape &shape);
    Standard_Boolean is_loaded();
    Standard_Real distance_to_boundary();
    TopoDS_Edge normal();
    TDF_Label label_on_shape();
private:
    void load_first_shape();

    BRepExtrema_DistShapeShape dist_calculator;
    Handle(XCAFDoc_ShapeTool) _file_info;
};
#endif
