#include "XDEReader.h"
#include "GeometryModel.h"

#include <cassert>
#include <cmath>

int main(int argc, char **args) {
    GeometryModel geom_model_igs("torous_n_box.igs");
    GeometryModel geom_model_stp("torous_n_box.stp");

    double bbox_igs[6];
    double bbox_stp[6];

    geom_model_igs.bounding_box(bbox_igs);
    geom_model_stp.bounding_box(bbox_stp);

    for (int i = 0; i < 6; i++) {
        assert(fabs(bbox_igs[i] - bbox_stp[i]) < 10e-6);
    }

    return 0;
}
