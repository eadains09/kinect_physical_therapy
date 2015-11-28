//
//  FileWriter.cpp
//

#include "FileWriter.h"
#include "stdafx.h"

FileWriter::FileWriter() {}

FileWriter::FileWriter(string fileName, string header) {
	string fullFileName = fileName + ".dat";
	currFile.open(fullFileName);
	currFile << "{" << std::endl << "\"" << header << "\": [" << std::endl;
}

void FileWriter::openBodyFrame() {
	currFile << "[" << std::endl;
}

void FileWriter::logTimestamp(double timestamp) {
	//write timestamp
	currFile << "{" << "\"timestamp\": " << timestamp << ", \"joints\": " ;
}


void FileWriter::logDataQuat(float x, float y, float z, float w)
{
	currFile
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< ", "
		<< w
		<< "]";
}

void FileWriter::logDataPoint(float x, float y, float z)
{
	currFile
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< "]";
}

void FileWriter::closeBodyFrame() {
	currFile << std::endl << "]";
}

void FileWriter::closeKeyframe() {
	currFile << std::endl << "}";
}

void FileWriter::closeFile() {
	currFile << std::endl << "]" << std::endl << "}" << std::endl;
	currFile.close();
}

void FileWriter::addComma() {
	currFile << "," << std::endl;
}


//Logging points specifically

void FileWriter::openPointLog()
{
	whereData.open("whereData.dat");
	whereData << "{" << std::endl << "\"joints\": [" << std::endl;
}

void FileWriter::firstPointBodyFrame()
{
	whereData << "[" << std::endl;
}

void FileWriter::openPointBodyFrame()
{
	whereData << "," << std::endl << "[" << std::endl;
}

void FileWriter::logPoint(float x, float y, float z)
{
	whereData
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< "]";
}

void FileWriter::closePointBodyFrame()
{
	whereData << std::endl << "]";
}

void FileWriter::closePointLog()
{
	whereData << std::endl << "]" << std::endl << "}";
	whereData.close();
}

void FileWriter::subsequentPoint()
{
	whereData << "," << std::endl;
}


//Logging Quats specifically

void FileWriter::openQuatLog()
{
	moveData.open("moveData.dat");
	moveData << "{" << std::endl << "\"jointQuats\": [" << std::endl;
}

void FileWriter::firstQuatBodyFrame()
{
	moveData << "[" << std::endl;
}

void FileWriter::openQuatBodyFrame()
{
	moveData << "," << std::endl << "[" << std::endl;
}

void FileWriter::logQuat(float x, float y, float z, float w)
{
	moveData
		<< "["
		<< x
		<< ", "
		<< y
		<< ", "
		<< z
		<< ", "
		<< w
		<< "]";
}

void FileWriter::closeQuatBodyFrame()
{
	moveData << std::endl << "]";
}

void FileWriter::closeQuatLog()
{
	moveData << std::endl << "]" << std::endl << "}";
	moveData.close();
}

void FileWriter::subsequentQuat()
{
	moveData << "," << std::endl;
}