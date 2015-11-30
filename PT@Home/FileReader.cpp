//
//  FileReader.cpp
//

#include "FileReader.h"
#include "stdafx.h"

FileReader::FileReader() {}

FileReader::FileReader(string filePath) {
	currFile.open(filePath);
}

bool FileReader::isOpen() {
	return currFile.is_open();
}

/*char FileReader::getChar() {
	return currFile.get();
}*/

void FileReader::findFileStart() {
	currFile.ignore(256, '\"');
}

bool FileReader::findKeyframeStart() {
	char c;

	c = currFile.get();
	
	while (c != '{') {		
		// if (c == '}') {
		// 	return false;
		// }

		if (currFile.eof()) {
			return false;
		}

		c = currFile.get();

	}

	return true;
}

double FileReader::findTimestamp() {
	char c;
	std::string str;
	double timestamp;

	currFile.ignore(13, ' ');
	timestamp = findDouble();

	return timestamp;
}

bool FileReader::findJointStart() {
	char c;

	c = currFile.get();
	
	while (c != '[') {		
		if (c == ']') {
			return false;
		}

		if (currFile.eof()) {
			return false;
		}

		c = currFile.get();

	}

	return true;
}

double FileReader::findDouble() {
	char c;
	std::string str;
	double x;

	currFile.get(c);
	while(c == ' ') {
		currFile.get(c);
	}
	while (c != ',' && c != ']') {
		str.append(1, c);
		currFile.get(c);
	}

	x = stod(str, NULL);

	return x;
}

void FileReader::closeFile() {
	currFile.close();
}
