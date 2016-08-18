#include "XDEReader.h"

#include <iostream>

int main(int argc, char **args) {
    Handle(XCAFDoc_ShapeTool) iges_doc = readIGES("torous_n_box.igs");
    Handle(XCAFDoc_ShapeTool) step_doc = readSTEP("torous_n_box.stp");

    return 0;
}
