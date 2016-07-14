#include <cassert>
#include <cmath>
#include <ctime>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI.hxx>

void saveInSTL(TopoDS_Shape &shape, char const *filename)
{
    StlAPI::Write(shape, filename);
}

bool saveInSTEP(TopoDS_Shape &shape, char const *filename)
{
    STEPControl_Writer writer;
    writer.Transfer(shape, STEPControl_AsIs);
    IFSelect_ReturnStatus status = writer.Write(filename);

    if (status != IFSelect_RetDone) {
        return false;
    }

    return true;
}

bool saveInIGES(TopoDS_Shape &shape, char const *filename)
{
    if(!IGESControl_Controller::Init()) {
        return false;
    };

    IGESControl_Writer writer;
    writer.AddShape(shape);
    return writer.Write(filename);
}

TopoDS_Shape openSTL(char const *filename)
{
    TopoDS_Shape shape;
    StlAPI::Read(shape, filename);

    return shape;
}

TopoDS_Shape openSTEP(char const *filename)
{
    STEPControl_Reader reader;
    IFSelect_ReturnStatus status = reader.ReadFile(filename);

    assert(status == IFSelect_RetDone);

    reader.TransferRoots();

    return reader.OneShape();
}

TopoDS_Shape openIGES(char const *filename)
{
    IGESControl_Reader reader;
    IFSelect_ReturnStatus status = reader.ReadFile(filename);

    assert(status == IFSelect_RetDone);

    reader.TransferRoots();

    return reader.OneShape();
}

Standard_Real distanceTwoShapes(TopoDS_Shape &shape_1, TopoDS_Shape &shape_2, bool &inside)
{
    BRepExtrema_DistShapeShape dist_calc(shape_1, shape_2);

    dist_calc.Perform();

    inside = dist_calc.InnerSolution();

    return dist_calc.Value();
}

bool almost_equal(Standard_Real x, Standard_Real y, Standard_Real eps)
{
    return fabs(x - y) <= eps;
}

int main(int argc, char **args)
{
    // Create a sphere
    gp_Pnt center(0, 0, 0);
    Standard_Real radius = 100;

    TopoDS_Shape sphere =
        BRepPrimAPI_MakeSphere(
            center, radius
        ).Shape();

    // Save it in STL, STEP and IGES
    saveInSTL(sphere, "shape.stl");
    assert(saveInSTEP(sphere, "shape.stp"));
    assert(saveInIGES(sphere, "shape.igs"));

    // Load the sphere from the files
    TopoDS_Shape shape_from_stl = openSTL("shape.stl");
    TopoDS_Shape shape_from_step = openSTEP("shape.stp");
    TopoDS_Shape shape_from_iges = openIGES("shape.igs");

    bool inside;
    clock_t stl_speed = 0;
    clock_t step_speed = 0;
    clock_t iges_speed = 0;
    Standard_Real count_points = 0;

    for (Standard_Real x = -(radius/2); x <= (radius/2); x += (radius/5)) {
        for (Standard_Real y = -(radius/2); y <= (radius/2); y += (radius/5)) {
            for (Standard_Real z = -(radius/2); z <= (radius/2); z += (radius/5)) {
                count_points++;

                TopoDS_Vertex vertex =
                    BRepBuilderAPI_MakeVertex(gp_Pnt(x, y, z)).Vertex();

                Standard_Real expected_distance = fabs(sqrt(
                        pow(center.X() - x, 2) +
                        pow(center.Y() - y, 2) +
                        pow(center.Z() - z, 2))
                                                       - radius);

                clock_t t = clock();
                Standard_Real distance = distanceTwoShapes(shape_from_stl, vertex, inside);
                stl_speed += clock() - t;

                assert(almost_equal(distance, expected_distance, 0.3));

                t = clock();
                distance = distanceTwoShapes(shape_from_step, vertex, inside);
                step_speed += clock() - t;

                // don't check if inside
                if (!inside) {
                    assert(almost_equal(distance, expected_distance, 0.0001));
                }

                t = clock();
                distance = distanceTwoShapes(shape_from_iges, vertex, inside);
                iges_speed += clock() - t;

                assert(almost_equal(distance, expected_distance, 0.0001));
            }
        }
    }

    stl_speed /= count_points;
    step_speed /= count_points;
    iges_speed /= count_points;

    cout << "STL average speed: " << stl_speed << " ticks" << endl;
    cout << "STEP average speed: " << step_speed << " ticks" << endl;
    cout << "IGES average speed: " << iges_speed << " ticks" << endl;

    return 0;
}
