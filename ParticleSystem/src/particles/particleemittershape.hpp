#pragma once

#include "../common.hpp"

class IEmitterShape
{
public:
	virtual Vector2 GetStartPos() = 0;
	virtual Vector2 GetStartVel() = 0;
	virtual ~IEmitterShape() = default;
};

class LineEmitterShape : public IEmitterShape
{
	float width;

public:
	LineEmitterShape(float width) : width(width) {}
	Vector2 GetStartPos() override { return { RandomSpread(width), 0.0f }; }
	Vector2 GetStartVel() override { return Zero; }
};

class BoxEmitterShape : public IEmitterShape
{
	float width;
	float height;
	bool isOutline;

public:
	BoxEmitterShape(float width, float height, bool isOutline = false) :
		width(width),
		height(height),
		isOutline(isOutline)
	{
	}

	Vector2 GetStartPos() override
	{
		if (isOutline)
		{
			if (GetRandomValue(0, 1) == 1)
			{
				return { RandomSpread(width), 0.0f };
			}
			else
			{
				return { 0.0f, RandomSpread(height) };
			}
		}
		else
		{
			return RandomVector2({ -width, width }, { -height, height });
		}
	}

	Vector2 GetStartVel() override { return Zero; }
};

class CircleEmitterShape : public IEmitterShape
{
	float radius;
	bool isOutline;

public:
	CircleEmitterShape(float radius, bool isOutline = false) : radius(radius), isOutline(isOutline) {}

	Vector2 GetStartPos() override
	{
		float angle = Random(0.f, 2 * PI);
		if (isOutline)
		{
			return { radius * cosf(angle), radius * sinf(angle) };
		}
		else
		{
			float dist = Random(0.f, radius);
			return { dist * cosf(angle), radius * sinf(angle) };
		}
	}

	Vector2 GetStartVel() override { return Zero; }
};

class ConeEmitterShape : public IEmitterShape
{
	//	   
	//    /
	//   /		   * - intensity
	//	/	   *
	// /	*
	// \ ϴ	- angle (How wide is the cone)
	//  \___________
	// ^ - baseWidth

	float baseWidth;
	float angle;
	Vector2 minMaxIntensity;

public:
	ConeEmitterShape(float baseWidth, float angle, Vector2 minMaxIntensity) : baseWidth(baseWidth), angle(angle), minMaxIntensity(minMaxIntensity) {}
	ConeEmitterShape(float baseWidth, float angle, float intensity) : ConeEmitterShape(baseWidth, angle, { 0.0f, intensity }) {}

	Vector2 GetStartPos() override
	{
		return Vector2{ RandomSpread(baseWidth) };
	}

	Vector2 GetStartVel() override
	{
		float angle = RandomSpread(this->angle * DEG2RAD);
		return Vector2Rotate({ 0.0f, Random(minMaxIntensity.x, minMaxIntensity.y) }, angle);
	}
};