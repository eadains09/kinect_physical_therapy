//
//  FileReader.h
//
#ifndef _FILE_READER_H_
#define _FILE_READER_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>


using namespace std;

class FileReader {

private:
	ifstream currFile;

public:
	FileReader();
	FileReader(string filePath);

	bool isOpen();
	//char getChar();
	void findFileStart();
	bool findKeyframeStart();
	double findTimestamp();
	bool findJointStart();
	double findDouble();
	void closeFile();
};

#endif /* _FILE_READER_H_ */