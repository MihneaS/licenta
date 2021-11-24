#include "utils.h"

#include <Migine/constants.h>

#include <sstream>
#include <iostream>

using namespace Migine;
using glm::quat;
using glm::vec3;
using std::cout;
using std::stringstream;

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
	static int lastPrintedStringLength = 0;
	static float timeSinceLastPrint = 0;
	static int framesSinceLastPrint = 0;

	stringstream ss;

	timeSinceLastPrint += deltaTime;
	framesSinceLastPrint++;
	if (timeSinceLastPrint >= 0.66) {
		for (int i = 0; i < lastPrintedStringLength; i++) {
			cout << "\b";
		}
		ss << framesSinceLastPrint / timeSinceLastPrint << " fps";
		lastPrintedStringLength = ss.str().size();
		cout << ss.str();
		timeSinceLastPrint = 0;
		framesSinceLastPrint = 0;
	}
}
