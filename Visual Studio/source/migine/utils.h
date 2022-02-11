#pragma once

#include <Core/Engine.h>
#include <string>
#include <tuple>
#include <functional>
#include <type_traits>

#include <migine/constants.h>
#include <migine/game_objects/Game_object.h>

namespace migine {
    glm::quat euler_angles_to_quat(glm::vec3 euler_angles);
    glm::quat euler_angles_deg_to_quat(glm::vec3 euler_angles);
    glm::vec4 position_to_vec4(glm::vec3 v);
    void continous_print(const std::string s);
    void continous_print_line_reset();
    void inverse_inplace(glm::mat3& mat);
    //glm::quat quat_add_vec3(glm::quat q, glm::vec3 v);
    float get_elapsed_time();
    bool is_zero_aprox(float val); // credits for ideea: godot
    glm::vec3 lerp(glm::vec3 v0, glm::vec3 v1, float t);
    glm::vec3 mid_point(glm::vec3 v0, glm::vec3 v1);
    void set_name(Game_object* obj, std::string name);

    inline void _transform_inertia_tensor(glm::mat3& iitWorld, const glm::mat3& iitBody, const glm::mat4& rotmat);

    // credits to answer from https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
    namespace {
        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v) {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct Hash_value_impl {
            static void apply(size_t& seed, Tuple const& tuple) {
                Hash_value_impl<Tuple, Index - 1>::apply(seed, tuple);
                hash_combine(seed, std::get<Index>(tuple));
            }
        };

        template <class Tuple>
        struct Hash_value_impl<Tuple, 0> {
            static void apply(size_t& seed, Tuple const& tuple) {
                hash_combine(seed, std::get<0>(tuple));
            }
        };
    }

    template <typename ... TT>
    struct Tuple_hasher {
        size_t operator()(std::tuple<TT...> const& tt) const {
            size_t seed = 0;
            Hash_value_impl<std::tuple<TT...> >::apply(seed, tt);
            return seed;
        }
    };
}

//namespace std {
//   template<> struct hash<migine::Has_id> {
//        size_t operator()(const migine::Has_id& x) const {
//            //size_t operator()(migine::Has_id x) const {
//            return std::hash<size_t>()(x.id);
//        }
//    };
//    template<> struct hash<migine::Collider_base> {
//        size_t operator()(const migine::Collider_base& x) const {
//            //return std::hash<migine::Has_id>()(x);
//            return std::hash<size_t>()(x.id);
//        }
//    };
//}