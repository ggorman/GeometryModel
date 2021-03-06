#include "GeometryModel.h"

#include <cassert>
#include <cmath>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>

Handle(XCAFDoc_ShapeTool) create_test_shapes(
        TDF_Label &sphere_label, TDF_Label &box_label) {

    
    // Create a sphere
    gp_Pnt center(0, 0, 0);
    Standard_Real radius = 10;

    TopoDS_Shape sphere =
        BRepPrimAPI_MakeSphere(
                center, radius
                ).Shape();

    // Create a box 
    gp_Pnt corner(-10, -10, 30);
    TopoDS_Solid box =
        BRepPrimAPI_MakeBox(
                corner, 20, 20, 20
                ).Solid();


    // Create ShapeTool and add the shapes
    Handle(TDocStd_Document) doc = new TDocStd_Document("");
    Handle(XCAFDoc_ShapeTool) shapes = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    sphere_label = shapes->AddShape(sphere);
    box_label = shapes->AddShape(box);

    return shapes;
}

int main(int argc, char **argv)
{
    TDF_Label sphere_label;
    TDF_Label box_label;
    GeometryModel geo_model(create_test_shapes(sphere_label, box_label));

    TopoDS_Vertex test_1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 0)).Vertex();

    assert(geo_model.load_second_shape(test_1));

    assert(geo_model.curvature() == -0.1);
    assert(geo_model.distance_to_boundary() == 10);
    assert(geo_model.label_on_shape().IsDescendant(sphere_label));

    TopoDS_Vertex test_2 = BRepBuilderAPI_MakeVertex(gp_Pnt(0, 0, 80)).Vertex();

    assert(geo_model.load_second_shape(test_2));

    assert(geo_model.curvature() == 0);
    assert(geo_model.distance_to_boundary() == 30);
    assert(geo_model.label_on_shape().IsDescendant(box_label));

    double bbox[6];
    double target_bbox[6] = { -10, 10, -10, 10, -10, 50 };
    geo_model.bounding_box(bbox);

    for (int i = 0; i < 6; i++) {
        assert(fabs(bbox[i]-target_bbox[i]) < 10e-6);
    }

    return 0;
}
