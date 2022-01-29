#include "Has_id.h"


namespace migine {
	// pt paralelism, fa un array<size_t, THREAD_NO> counters = { 0 ... THREAD_NO -1}
	size_t Has_id::counter = 0;

	Has_id::Has_id() : id(counter) {
		counter++;
	}

	bool operator==(Has_id lhs, Has_id rhs) {
		return lhs.id == rhs.id;
	}

	bool operator!=(Has_id lhs, Has_id rhs) {
		return !(lhs == rhs);
	}

	bool operator<(Has_id lhs, Has_id rhs) {
		return lhs.id < rhs.id;
	}

	bool operator>(Has_id lhs, Has_id rhs) {
		return rhs < lhs;
	}

	bool operator<=(Has_id lhs, Has_id rhs) {
		return !(lhs > rhs);
	}

	bool operator>=(Has_id lhs, Has_id rhs) {
		return !(lhs < rhs);
	}
}