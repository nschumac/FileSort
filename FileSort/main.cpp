#include "FileSort.hpp"

int main(int argc, char* argv[])
{
	try
	{
		FileSort fs(10000000, 10007, 6);

		fs.Sort("C:\\Users\\Pot4t0\\Desktop\\test.txt", "C:\\Users\\Pot4t0\\Desktop\\sorted.txt");
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}