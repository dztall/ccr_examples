#include <stdio.h>

struct Vector
{
	float x, y, z;
};

int main()
{
	struct Vector pos, dir;

	pos.x = 0;
	pos.y = 0;
	pos.z = 1500;

	dir.x = 0;
	dir.y = 0;
	dir.z = -1;

	struct Vector *ppos = &pos, *pdir = &dir;
	printf("pos=(%f %f %f) , dir=(%f %f %f)",
		ppos->x, ppos->y, ppos->z, pdir->x, pdir->y, pdir->z);

	return 0;
}