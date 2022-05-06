#include "utils.h"

#include <migine/define.h>
#include <migine/constants.h>

#include <vector>
#include <iostream>
#include <sstream>

using glm::quat;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using glm::length2;
using glm::angleAxis;
using glm::dot;

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
		assert(false); // am descoperit ca matricile  pe care se bazeaza aceste hardcodari sunt transpuse fata de matricile din glm
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

	//glm::quat quat_add_vec3(glm::quat q, glm::vec3 v) {
	//	quat q2 = {0, v.x, v.y, v.z};
	//	q2 *= q;
	//	q2 *= 0.5;
	//	return q2;
	//}

	float get_elapsed_time() {
		return glfwGetTime();
	}

	bool is_zero_aprox(float val) {
		return fabs(val) < k_float_epsilon;
	}

	bool is_equal_aprox(float val0, float val1) {
		return is_zero_aprox(val0 - val1);
	}

	vec3 lerp(vec3 v0, vec3 v1, float t) {
		return v0 + (v1 - v0) * t;
	}

	glm::vec3 mid_point(glm::vec3 v0, glm::vec3 v1) {
		return (v0 + v1) / 2.0f;
	}

	void set_name(Game_object* obj, std::string name) {
#ifdef DEBUGGING
		obj->name = name;
#endif // DEBUGGING
	}

	// credits to https://gamedev.stackexchange.com/questions/72528/how-can-i-project-a-3d-point-onto-a-3d-line
	vec3 project_point_onto_axis(vec3 p, vec3 seg_p0, vec3 seg_p1) {
		vec3 seg = seg_p1 - seg_p0;
		// fie seg_p0 = p0, seg_p1 = p1, q = p
		vec3 p0q = p - seg_p0;
		return seg_p0 + dot(p0q, seg) / dot(seg, seg) * seg;
	}

	void _transform_inertia_tensor(mat3& iitWorld, const mat3& iitBody, const mat4& rotmat) {
		assert(false); // am descoperit ca matricile  pe care se bazeaza aceste hardcodari sunt transpuse fata de matricile din glm
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

	// Returns a quaternion such that q*start = dest
	quat rotation_between_vectors(vec3 start, vec3 dest) {
		start = normalize(start);
		dest = normalize(dest);

		float cos_theta = dot(start, dest);
		vec3 rotation_axis;

		if (cos_theta < -1 + 0.001f) {
			// special case when vectors in opposite directions :
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			// This implementation favors a rotation around the Up axis,
			// since it's often what you want to do.
			rotation_axis = cross(vec3(0.0f, 0.0f, 1.0f), start);
			if (length2(rotation_axis) < 0.01) // bad luck, they were parallel, try again!
				rotation_axis = cross(vec3(1.0f, 0.0f, 0.0f), start);

			rotation_axis = normalize(rotation_axis);
			return angleAxis(glm::radians(180.0f), rotation_axis);
		}

		// Implementation from Stan Melax's Game Programming Gems 1 article
		rotation_axis = cross(start, dest);

		float s = sqrt((1 + cos_theta) * 2);
		float invs = 1 / s;

		return quat(
			s * 0.5f,
			rotation_axis.x * invs,
			rotation_axis.y * invs,
			rotation_axis.z * invs
		);


	}

	// Returns a quaternion that will make your object looking towards 'direction'.
	// Similar to RotationBetweenVectors, but also controls the vertical orientation.
	// This assumes that at rest, the object faces +Z.
	// Beware, the first parameter is a direction, not the target point !
	quat look_at(vec3 direction, vec3 desired_up) {

		if (length2(direction) < 0.0001f)
			return quat();

		// Recompute desiredUp so that it's perpendicular to the direction
		// You can skip that part if you really want to force desiredUp
		vec3 right = cross(direction, desired_up);
		desired_up = cross(right, direction);

		// Find the rotation between the front of the object (that we assume towards +Z,
		// but this depends on your model) and the desired direction
		quat rot1 = rotation_between_vectors(vec3(0.0f, 0.0f, 1.0f), direction);
		// Because of the 1rst rotation, the up is probably completely screwed up. 
		// Find the rotation between the "up" of the rotated object, and the desired up
		vec3 new_up = rot1 * vec3(0.0f, 1.0f, 0.0f);
		quat rot2 = rotation_between_vectors(new_up, desired_up);

		// Apply them
		return rot2 * rot1; // remember, in reverse order.
	}



	// Like SLERP, but forbids rotation greater than maxAngle (in radians)
	// In conjunction to LookAt, can make your characters 
	quat rotate_towards(quat q1, quat q2, float max_angle) {

		if (is_zero_aprox(max_angle)) {
			// No rotation allowed. Prevent dividing by 0 later.
			return q1;
		}

		float cos_theta = dot(q1, q2);

		// q1 and q2 are already equal.
		// Force q2 just to be sure
		if (is_zero_aprox(1-cos_theta)) {
			return q2;
		}

		// Avoid taking the long path around the sphere
		if (cos_theta < 0) {
			q1 = q1 * -1.0f;
			cos_theta *= -1.0f;
		}

		float angle = acos(cos_theta);

		// If there is only a 2° difference, and we are allowed 5°,
		// then we arrived.
		if (angle < max_angle) {
			return q2;
		}

		// This is just like slerp(), but with a custom t
		float t = max_angle / angle;
		angle = max_angle;

		quat res = (sin((1.0f - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
		res = normalize(res);
		return res;

	}

	quat change_rotation(vec3 old_direction, glm::vec3 desired_direction, glm::vec3 old_up, vec3 desired_up) {
		if (is_zero_aprox(desired_direction.length())) {
			return quat();
		}

		// Recompute desiredUp so that it's perpendicular to the direction
		// You can skip that part if you really want to force desiredUp
		vec3 right = cross(desired_direction, desired_up);
		desired_up = cross(right, desired_direction);

		// Find the rotation between old_direction and desired_direction
		quat rot1 = rotation_between_vectors(old_direction, desired_direction);
		// Because of the 1rst rotation, the old_up is probably completely screwed up. 
		// Find the rotation between the "up" of the rotated object, and the desired up
		vec3 new_up = rot1 * old_up;
		quat rot2 = rotation_between_vectors(new_up, desired_up);

		// Apply them
		return rot2 * rot1; // remember, in reverse order.
	}
}