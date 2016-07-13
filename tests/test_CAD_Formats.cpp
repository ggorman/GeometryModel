#include <cassert>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI.hxx>


void saveInSTL(TopoDS_Shape &shape) {
  cout << "Saving in STL";
  StlAPI::Write(shape, "shape.stl");
}

bool saveInSTEP(TopoDS_Shape &shape) {
  STEPControl_Writer writer;
  writer.Transfer(shape, STEPControl_AsIs);
  IFSelect_ReturnStatus status = writer.Write("shape.stp");

  if (status != IFSelect_RetDone) {
    return false;
  }

  return true;
}

bool saveInIGES(TopoDS_Shape &shape) {
  if(!IGESControl_Controller::Init()) {
    return false;
  };

  IGESControl_Writer writer;
  writer.AddShape(shape);
  return writer.Write("shape.igs");
}

int main(int argc, char **args) {
  TopoDS_Shape sphere =
    BRepPrimAPI_MakeSphere(
        gp_Pnt(600, 600, 600), 100
    ).Shape();

  saveInSTL(sphere);
  assert(saveInSTEP(sphere) == true);
  assert(saveInIGES(sphere) == true);

  return 0;
}
