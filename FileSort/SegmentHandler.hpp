#include <filesystem>
#include <string>
#include <map>

#ifdef __linux__

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

#elif _WIN32

#include <Windows.h>

#else

#error Unsupported Version

#endif


class SegmentHandler
{

public:
	class segmenthandler_error : public std::exception
	{
	private:

		std::string _msg;

	public:

		segmenthandler_error(const std::string& msg) : _msg(msg) {}

		const char* what() const noexcept { return _msg.c_str(); }
	};

private:

	std::filesystem::path _baseDirectory;

	std::map<int, long int>	_positions;

public:

	SegmentHandler(const std::string& baseDirectory = "");
	~SegmentHandler();

	/*
	* Reads From a File count Bytes and returns a string
	*/
	std::string read(const int& segment, const int &count);

	/*
	* Writes count bytes from data to a file
	*/
	void write(const int& segment, void *data, const int& count);

	/*
	* Sets Segment File pointer to beginning of File
	* Seek / SetFilePointer
	*/
	void resetSegment(const int& segment);

	unsigned long size();
};
