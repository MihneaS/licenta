#include "utils.h"

#include <migine/constants.h>

#include <vector>
#include <iostream>
#include <sstream>

using glm::quat;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

using std::stringstream;
using std::string;
using std::vector;
using std::cout;

namespace migine {
	quat euler_angles_to_quat(vec3 euler_angles) {
		quat rox = glm::rotate(quat(), euler_angles.x, {1,0,0});
		quat roy = glm::rotate(quat(), euler_angles.y, {0,1,0});
		quat roz = glm::rotate(quat(), euler_angles.z, {0,0,1});

		return rox * roy * roz;
	}

	quat euler_angles_deg_to_quat(vec3 euler_angles) {
		return euler_angles_to_quat(euler_angles * k_deg_to_rad);
	}

	vec4 position_to_vec4(vec3 v) {
		return {v.x, v.y, v.z, 1};
	}

	static int printed_characters_count = 0;
	void continous_print(const std::string s) {
		stringstream ss;
		ss << s;
		printed_characters_count += ss.str().size();
		cout << ss.str();
	}

	void continous_print_line_reset() {
		for (int i = 0; i < printed_characters_count; i++) {
			cout << "\b \b\b";
		}
		printed_characters_count = 0;
	}
	void inverse_inplace(glm::mat3& mat) {
		// Ian Millington game physics engine development page 173
		float t4 = mat[0][0] * mat[1][1];
		float t6 = mat[0][0] * mat[1][2];
		float t8 = mat[0][1] * mat[1][0];
		float t10 = mat[0][2] * mat[1][0];
		float t12 = mat[0][1] * mat[2][0];
		float t14 = mat[0][2] * mat[2][0];

		// Calcualte the determinant.
		float t16 = t4*mat[2][2] - t6*mat[2][1] - t8*mat[2][2] + t10*mat[2][1] + t12*mat[1][2] - t14*mat[1][1];
		float t17 = 1 / t16;

		mat[0][0] =  (mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1])*t17;
		mat[0][1] = -(mat[0][1]*mat[2][2] - mat[0][2]*mat[2][1])*t17;
		mat[0][2] =  (mat[0][1]*mat[1][2] - mat[0][2]*mat[1][1])*t17;
		mat[1][0] = -(mat[1][0]*mat[2][2] - mat[1][2]*mat[2][0])*t17;
		mat[1][1] =  (mat[0][0]*mat[2][2] - t14)*t17;
		mat[1][2] = -(t6 - t10)*t17;
		mat[2][0] =  (mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0])*t17;
		mat[2][1] = -(mat[0][0]*mat[2][1] - t12)*t17;
		mat[2][2] =  (t4 - t8)*t17;
	}

	glm::quat quat_add_vec3(glm::quat q, glm::vec3 v) {
		quat q2 = {0, v.x, v.y, v.z};
		q2 *= q;
		q2 *= 0.5;
		return q2;
	}

	float get_elapsed_time() {
		return glfwGetTime();
	}

	void _transform_inertia_tensor(mat3& iitWorld, const mat3& iitBody, const mat4& rotmat) {
		float t04 = rotmat[0][0] * iitBody[0][0] +
			rotmat[0][1] * iitBody[1][0] +
			rotmat[0][2] * iitBody[2][0];
		float t09 = rotmat[0][0] * iitBody[0][1] +
			rotmat[0][1] * iitBody[1][1] +
			rotmat[0][2] * iitBody[2][1];
		float t14 = rotmat[0][0] * iitBody[0][2] +
			rotmat[0][1] * iitBody[1][2] +
			rotmat[0][2] * iitBody[2][2];
		float t28 = rotmat[1][0] * iitBody[0][0] +
			rotmat[1][1] * iitBody[1][0] +
			rotmat[1][2] * iitBody[2][0];
		float t33 = rotmat[1][0] * iitBody[0][1] +
			rotmat[1][1] * iitBody[1][1] +
			rotmat[1][2] * iitBody[2][1];
		float t38 = rotmat[1][0] * iitBody[0][2] +
			rotmat[1][1] * iitBody[1][2] +
			rotmat[1][2] * iitBody[2][2];
		float t52 = rotmat[2][0] * iitBody[0][0] +
			rotmat[2][1] * iitBody[1][0] +
			rotmat[2][2] * iitBody[2][0];
		float t57 = rotmat[2][0] * iitBody[0][1] +
			rotmat[2][1] * iitBody[1][1] +
			rotmat[2][2] * iitBody[2][1];
		float t62 = rotmat[2][0] * iitBody[0][2] +
			rotmat[2][1] * iitBody[1][2] +
			rotmat[2][2] * iitBody[2][2];
		iitWorld[0][0] = t04 * rotmat[0][0] +
			t09 * rotmat[0][1] +
			t14 * rotmat[0][2];
		iitWorld[0][1] = t04 * rotmat[1][0] +
			t09 * rotmat[1][1] +
			t14 * rotmat[1][2];
		iitWorld[0][2] = t04 * rotmat[2][0] +
			t09 * rotmat[2][1] +
			t14 * rotmat[2][2];
		iitWorld[1][0] = t28 * rotmat[0][0] +
			t33 * rotmat[0][1] +
			t38 * rotmat[0][2];
		iitWorld[1][1] = t28 * rotmat[1][0] +
			t33 * rotmat[1][1] +
			t38 * rotmat[1][2];
		iitWorld[1][2] = t28 * rotmat[2][0] +
			t33 * rotmat[2][1] +
			t38 * rotmat[2][2];
		iitWorld[2][0] = t52 * rotmat[0][0] +
			t57 * rotmat[0][1] +
			t62 * rotmat[0][2];
		iitWorld[2][1] = t52 * rotmat[1][0] +
			t57 * rotmat[1][1] +
			t62 * rotmat[1][2];
		iitWorld[2][2] = t52 * rotmat[2][0] +
			t57 * rotmat[2][1] +
			t62 * rotmat[2][2];
	}
}