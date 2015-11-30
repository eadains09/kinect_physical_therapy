//
//  FileWriter.h
//

#ifndef _FILE_WRITER_H_
#define _FILE_WRITER_H_

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <quaternion.h>
//#include "QuatFrame.h"


using namespace std;

/*enum logType {
	QUAT = 0,
	POINTS = 1
};*/

class FileWriter {

private:
	std::ofstream moveData, whereData;
	ofstream currFile;

public:
	FileWriter();
	FileWriter(string fileName, string header);
	void openBodyFrame();
	void logTimestampMidspine(double timestamp, irr::core::vector3df midspine);
	void logDataQuat(float x, float y, float z, float w);
	void logDataPoint(float x, float y, float z);
	void closeBodyFrame();
	void closeKeyframe();
	void closeFile();
	void addComma();

	//Point specific methods
	void openPointLog();
	void firstPointBodyFrame();
	void openPointBodyFrame();
	void logPoint(float x, float y, float z);
	void closePointBodyFrame();
	void closePointLog();
	void subsequentPoint();


	//Quat specific methods
	void openQuatLog();
	void firstQuatBodyFrame();
	void openQuatBodyFrame();
	void logQuat(float x, float y, float z, float w);
	void closeQuatBodyFrame();
	void closeQuatLog();
	void subsequentQuat();


};

#endif /* _FILE_WRITER_H_ */