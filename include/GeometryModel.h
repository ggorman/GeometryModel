#ifndef GEOMETRYMODEL_H
#define GEOMETRYMODEL_H

#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <cmath>
#include <iostream>
#include <limits>
#include <gp_Pnt2d.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <Standard_TypeDef.hxx>
#include <string>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_State.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include "XDEReader.h"

enum FileType { IGES_FILE, STEP_FILE };

class GeometryModel
{
public:
    GeometryModel(Handle(XCAFDoc_ShapeTool) file_info);
    GeometryModel(const std::string filename);

    Standard_Boolean load_second_shape(const TopoDS_Shape &shape);
    Standard_Boolean is_loaded();
    Standard_Real distance_to_boundary();
    TopoDS_Edge normal();
    TDF_Label label_on_shape();
    Standard_Real curvature();
private:
    void load_first_shape();

    BRepExtrema_DistShapeShape dist_calculator;
    Handle(XCAFDoc_ShapeTool) _file_info;
};
#endif
