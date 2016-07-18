#include <GeometryModel.h>

GeometryModel::GeometryModel(Handle(XCAFDoc_ShapeTool) file_info)
{
    _file_info = file_info;
    load_first_shape();
}

Standard_Boolean GeometryModel::load_second_shape(const TopoDS_Shape &shape)
{
    dist_calculator.LoadS2(shape);

    return dist_calculator.Perform();
}

Standard_Boolean GeometryModel::is_loaded()
{
    return dist_calculator.IsDone();
}

Standard_Real GeometryModel::curvature()
{
    TDF_Label main_shape_label =
        _file_info->FindMainShape(dist_calculator.SupportOnShape1(1));

    TopoDS_Shape main_shape = _file_info->GetShape(main_shape_label);
    gp_Pnt point = dist_calculator.PointOnShape1(1);
    Standard_Real curvature = std::numeric_limits<Standard_Real>::max();

    for (TopExp_Explorer exp(main_shape, TopAbs_FACE); exp.More(); exp.Next()) {
        BRepAdaptor_Surface surface(TopoDS::Face(exp.Current()));
        BRepClass_FaceClassifier classifier(
                TopoDS::Face(exp.Current()), point, surface.Tolerance());

        if (classifier.State() == TopAbs_IN || classifier.State() == TopAbs_ON) {
            ShapeAnalysis_Surface sas(surface.Surface().Surface());
            gp_Pnt2d uv = sas.ValueOfUV(point, 0.0001);

            BRepLProp_SLProps prop_calc(
                    surface, uv.Y(), uv.X(), 2, surface.Tolerance());

            curvature = std::min(curvature, prop_calc.MeanCurvature());
        }
    }
    return curvature;
}

Standard_Real GeometryModel::distance_to_boundary()
{
    if (!is_loaded()) {
        cerr << "Error: Second shape not loaded." << endl;
        return -1;
    }

    return dist_calculator.Value();
}

TopoDS_Edge GeometryModel::normal()
{
    return BRepBuilderAPI_MakeEdge(
            dist_calculator.PointOnShape1(1),
            dist_calculator.PointOnShape2(1)
            ).Edge();
}

TDF_Label GeometryModel::label_on_shape()
{
    TopoDS_Shape support = dist_calculator.SupportOnShape1(1);

    TDF_Label label;

    _file_info->Search(support, label, true, true, true);

    return label;
}

void GeometryModel::load_first_shape()
{
    TDF_LabelSequence top_shapes;
    _file_info->GetFreeShapes(top_shapes);

    if (top_shapes.Length() == 1) {
        dist_calculator.LoadS1(_file_info->GetShape(top_shapes.Value(1)));
    } else {
        TopoDS_Compound compound;
        BRep_Builder builder;

        builder.MakeCompound(compound);

        for (Standard_Integer i = 1; i <= top_shapes.Length(); i++) {
            TopoDS_Shape shape = _file_info->GetShape(top_shapes.Value(i));
            builder.Add(compound, shape);
        }

        dist_calculator.LoadS1(compound);
    }
}
