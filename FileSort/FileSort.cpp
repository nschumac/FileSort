#include "FileSort.hpp"

FileSort::FileSort(int maxFileSizeBytes, int numberOfLinesPerSegment, int lineSizeBytes)
	: _maxFileSizeBytes(maxFileSizeBytes), _numberOfLinesPerSegment(numberOfLinesPerSegment), _lineSizeBytes(lineSizeBytes)
{
}

void FileSort::Sort(const std::string& inFilePath, const std::string& outFilePath)
{

	try
	{
		std::filesystem::path _inFilePath(inFilePath);

		// check some stuff for infile

		if (!std::filesystem::exists(_inFilePath))
			throw filesort_error("In File: path does not exist!");

		if (!std::filesystem::is_regular_file(_inFilePath))
			throw filesort_error("In File: is not a regular File, support only regular Files!");

		// check perms add user perms
		std::filesystem::perms inFilePerms = std::filesystem::status(_inFilePath).permissions();

		if ((inFilePerms & std::filesystem::perms::owner_all) == std::filesystem::perms::none)
			throw filesort_error("In File: Incorrect Permissions");

		uintmax_t _currentFileSize = std::filesystem::file_size(_inFilePath);
		if (_currentFileSize > static_cast<uintmax_t>(_maxFileSizeBytes))
			throw filesort_error("In File: file size is too large!");

#ifdef  __linux__

		FILE* _iInFile = fopen(_inFilePath.c_str(), "r");

		if (!_iInFile)
			throw filesort_error("In File: Could not Create File ptr!");

#elif _WIN32 || _WIN64

		HANDLE _iInFile = CreateFile(_inFilePath.native().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (_iInFile == INVALID_HANDLE_VALUE)
			throw filesort_error("In File: Could not Create Handle!");
#endif


#ifdef __linux__
		long int numberRead = 0;
		long int numberWritten = 0;
#elif _WIN32 || _WIN64
		DWORD numberRead = 0;
		DWORD numberWritten = 0;
#endif
		SegmentHandler segmentHandler;
		int currentSegment = 0;

		std::vector<char> buffer;
		buffer.resize(_lineSizeBytes);

		std::vector<std::string> _readLines;

		do
		{
#ifdef __linux__
			numberRead = fread(buffer.data(), 1, _lineSizeBytes, _iInFile);
#elif _WIN32 || _WIN64
			if (!ReadFile(_iInFile, static_cast<LPVOID>(buffer.data()), _lineSizeBytes, &numberRead, NULL))
				throw filesort_error("Read File: error");
#endif
			if (numberRead > 0)
				_readLines.push_back(std::string(buffer.begin(), buffer.end()));

			if (_readLines.size() % _numberOfLinesPerSegment == 0)
			{
				std::sort(_readLines.begin(), _readLines.end());

				for (auto& s : _readLines)
					segmentHandler.write(currentSegment, (void*)s.c_str(), s.length());
				_readLines.clear();
				currentSegment++;
			}
		} while (numberRead != 0);

		for (auto& s : _readLines)
			segmentHandler.write(currentSegment, (void*)s.c_str(), s.length());
		_readLines.clear();


		std::filesystem::path _outFilePath(outFilePath);

#ifdef __linux__
		int fd = open(_outFilePath.c_str(), O_TRUNC | O_RDWR | O_CREAT, 0666);
		FILE* _oOutFile = fdopen(fd, "r+");
#elif _WIN32 || _WIN64
		HANDLE _oOutFile = CreateFile(_outFilePath.native().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

		std::multimap<std::string, int> lines;

		for (unsigned long i = 0; i < segmentHandler.size(); ++i)
			lines.insert(std::make_pair(segmentHandler.read(i, _lineSizeBytes), i));

		while (!lines.empty())
		{
#ifdef __linux__
			fwrite(lines.begin()->first.c_str(), 1, lines.begin()->first.length(), _oOutFile);
#elif _WIN32 || _WIN64
			if (!WriteFile(_oOutFile, lines.begin()->first.c_str(), lines.begin()->first.length(), &numberWritten, NULL))
				throw filesort_error("Write File: error");
#endif
			int idx = lines.begin()->second;
			lines.erase(lines.begin());

			std::string read = segmentHandler.read(idx, _lineSizeBytes);
			if (read == "")
				continue;
			lines.insert(std::make_pair(read, idx));
		}

#ifdef __linux__
		fclose(_iInFile);
		fclose(_oOutFile);
#elif _WIN32 || _WIN64
		CloseHandle(_iInFile);
		CloseHandle(_oOutFile);
#endif
	}
	catch (std::filesystem::filesystem_error& e)
	{
		throw filesort_error("FileSystem Error: " + std::string(e.what()));
	}
	catch (SegmentHandler::segmenthandler_error& e)
	{
		throw filesort_error("Segment Handler Error: " + std::string(e.what()));
	}
}
