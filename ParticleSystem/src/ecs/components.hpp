#pragma once

#include "../common.hpp"

namespace ecs
{
	template<typename T>
	struct InterpolatorComponent
	{
		struct KeyValue
		{
			float key;
			T value;
		};

		KeyValue keyValues[8];
		int length;
		T interpolated;

		InterpolatorComponent(const std::initializer_list<KeyValue>& kvs)
		{
			assert(kvs.size() > 2 && kvs.size() <= 8);

			size_t i = 0;
			for (const auto& kv : kvs)
			{
				keyValues[i++] = kv;
			}
			length = (int)i;
		}
	};

	struct ColorOverLifetimeComponent : public InterpolatorComponent<Color>
	{
		using InterpolatorComponent<Color>::InterpolatorComponent;
	};

	struct RotationOverLifetimeComponent : public InterpolatorComponent<float>
	{
		using InterpolatorComponent<float>::InterpolatorComponent;
	};

	struct SizeOverLifetimeComponent : public InterpolatorComponent<Vector2>
	{
		using InterpolatorComponent<Vector2>::InterpolatorComponent;
	};

	struct VelocityOverLifetimeComponent : public InterpolatorComponent<Vector2>
	{
		using InterpolatorComponent<Vector2>::InterpolatorComponent;
	};

	struct AngularVelocityOverLifetimeComponent : public InterpolatorComponent<float>
	{
		using InterpolatorComponent<float>::InterpolatorComponent;
	};

	struct DestroyEntityComponent {};

	struct PixelDrawComponent {};

	struct CircleDrawComponent {};

	struct PointBatchDrawComponent {};

	struct CircleBatchDrawComponent {};

	struct EllipseDrawComponent {};

	struct RectDrawComponent {};

	struct RingDrawComponent
	{
		float startAngle;
		float endAngle;
		int segements;
	};

	struct RectGradientDrawComponent
	{
		Color other;
		bool order;
		bool isHorizontal;
	};

	struct RoundedRectDrawComponent
	{
		float roundness;
		int segments;
	};

	struct LifetimeComponent
	{
		float lifetime;
		float spawntime;
		float t;
	};

	struct PositionComponent
	{
		Vector2 position;
	};

	struct VelocityComponent
	{
		Vector2 velocity;
	};

	struct AccelerationComponent
	{
		Vector2 acceleration;
	};

	struct RotationComponent
	{
		float rotation;
	};

	struct AngularVelocityComponent
	{
		float angularVelocity;
	};

	struct AngularAccelerationComponent
	{
		float angularAcceleration;
	};

	struct SizeComponent
	{
		Vector2 size;
	};

	struct ColorComponent
	{
		Color color;
	};
}