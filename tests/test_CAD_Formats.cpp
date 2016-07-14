#include <cassert>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI.hxx>


void saveInSTL(TopoDS_Shape &shape, char const *filename) {
  StlAPI::Write(shape, filename);
}

bool saveInSTEP(TopoDS_Shape &shape, char const *filename) {
  STEPControl_Writer writer;
  writer.Transfer(shape, STEPControl_AsIs);
  IFSelect_ReturnStatus status = writer.Write(filename);

  if (status != IFSelect_RetDone) {
    return false;
  }

  return true;
}

bool saveInIGES(TopoDS_Shape &shape, char const *filename) {
  if(!IGESControl_Controller::Init()) {
    return false;
  };

  IGESControl_Writer writer;
  writer.AddShape(shape);
  return writer.Write(filename);
}

TopoDS_Shape openSTL(char const *filename) {
  TopoDS_Shape shape;
  StlAPI::Read(shape, filename);

  return shape;
}

TopoDS_Shape openSTEP(char const *filename) {
  STEPControl_Reader reader;
  IFSelect_ReturnStatus status = reader.ReadFile(filename);

  assert(status == IFSelect_RetDone);

  return reader.OneShape();
}

TopoDS_Shape openIGES(char const *filename) {
  IGESControl_Reader reader;
  IFSelect_ReturnStatus status = reader.ReadFile(filename);

  assert(status == IFSelect_RetDone);

  return reader.OneShape();
}

int main(int argc, char **args) {
  TopoDS_Shape sphere =
    BRepPrimAPI_MakeSphere(
        gp_Pnt(600, 600, 600), 100
    ).Shape();

  saveInSTL(sphere, "shape.stl");
  assert(saveInSTEP(sphere, "shape.stp"));
  assert(saveInIGES(sphere, "shape.igs"));

  TopoDS_Shape shape_from_stl = openSTL("shape.stl");
  TopoDS_Shape shape_from_step = openSTEP("shape.stp");
  TopoDS_Shape shape_from_iges = openIGES("shape.igs");

  return 0;
}
