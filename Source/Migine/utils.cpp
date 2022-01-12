#include "utils.h"

#include <Migine/constants.h>

#include <vector>
#include <iostream>
#include <sstream>

using namespace Migine;
using glm::quat;
using glm::vec3;
using glm::vec4;
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

vec4 Migine::PositionToVec4(vec3 v) {
	return {v.x, v.y, v.z, 1};
}

static int printedCharactersCount = 0;
void Migine::ContinousPrint(const std::string s) {
	stringstream ss;
	ss << s;
	printedCharactersCount += ss.str().size();
	cout << ss.str();
}

void Migine::ContinousPrintLineReset() {
	for (int i = 0; i < printedCharactersCount; i++) {
		cout << "\b";
	}
	printedCharactersCount = 0;
}
