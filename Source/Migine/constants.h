#pragma once

#include <Core/Engine.h>



constexpr glm::vec3 kDefaultColor = { 1,0,1 };
constexpr int kDefaultLineWidth = 5;
constexpr glm::vec3 kAABBColor = { 0.1,0.6,0.1 };
constexpr float kDegToRad = glm::pi<float>() / 180.0f;
constexpr int kBoxMaterialShiness = 30;
constexpr float kBoxMaterialKd = 0.5;
constexpr float kBoxMaterialKs = 0.5;
constexpr glm::vec3 kBoxColor = { 0.0f / 256.0f, 153.0f / 256.0f, 255.0f };
constexpr glm::vec3 kVec3Zero = {0,0,0};
constexpr int kNoIndex = -1;