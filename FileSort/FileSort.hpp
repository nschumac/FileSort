#pragma once

#include <iostream>
#include <string>
#include <algorithm>

#include "SegmentHandler.hpp"

class FileSort
{
public:

	class filesort_error : public std::exception
	{
	private:

		std::string _msg;

	public:

		filesort_error(const std::string &msg) : _msg(msg) {}

		const char* what() const noexcept { return _msg.c_str(); }
	};

private:

	const int _maxFileSizeBytes;
	const int _numberOfLinesPerSegment;
	const int _lineSizeBytes;

	FileSort() : _maxFileSizeBytes(), _numberOfLinesPerSegment(), _lineSizeBytes() {}

public:

	FileSort(int maxFileSizeBytes, int numberOfLinesPerSegment, int lineSizeBytes);

	void Sort(const std::string& inFilePath, const std::string& outFilePath);

	~FileSort(){}

};