#include "utils.h"

#include <migine/constants.h>

#include <vector>
#include <iostream>
#include <sstream>

using glm::quat;
using glm::vec3;
using glm::vec4;
using std::cout;
using std::stringstream;
using std::string;
using std::vector;

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
			cout << "\b";
		}
		printed_characters_count = 0;
	}
}
