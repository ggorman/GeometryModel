#ifndef GEOMETRYMODEL_H
#define GEOMETRYMODEL_H

#include <vector>
#include <vtkCell.h>
#include <vtkGenericCell.h>
#include <vtkIdList.h>
#include <vtkKdTree.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>

class GeometryModel
{
public:
    GeometryModel(vtkPolyData &pd, double resolution);
    GeometryModel(const char *filename, double resolution);

    double distance_to_boundary(const double *centre) const;
    bool is_boundary_within_radius(const double &radius, const double *centre) const;
    bool is_boundary_within_radius(const double &radius, const double *centre, double *normal, double &distance)  const;

private:
    void insert_facet_centre(const double *x0, const double *x1, const double *x2, const double *n);

    // Geometry data.
    vtkSmartPointer<vtkKdTree> locator;
    vtkSmartPointer<vtkGenericCell> generic_cell;
    vtkSmartPointer<vtkIdList> id_list;
    std::vector<double> facet_centres;
    double _resolution;
};
#endif
