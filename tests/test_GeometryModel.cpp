#include "GeometryModel.h"

#include <cassert>
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>

int main(int argc, char **argv)
{
    // Create sphere geometry.
    vtkSmartPointer<vtkSphereSource> sphere=vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetRadius(100.0);
    sphere->SetCenter(600, 600, 600);
    sphere->SetPhiResolution(180);
    sphere->SetThetaResolution(360);
    sphere->Update();

    vtkSmartPointer<vtkPolyData> pd=vtkSmartPointer<vtkPolyData>::New();

    double resolution = 1.0;
    GeometryModel geometry(*sphere->GetOutput(), resolution);

    double dx = 50.0;
    double normal[3], distance;
    for(int i=0; i<40; i++) {
        for(int j=0; j<40; j++) {
            for(int k=0; k<40; k++) {
                double v[] = {-1000.0+i*dx, -1000.0+j*dx, -1000.0+k*dx};
                double r = sqrt((v[0]-600)*(v[0]-600) + (v[1]-600)*(v[1]-600) + (v[2]-600)*(v[2]-600));

                double distance_to_boundary = geometry.distance_to_boundary(v);
                double exact_distance_to_boundary = r - 100.0;
                assert(fabs(distance_to_boundary-exact_distance_to_boundary)<1.0);

                if(fabs(distance_to_boundary)<100) {
                    assert(geometry.is_boundary_within_radius(100.0+resolution, v));
                    assert(geometry.is_boundary_within_radius(100.0+resolution, v, normal, distance));
                    assert(fabs(distance-distance_to_boundary)<1.0);
                } else {
                    assert(!geometry.is_boundary_within_radius(100.0-resolution, v));
                    assert(!geometry.is_boundary_within_radius(100.0-resolution, v, normal, distance));
                }
            }
        }
    }

    return 0;
}

