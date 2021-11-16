#include "utils.h"

#include <Migine/constants.h>

using namespace Migine;
using glm::quat;
using glm::vec3;

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
