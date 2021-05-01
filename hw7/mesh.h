#ifndef Mesh_H
#define Mesh_H

#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>

#include <fstream>
#include <string>

#include "vector.h"
#include "traingle.h"

class Mesh {

	public:
		Mesh() {}
		
	
		void add(Triangle t) { Triangles.push_back(t); }
	
		void add(const Point3& vertex1, 
				 const Point3& vertex2,
				 const Point3& vertex3)
		{
			Triangle t(vertex1, vertex2, vertex3);
			Triangles.push_back(t);
		}

		void add(const Point3& vertex1, 
				 const Point3& vertex2,
				 const Point3& vertex3,
				 const Point3& norm)
		{
			Triangle t(vertex1, vertex2, vertex3, norm);
			Triangles.push_back(t);
		}
	

		Point3 get_normal(const Point3& vertex1, 
				 const Point3& vertex2,
				 const Point3& vertex3)
		{
			Triangle t(vertex1, vertex2, vertex3);
			Point3 p = t.normal();
			return p;
		}

		void clear() { Triangles.clear(); }


		int size() { return Triangles.size(); }


		Mesh& scale(const Vec3& s)
		{
			
			double x, y, z;
			x = ((s.x()!=0) ? s.x() : 1);
			y = ((s.y()!=0) ? s.y() : 1);
			z = ((s.z()!=0) ? s.z() : 1);
			Vec3 _s(x, y, z);

			Vec3 v1, v2, v3;
			for (auto& t : Triangles) {
				v1 = t.v1 * _s;
				v2 = t.v2 * _s;
				v3 = t.v3 * _s;
				t = {v1, v2, v3};
			}
			return *this;
		}

	
		Mesh& translate(const Vec3& trans)
		{
			Vec3 v1, v2, v3;
			for (auto& t : Triangles) {
				v1 = t.v1 + trans;
				v2 = t.v2 + trans;
				v3 = t.v3 + trans;
				t = {v1, v2, v3};
			}
			return *this;
		}

	
		Mesh& rotate(double deg, const Vec3& n)
		{
			Vec3 v1, v2, v3;
			for (auto& t : Triangles) {
				v1 = rodrigues_rotate(t.v1, deg, n);
				v2 = rodrigues_rotate(t.v2, deg, n);
				v3 = rodrigues_rotate(t.v3, deg, n);
				t = {v1, v2, v3};
			}
			return *this;
		}

		
		Mesh& flip_normal()
		{
			for( auto& t : Triangles) {
				t.flip_normal();
			}
			return *this;
		}

		
		void write_stl(const std::string& filename, const std::string& name="") const {
			std::ofstream file(filename);

			file << "solid" << " " << name << std::endl;
			for (const auto& t : Triangles) {
				file << "\t" << "facet normal" << " " << t.normal() << std::endl;
				file << "\t\t" << "outer loop" << std::endl;
				file << "\t\t\t" << " " << "vertex" << " " << t.vertex1() << std::endl;
				file << "\t\t\t" << " " << "vertex" << " " << t.vertex2() << std::endl;
				file << "\t\t\t" << " " << "vertex" << " " << t.vertex3() << std::endl;
				file << "\t\t" << "endloop" << std::endl;
				file << "\t" << "endfacet" << std::endl;
			}
			file << "endsolid" << " " << name << std::endl;

			file.close();
		}


		Mesh& operator+=(const Mesh &m) {
			Triangles.insert(Triangles.end(), (m.Triangles).begin(), (m.Triangles).end());
			return *this;
		}

	public:
		std::vector<Triangle> Triangles;

};


inline Mesh operator+(const Mesh &m1, const Mesh &m2) {
	Mesh m;
	(m.Triangles).insert((m.Triangles).end(), (m1.Triangles).begin(), (m1.Triangles).end());
	(m.Triangles).insert((m.Triangles).end(), (m2.Triangles).begin(), (m2.Triangles).end());
	return m;
}

inline Mesh create_quadrilateral (const Point3& p1,
					const Point3& p2,
			   		const Point3& p3,
			   		const Point3& p4)
{
	Mesh m;
	m.add(p1, p2, p4);
	m.add(p4, p2, p3);
	return m;
};

inline Mesh create_cylinder(double x, double y, double z, double r, double h, int fn)
{	
	Mesh m;
	Point3 vertex1, vertex2, vertex3, norm;

	const double pi = 3.141592654;
	double perAngle = 2*pi/fn;

	for(int i = 0; i < fn; i++)
	{
		double angle = i * perAngle;
		double rcos = cos(angle)*r;
		double rsin = sin(angle)*r;
		double rpcos = cos(angle-perAngle)*r;
		double rpsin = sin(angle-perAngle)*r;

		vertex1 = {rcos, rsin, h};
		vertex2 = {rpcos, rpsin, h};
		vertex3 = {rpcos, rpsin, 0};
		norm = get_normal(vertex1, vertex2, vertex3);
		m.add(vertex1, vertex2, vertex3);

		vertex1 = {rpcos, rpsin, 0};
		vertex2 = {rcos, rsin, 0};
		vertex3 = {rcos, rsin, h};
		m.add(vertex1, vertex2, vertex3, norm);
	}

	m.translate(Vec3(x, y, z));
	return m;
}

inline Mesh create_cube (double x, double y, double z, double side)
{
	Mesh m;
	Point3 vertex1, vertex2, vertex3;

	vertex1 = {0, 0, side};
	vertex2 = {side, 0, side};
	vertex3 = {0, side, side};
	m.add(vertex1, vertex2, vertex3);

	vertex1 = {side, side, side};
	vertex2 = {0, side, side};
	vertex3 = {side, 0, side};
	m.add(vertex1, vertex2, vertex3);

	vertex1 = {side, 0, side};
	vertex2 = {side, 0, 0};
	vertex3 = {side, side, side};
	m.add(vertex1, vertex2, vertex3);

	vertex1 = {side, side, 0};
	vertex2 = {side, side, side};
	vertex3 = {side, 0, 0};
	m.add(vertex1, vertex2, vertex3);

	vertex1 = {side, 0, 0};
	vertex2 = {0, 0, 0};
	vertex3 = {side, side, 0};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {0, side, 0};
	vertex2 = {side, side, 0};
	vertex3 = {0, 0, 0};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {0, 0, 0};
	vertex2 = {0, 0, side};
	vertex3 = {0, side, 0};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {0, side, side};
	vertex2 = {0, side, 0};
	vertex3 = {0, 0, side};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {0, side, side};
	vertex2 = {side, side, side};
	vertex3 = {0, side, 0};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {side, side, 0};
	vertex2 = {0, side, 0};
	vertex3 = {side, side, side};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {side, 0, side};
	vertex2 = {0, 0, side};
	vertex3 = {side, 0, 0};
	m.add(vertex1, vertex2, vertex3);
	
	vertex1 = {0, 0, 0};
	vertex2 = {side, 0, 0};
	vertex3 = {0, 0, side};
	m.add(vertex1, vertex2, vertex3);

	m.translate(Vec3(x, y, z));
	return m;
}


#endif