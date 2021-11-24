#include "utils.h"

#include <Migine/constants.h>

#include <vector>
#include <iostream>
#include <sstream>

using namespace Migine;
using glm::quat;
using glm::vec3;
using std::cout;
using std::stringstream;
using std::string;
using std::vector;

quat Migine::EulerAnglesToQuat(vec3 eulerAngles)
{
	quat rox = glm::rotate(quat(), eulerAngles.x, { 1,0,0 });
	quat roy = glm::rotate(quat(), eulerAngles.y, { 0,1,0 });
	quat roz = glm::rotate(quat(), eulerAngles.z, { 0,0,1 });

	return rox * roy * roz;
}

quat Migine::EulerAnglesDegToQuat(vec3 eulerAngles)
{
	return EulerAnglesToQuat(eulerAngles * kDegToRad);
}



void Migine::PrintFps(float deltaTime) {
	// time is in seconds
	static float timeSinceLastPrint = 0;
	static int framesSinceLastPrint = 0;
	static int continousPrintIndex = kNoIndex;

	stringstream ss;

	timeSinceLastPrint += deltaTime;
	framesSinceLastPrint++;
	if (timeSinceLastPrint >= 0.66) {
		ss << framesSinceLastPrint / timeSinceLastPrint << " fps";
		continousPrintIndex = ContinousPrint(ss.str(), continousPrintIndex);
		timeSinceLastPrint = 0;
		framesSinceLastPrint = 0;
	}
}

int Migine::ContinousPrint(std::string s, int index) {
	static vector<string> toBePrinted;
	static int printedCharactersCount = 0;//vector<int> printedCharactersPerSS;
	if (index == kNoIndex) {
		index = toBePrinted.size();
		toBePrinted.push_back(s);
	}
	else {
		toBePrinted[index] = s;
	}
	stringstream ss;
	for (int i = 0; i < printedCharactersCount; i++) {
		ss << "\b";
	}
	for (auto s : toBePrinted) {
		ss << s << ";";
	}
	printedCharactersCount = ss.str().size();
	cout << ss.str();

	return index;
}
