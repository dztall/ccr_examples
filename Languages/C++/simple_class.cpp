#include <stdio.h>
#include <stdlib.h>

class Shape
{
public:
	Shape() { printf("Shape constructed.\n"); }
	~Shape() { printf("Shape destructed.\n"); }
	virtual double GetArea() { return 0.0; }
	virtual void DescribeSelf() { printf("I'm Shape.\n"); }
};

class Circle :public Shape
{
protected:
	double radius;

public:
	Circle()
	{
		printf("Circle constructed.\n");
		radius = 2.0;
	}
	~Circle() { printf("Circle destructed.\n"); }
	virtual double GetArea() { return 3.141592*radius*radius; }
	virtual void DescribeSelf() { printf("I'm Circle and area is %f.\n", GetArea()); }
};

class Polygon :public Shape
{
protected:
	int edgeCount;
	double area;

public:
	Polygon()
	{
		printf("Polygon constructed.\n");
		edgeCount = 0;
		area = 0.0;
	}
	~Polygon() { printf("Polygon destructed.\n"); }
	virtual double GetArea() { return area; }
	virtual void DescribeSelf() { printf("I'm Polygon and area is %f.\n", GetArea()); }
};

class Trigon :public Polygon
{
public:
	Trigon()
	{
		printf("Trigon constructed.\n");
		edgeCount = 3;
		area = 3.0;
	}
	~Trigon() { printf("Trigon destructed.\n"); }
	virtual void DescribeSelf() { printf("I'm Trigon and area is %f.\n", GetArea()); }
};

class  Tetragon :public Polygon
{
public:
	Tetragon()
	{
		printf("Tetragon constructed.\n");
		edgeCount = 4;
		area = 4.0;
	}
	~Tetragon() { printf("Tetragon destructed.\n"); }
	virtual void DescribeSelf() { printf("I'm Tetragon and area is %f.\n", area); }
};

class Megagon :public Polygon
{
public:
	Megagon()
	{
		printf("Megagon constructed.\n");
		edgeCount = 1000000;
		area = 1000000;
	}
	~Megagon() { printf("Megagon destructed.\n"); }
	virtual void DescribeSelf() { printf("I'm Megagon and area is %f.\n", area); }
};

int main()
{
	Circle circle;
	Trigon trigon;
	Tetragon tetragon;
	Megagon megagon;

	circle.DescribeSelf();
	trigon.DescribeSelf();
	tetragon.DescribeSelf();
	megagon.DescribeSelf();

	return 0;
}