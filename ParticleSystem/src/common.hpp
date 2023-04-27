#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cassert>
#include <atomic>
#include <random>
#include <optional>
#include <execution>
#include <thread>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <glad/glad.h>

#define ENTT_USE_ATOMIC
#include <entt/entt.hpp>

const char* SolutionName = "Module 12 - Programming Patterns & ECS";

extern const char* PrependSolutionName(const char* name)
{
	return TextFormat("%s | %s", SolutionName, name);
}

// Alias for std::shared_ptr
template<typename T>
using Ref = std::shared_ptr<T>;

// Alias for std::make_shared
template<typename T, typename... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

// Alias for std::unique_ptr
template<typename T>
using Scoped = std::unique_ptr<T>;

// Alias for std::make_unique
template<typename T, typename... Args>
constexpr Scoped<T> MakeScoped(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

// Generates Temporary IDs of derived type
template<typename T>
class TUID
{
	std::atomic<T> id = 0;

public:
	TUID() = default;
	T GetNext() { return id++; }
};

// Utility Functions

float Clamp01(float value)
{
	return Clamp(value, 0.f, 1.f);
}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

float Random()
{
	return dis(gen);
}

float Random(float min, float max)
{
	return Remap(Random(), 0.0f, 1.0f, min, max);
}

float RandomSpread(float spread)
{
	return Random(-spread, spread);
}

Vector2 RandomVector2(float min, float max)
{
	return Vector2 { Random(Random(min, max), Random(min, max)), Random(Random(min, max), Random(min, max)) };
}

Vector2 RandomVector2(Vector2 index1, Vector2 index2)
{
	return Vector2 {
	Random(Random(index1.x, index1.y), Random(index1.x, index1.y)),
	Random(Random(index2.x, index2.y), Random(index2.x, index2.y))
	};
}

constexpr Vector2 Up = Vector2 { 0.0, 1.0 };
constexpr Vector2 Right = Vector2 { 1.0, 0.0 };
constexpr Vector2 Down = Vector2 { 0.0, -1.0 };
constexpr Vector2 Left = Vector2 { -1.0, 0.0 };
constexpr Vector2 Zero = Vector2 {};

template <typename... Args>
constexpr void DebugLog(const char* text, Args&& ... args)
{
	TraceLog(LOG_DEBUG, std::forward<const char*>(text), std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void InfoLog(const char* text, Args&& ... args)
{
	TraceLog(LOG_INFO, std::forward<const char*>(text), std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void WarnLog(const char* text, Args&& ... args)
{
	TraceLog(LOG_WARNING, std::forward<const char*>(text), std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void ErrorLog(const char* text, Args&& ... args)
{
	TraceLog(LOG_ERROR, std::forward<const char*>(text), std::forward<Args>(args)...);
}

const char* Stringify(Vector2 v)
{
	return TextFormat("Vector2 - [%.1f, %.1f]", v.x, v.y);
}

const char* Stringify(Vector3 v)
{
	return TextFormat("Vector2 - [%.1f, %.1f, %.1f]", v.x, v.y, v.z);
}

const char* Stringify(Color color)
{
	return TextFormat("Color - [%d, %d, %d, %d]", color.r, color.g, color.b, color.a);
}

const char* FormatBytes(std::size_t bytes)
{
	const char* suffixes [] = { "B", "KB", "MB", "GB", "TB" };
	int suffixIndex = 0;
	double size = static_cast<double>(bytes);
	while (size >= 1024 && suffixIndex < 4)
	{
		size /= 1024;
		suffixIndex++;
	}
	return TextFormat("%.1f %s", size, suffixes [suffixIndex]);
}

class IRandomize
{
public:
	virtual void Randomize() = 0;
};