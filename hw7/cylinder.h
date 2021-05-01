#include "mesh.h"
#include <string>

using namespace std;

class Cylinder{
    private:
    double x, y, z, r, h;
    int fn;
    Mesh m;
    public:
    Cylinder(double x, double y, double z, double r, double h, int fn): m(create_cylinder(x, y, z, r, h, fn)){ }

    void print(string filename){
        m.write_stl(filename, "Cylinder");
    }
};