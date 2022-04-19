#include "FileSort.hpp"

int main(int argc, char* argv[])
{
	try
	{
		FileSort fs(10000000, 3, 5);
		if (argc == 3)
		{
			fs.Sort(argv[1], argv[2]);
		}
		fs.Sort("test.txt", "sorted.txt");
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}