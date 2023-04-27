#pragma once

#include "../common.hpp"
#include "../interpolator.hpp"
#include "../gradient.hpp"

#include "components.hpp"

namespace ecs
{
	enum class ps_entity : std::uint64_t {};
	using ps_registry = entt::basic_registry<ps_entity>;

	enum class DrawType
	{
		PIXEL, POINT, CIRCLE, ELLIPSE, RING, RECT, RECT_GRADIENT, ROUNDED_RECT, BATCH_CIRCLE
	};

	struct SharedParticleData
	{
		using Vector2Interpolator = advanced::Vector2Interpolator;
		using FloatInterpolator = advanced::FloatInterpolator;
		using Gradient = advanced::Gradient;

		Ref<FloatInterpolator> randomLifetime;
		float lifetime;

		Ref<Vector2Interpolator> randomVelocity;
		Ref<VelocityOverLifetimeComponent> velocityOverLifetime;
		Vector2 velocity;

		Ref<Vector2Interpolator> randomAcceleration;
		std::optional<Vector2> acceleration;

		Ref<FloatInterpolator> randomRotation;
		Ref<RotationOverLifetimeComponent> rotationOverLifetime;
		std::optional<float> rotation;

		Ref<FloatInterpolator> randomAngularVelocity;
		Ref<AngularVelocityOverLifetimeComponent> angularVelocityOverLifetime;
		std::optional<float> angularVelocity;

		Ref<FloatInterpolator> randomAngularAcceleration;
		std::optional<float> angularAcceleration;

		Ref<Vector2Interpolator> randomSize;
		Ref<SizeOverLifetimeComponent> sizeOverLifetime;
		Vector2 size;

		Ref<Gradient> randomColor;
		Ref<ColorOverLifetimeComponent> colorOverLifetime;
		Color color;

		DrawType drawType;
		RingDrawComponent ringDrawCompProto;
		RectGradientDrawComponent rectGradDrawCompProto;
		RoundedRectDrawComponent roundedRectDrawCompProto;

		Ref<IEmitterShape> emitterShape;

		SharedParticleData() :
			lifetime(1.f),
			randomAcceleration(nullptr),
			randomVelocity(nullptr),
			acceleration(std::nullopt),
			velocity({ 0.f, 0.f }),
			randomRotation(nullptr),
			rotationOverLifetime(nullptr),
			rotation(std::nullopt),
			randomAngularAcceleration(nullptr),
			randomAngularVelocity(nullptr),
			angularVelocityOverLifetime(nullptr),
			angularVelocity(std::nullopt),
			angularAcceleration(std::nullopt),
			randomColor(nullptr),
			randomSize(nullptr),
			color(PINK),
			size({ 10.0f }),
			colorOverLifetime(nullptr),
			sizeOverLifetime(nullptr),
			drawType(DrawType::PIXEL),
			ringDrawCompProto({}),
			rectGradDrawCompProto({}),
			roundedRectDrawCompProto({}),
			emitterShape(nullptr)
		{
		}

		float GetLifetime() 
		{ 
			return randomLifetime ? randomLifetime->Evaluate(random) : lifetime; 
		}

		Vector2 GetVelocity() 
		{ 
			return randomVelocity ? randomVelocity->Evaluate(random) : velocity; 
		}
		
		std::optional<float> GetRotation() 
		{ 
			return randomRotation ? randomRotation->Evaluate(random) : rotation; 
		}
		
		std::optional<Vector2> GetAcceleration() 
		{ 
			return randomAcceleration ? randomAcceleration->Evaluate(random) : acceleration; 
		}

		std::optional<float> GetAngularVelocity() 
		{ 
			return randomAngularVelocity ? randomAngularVelocity->Evaluate(random) : angularVelocity; 
		}

		std::optional<float> GetAngularAcceleration() 
		{ 
			return randomAngularAcceleration ? randomAngularAcceleration->Evaluate(random) : angularAcceleration; 
		}
		
		Vector2 GetSize() 
		{ 
			return randomSize ? randomSize->Evaluate(random) : size; 
		}
		
		Color GetColor() 
		{ 
			return randomColor ? randomColor->Evaluate(random) : color; 
		}

		inline void Randomize() 
		{
			random = Random();
		}

	private:
		float random;
	};
}