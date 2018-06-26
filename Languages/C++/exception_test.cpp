#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		std::cout << "throw 3" << std::endl;
		throw 3;
	}
	catch(int v)
	{
		std::cout << "catch " << v << std::endl;
	}

	return 0;
}