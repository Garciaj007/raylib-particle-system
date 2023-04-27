#pragma once

#include "../common.hpp"

class IParticleDrawer
{
public:
	struct Data
	{
		Vector2 position;
		Vector2 size;
		Color color;
	};
	virtual void Draw(const Data& data) = 0;
	virtual ~IParticleDrawer() = default;
};

class PixelParticleDrawer : public IParticleDrawer
{
public:
	void Draw(const Data& data)
	{
		DrawPixelV(data.position, data.color);
	}
};

class CircleParticleDrawer : public IParticleDrawer
{
public:
	void Draw(const Data& data)
	{
		DrawCircleV(data.position, data.size.x, data.color);
	}
};

class EllipseParticleDrawer : public IParticleDrawer
{
public:
	void Draw(const Data& data)
	{
		DrawEllipse((int)data.position.x, (int)data.position.y, data.size.x, data.size.y, data.color);
	}
};

class RingParticleDrawer : public IParticleDrawer
{
	float startAngle;
	float endAngle;
	int segments;

public:
	RingParticleDrawer(int segments = 3, float startAngle = 0, float endAngle = 360.f) : startAngle(startAngle), endAngle(endAngle), segments(segments)
	{
	}

	void Draw(const Data& data)
	{
		DrawRing(data.position, data.size.x, data.size.y, startAngle, endAngle, segments, data.color);
	}
};

class RectParticleDrawer : public IParticleDrawer
{
public:
	void Draw(const Data& data)
	{
		DrawRectangleV(data.position, data.size, data.color);
	}
};

class RectGradientParticleDrawer : public IParticleDrawer
{
	Color other;
	bool order;
	bool isHorizontal;

public:
	RectGradientParticleDrawer(Color other, bool isHorizontal = true, bool order = true) : other(other), isHorizontal(isHorizontal), order(order) {}

	void Draw(const Data& data)
	{
		if (isHorizontal)
		{
			DrawRectangleGradientH((int)data.position.x, (int)data.position.y, (int)data.size.x, (int)data.size.y, order ? data.color : other, order ? other : data.color);
		}
		else
		{
			DrawRectangleGradientV((int)data.position.x, (int)data.position.y, (int)data.size.x, (int)data.size.y, order ? data.color : other, order ? other : data.color);
		}
	}
};

class RoundedRectParticleDrawer : public IParticleDrawer
{
	float roundness;
	int segments;

public:
	RoundedRectParticleDrawer(float roundness, int segments = 3) : roundness(roundness), segments(segments) {}

	void Draw(const Data& data)
	{
		DrawRectangleRounded({ data.position.x, data.position.y, data.size.x, data.size.y }, roundness, segments, data.color);
	}
};