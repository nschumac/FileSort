#include "FileSort.hpp"

int main(int argc, char* argv[])
{
	try
	{
		FileSort fs(10000000, 1000007, 6);
		if (argc == 3)
		{
			fs.Sort(argv[1], argv[2]);
		}
		fs.Sort("C:\\Users\\Pot4t0\\Desktop\\test.txt", "C:\\Users\\Pot4t0\\Desktop\\sorted.txt");
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}