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
    bool is_zero_aprox(glm::vec3 vec);
    bool is_equal_aprox(float val0, float val1);
    float manhatten_distance(glm::vec3 p0, glm::vec3 p1);
    glm::vec3 lerp(glm::vec3 v0, glm::vec3 v1, float t);
    glm::vec3 mid_point(glm::vec3 v0, glm::vec3 v1);
    void set_name(Game_object* obj, std::string name);
    glm::vec3 project_point_onto_axis(glm::vec3 p, glm::vec3 seg_p0, glm::vec3 seg_p1);

    // credits for next 3 functions https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.hpp and quaternion_utils.cpp in the same folder
    glm::quat rotation_between_vectors(glm::vec3 start, glm::vec3 dest);
    glm::quat look_at(glm::vec3 direction, glm::vec3 desired_up);
    glm::quat rotate_towards(glm::quat q1, glm::quat q2, float maxAngle);
    glm::mat3 get_skew_symmetric(glm::vec3 vec);
    bool is_point_on_axis(glm::vec3 d0, glm::vec3 d1, glm::vec3 p);
    bool is_point_on_axis2(glm::vec3 d0, glm::vec3 d1, glm::vec3 p);
    glm::vec3 copy_sing_element_wise(glm::vec3 mag, glm::vec3 sign);
    bool is_finite(glm::vec3 v);
    bool is_finite(glm::quat q);
    glm::quat change_rotation(glm::vec3 old_direction, glm::vec3 desired_direction, glm::vec3 old_up, glm::vec3 desired_up);

    template <class Randome_generator>
    glm::vec3 generate_point_on_sphere(Randome_generator rand) {
        glm::vec3 p{0};
        do {
            p = {rand() - 0.5, rand() -0.5, rand() -0.5};
        } while (is_zero_aprox(p));
        p = glm::normalize(p);
        return p;
    }

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
