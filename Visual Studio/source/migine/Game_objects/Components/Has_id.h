#pragma once

#include <functional>

namespace migine {
	class Has_id {
		friend bool operator==(Has_id lhs, Has_id rhs);
		friend bool operator!=(Has_id lhs, Has_id rhs);
		friend bool operator<(Has_id lhs, Has_id rhs);
		friend bool operator>(Has_id lhs, Has_id rhs);
		friend bool operator<=(Has_id lhs, Has_id rhs);
		friend bool operator>=(Has_id lhs, Has_id rhs);

	public:
		Has_id();

		const size_t id;
	private:
		static size_t counter;
	};
}