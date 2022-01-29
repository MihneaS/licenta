#include "Collider_base.h"


namespace std {
	template<> struct hash<migine::Has_id> {
		hash() = default;
		size_t operator()(const migine::Has_id& x) const {
			//size_t operator()(migine::Has_id x) const {
			return std::hash<size_t>()(x.id);
		}
	};

	template<> struct hash<migine::Collider_base> {
		hash() = default;
		size_t operator()(const migine::Collider_base& x) const {
			return std::hash<migine::Has_id>()(x);
		}
	};
}