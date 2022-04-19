#include "SegmentHandler.hpp"

SegmentHandler::SegmentHandler(const std::string& baseDirectory)
	: _baseDirectory(baseDirectory)
{
	// set to current Directory
	if (baseDirectory == "" || !std::filesystem::exists(_baseDirectory))
	{
		_baseDirectory = std::filesystem::current_path();
	}

	std::filesystem::perms p = std::filesystem::status(_baseDirectory).permissions();

	if ((p & std::filesystem::perms::owner_all) == std::filesystem::perms::none)
		throw segmenthandler_error("Wrong Perms on Directory!");

}

std::string SegmentHandler::read(const int& segment, const int& count)
{

	std::filesystem::path segmentPath = _baseDirectory;

	segmentPath.append("seg" + std::to_string(segment) + ".txt");

#ifdef __linux__
	FILE* segmentFile;

#elif _WIN32

	HANDLE segmentFile;

#endif


	/*
	* check if file has already been created
	*/
	if (!std::filesystem::exists(segmentPath))
	{
		_positions.insert(std::make_pair(segment, 0));

#ifdef __linux__

		int fd = open(segmentPath.c_str(), O_RDWR | O_CREAT, 0666);

		if (fd == -1)
			throw segmenthandler_error("Could not create File!");

		segmentFile = fdopen(fd, "r");

		if (!segmentFile)
			throw segmenthandler_error("Could not create File!");
			
#elif _WIN32

		segmentFile = CreateFile(segmentPath.native().c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (segmentFile == INVALID_HANDLE_VALUE)
			throw segmenthandler_error("Could not create File!");
#endif
	}
	else
	{
#ifdef __linux__

		segmentFile = fopen(segmentPath.c_str(), "r");

		if (!segmentFile)
			throw segmenthandler_error("Could not open File!");
			//throw Error

		if (fseek(segmentFile, _positions[segment], SEEK_SET))
		{
			fclose(segmentFile);
			throw segmenthandler_error("Could not set File Position!");
		}

#elif _WIN32

		segmentFile = CreateFile(segmentPath.native().c_str(), GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (segmentFile == INVALID_HANDLE_VALUE)
			throw segmenthandler_error("Could not open File!");

		if (SetFilePointer(segmentFile, _positions[segment], NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			CloseHandle(segmentFile); //important or else it cannot remove the files
			throw segmenthandler_error("Could not set File Position");
		}
#endif
	}



	std::vector<char> buffer;
	buffer.reserve(count);

#ifdef __linux__

	int bytesRead = 0;

	bytesRead = fread(buffer.data(), 1, count, segmentFile);
	fclose(segmentFile);

	if (bytesRead < 0)
		throw segmenthandler_error("Read Error!");
#elif _WIN32

	DWORD bytesRead = 0;

	if (!ReadFile(segmentFile, static_cast<LPVOID>(buffer.data()), count, &bytesRead, NULL))
	{
		CloseHandle(segmentFile);
		throw segmenthandler_error("Read Error!");
	}
	CloseHandle(segmentFile);
#endif

	_positions[segment] += bytesRead;

	return std::string(buffer.data(), buffer.data() + bytesRead);
}


void SegmentHandler::write(const int& segment, void* data, const int& count)
{

	std::filesystem::path segmentPath = _baseDirectory;

	segmentPath.append("seg" + std::to_string(segment) + ".txt");

#ifdef __linux__
	FILE* segmentFile;

#elif _WIN32

	HANDLE segmentFile;

#endif


	/*
	* check if file has already been created
	*/
	if (!std::filesystem::exists(segmentPath))
	{
		_positions.insert(std::make_pair(segment, 0));

#ifdef __linux__

		int fd = open(segmentPath.c_str(), O_RDWR | O_CREAT, 0666);

		if (fd == -1)
			throw segmenthandler_error("Could not create File!");

		segmentFile = fdopen(fd, "w");

		if (!segmentFile);
		throw segmenthandler_error("Could not create File!");

#elif _WIN32

		segmentFile = CreateFile(segmentPath.native().c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (segmentFile == INVALID_HANDLE_VALUE)
			throw segmenthandler_error("Could not create File!");
#endif
	}
	else
	{
#ifdef __linux__

		segmentFile = fopen(segmentPath.c_str(), "a");

		if (!segmentFile)
			throw segmenthandler_error("Could not open File!");
		//throw Error

		if (fseek(segmentFile, 0, SEEK_END))
		{
			fclose(segmentFile);
			throw segmenthandler_error("Could not set File Position!");
	}

#elif _WIN32

		segmentFile = CreateFile(segmentPath.native().c_str(), FILE_APPEND_DATA, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (segmentFile == INVALID_HANDLE_VALUE)
			throw segmenthandler_error("Could not open File!");
#endif
	}

#ifdef __linux__

	int bytesWritten = 0;

	bytesWritten = fread(data, 1, count, segmentFile);

	fclose(segmentFile);
	if (bytesWritten < 0)
		throw segmenthandler_error("Write Error");

#elif _WIN32

	DWORD bytesWritten = 0;

	if (!WriteFile(segmentFile, static_cast<LPVOID>(data), count, &bytesWritten, NULL))
	{
		CloseHandle(segmentFile);
		throw segmenthandler_error("Write Error");
	}

	CloseHandle(segmentFile);
#endif
}


void SegmentHandler::resetSegment(const int &segment)
{
	std::filesystem::path segmentPath = _baseDirectory;

	segmentPath.append("seg" + std::to_string(segment) + ".txt");

	if (!std::filesystem::remove(segmentPath))
		throw segmenthandler_error("Could not reset File");
}


#include <iostream>

SegmentHandler::~SegmentHandler()
{
	std::filesystem::path segmentPath;



	while (_positions.size() > 0)
	{
		segmentPath = _baseDirectory;
		segmentPath.append("seg" + std::to_string(_positions.begin()->first) + ".txt");
		try {
			std::filesystem::remove(segmentPath);//could  somehow handle error but dont want to throw in here
		}
		catch (std::filesystem::filesystem_error& e)
		{
			std::cout << e.what() << std::endl;
		}
		_positions.erase(_positions.begin());
	}
}

unsigned long SegmentHandler::size()
{
	return _positions.size();
}
