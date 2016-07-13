#include <GeometryModel.h>

#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>

GeometryModel::GeometryModel(vtkPolyData &pd, double resolution)
{
    _resolution = resolution;

    int nfacets = pd.GetNumberOfCells();
    for(int i=0; i<nfacets; i++) {
        // Skip anything that is not a triangle.
        vtkCell *facet = pd.GetCell(i);
        if(facet->GetCellType()!=VTK_TRIANGLE) {
            continue;
        }

        double v[9];
        for(int j=0; j<3; j++)
            pd.GetPoint(facet->GetPointId(j), v+j*3);

        double n[3];
        vtkTriangle::ComputeNormal(v, v+3, v+6, n);
        insert_facet_centre(v, v+3, v+6, n);
    }

    int npts = facet_centres.size()/6;
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(npts);
    for(int i=0; i<npts; i++) {
        pts->SetPoint(i, facet_centres.data()+i*6);
    }
    generic_cell = vtkSmartPointer<vtkGenericCell>::New();
    id_list = vtkSmartPointer<vtkIdList>::New();

    locator = vtkSmartPointer<vtkKdTree>::New();
    locator->BuildLocatorFromPoints(pts);
}

GeometryModel::GeometryModel(const char *filename, double resolution)
{
    _resolution = resolution;

    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(filename);
    reader->MergingOn();

    vtkSmartPointer<vtkTriangleFilter> triangle_filter=vtkSmartPointer<vtkTriangleFilter>::New();
    triangle_filter->SetInputConnection(reader->GetOutputPort());
    triangle_filter->Update();

    vtkPolyData &pd = *triangle_filter->GetOutput();

    int nfacets = pd.GetNumberOfCells();
    for(int i=0; i<nfacets; i++) {
        // Skip anything that is not a triangle.
        vtkCell *facet = pd.GetCell(i);
        if(facet->GetCellType()!=VTK_TRIANGLE) {
            continue;
        }

        double v[9];
        for(int j=0; j<3; j++)
            pd.GetPoint(facet->GetPointId(j), v+j*3);

        double n[3];
        vtkTriangle::ComputeNormal(v, v+3, v+6, n);
        insert_facet_centre(v, v+3, v+6, n);
    }

    int npts = facet_centres.size()/6;
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    pts->SetDataTypeToDouble();
    pts->SetNumberOfPoints(npts);
    for(int i=0; i<npts; i++) {
        pts->SetPoint(i, facet_centres.data()+i*6);
    }
    generic_cell = vtkSmartPointer<vtkGenericCell>::New();
    id_list = vtkSmartPointer<vtkIdList>::New();

    locator = vtkSmartPointer<vtkKdTree>::New();
    locator->BuildLocatorFromPoints(pts);
}

double GeometryModel::distance_to_boundary(const double *centre) const
{
    // vtkSmartPointer<vtkIdList> pnts = vtkSmartPointer<vtkIdList>::New();
    locator->FindClosestNPoints(1, centre, id_list);

    vtkIdType ifacet = id_list->GetId(0);
    // vtkIdType ifacet = locator->FindClosestPoint(centre);
    assert(ifacet>=0);

    const double *pn = facet_centres.data()+6*ifacet+3;
    double r[3];
    for(int k=0; k<3; k++)
        r[k] = (centre[k]-facet_centres[6*ifacet+k]);

    return r[0]*pn[0]+r[1]*pn[1]+r[2]*pn[2];
}

bool GeometryModel::is_boundary_within_radius(const double &radius, const double *centre)  const
{
    double dist2;
    vtkIdType ifacet = locator->FindClosestPointWithinRadius(radius, centre, dist2);

    return (ifacet>=0);
}

bool GeometryModel::is_boundary_within_radius(const double &radius, const double *centre, double *normal, double &distance)  const
{

    // Check if this tet intersects with surface.
    double dist2;
    vtkIdType ifacet = locator->FindClosestPointWithinRadius(radius, centre, dist2);

    if(ifacet<0)
        return false;

    // Return a copy of the normal.
    const double *pn = facet_centres.data()+6*ifacet+3;
    assert(fabs(pn[0]*pn[0]+pn[1]*pn[1]+pn[2]*pn[2]-1.0)<1e-6);
    for(int i=0; i<3; i++)
        normal[i] = pn[i];

    // Calculate normal distance.
    distance=0;
    for(int i=0; i<3; i++)
        distance += (centre[i] - facet_centres[6*ifacet+i])*pn[i];

    return true;
}

void GeometryModel::insert_facet_centre(const double *x0, const double *x1, const double *x2, const double *n)
{
    double bounds[6];
    for(int i=0; i<3; i++) {
        bounds[i*2  ] = x0[i];
        bounds[i*2+1] = x0[i];
    }
    for(int i=0; i<3; i++) {
        bounds[i*2  ] = std::min(bounds[i*2  ], std::min(x1[i], x2[i]));
        bounds[i*2+1] = std::max(bounds[i*2+1], std::max(x1[i], x2[i]));
    }

    double maxl = std::max(bounds[1]-bounds[0], std::max(bounds[3]-bounds[2], bounds[5]-bounds[4]));

    if(maxl<_resolution) {
        for(int i=0; i<3; i++) {
            facet_centres.push_back((x0[i]+x1[i]+x2[i])/3);
        }
        for(int i=0; i<3; i++)
            facet_centres.push_back(n[i]);
    } else {
        double x3[3], x4[3], x5[3];
        for(int i=0; i<3; i++) {
            x3[i] = (x0[i]+x1[i])*0.5;
            x4[i] = (x1[i]+x2[i])*0.5;
            x5[i] = (x2[i]+x0[i])*0.5;
        }
        insert_facet_centre(x0, x3, x5, n);
        insert_facet_centre(x3, x1, x4, n);
        insert_facet_centre(x5, x4, x2, n);
        insert_facet_centre(x5, x3, x4, n);

    }
}
