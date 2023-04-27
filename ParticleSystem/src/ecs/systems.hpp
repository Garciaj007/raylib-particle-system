#pragma once

#include "../common.hpp"
#include "../instrumentation.hpp"

#include "common.hpp"
#include "components.hpp"

namespace ecs
{
	static const auto EXECUTION_POLICY = std::execution::par_unseq;

	// Destroy All Entities Marked for Destruction
	void DestroyEntitySystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		auto destroyEntityView = reg.view<DestroyEntityComponent>();
		reg.destroy(destroyEntityView.begin(), destroyEntityView.end());
	}

	_NODISCARD std::thread LifetimeUpdateSystem(ps_registry& reg, float time)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg, float time)
			{
				PROFILE_THREAD("LifetimeUpdateThread");

				// Update all Lifecycle Components
				reg.view<LifetimeComponent>()
					.each([&registry = reg, time](const auto entity, LifetimeComponent& lifetimeComponent)
						{
							float t = time - lifetimeComponent.spawntime;

							if (t > lifetimeComponent.lifetime)
							{

								registry.emplace<DestroyEntityComponent>(entity);
							}

							lifetimeComponent.t = t / lifetimeComponent.lifetime;
						});
			}, std::ref(reg), time);

		return std::move(thread);
	}

	_NODISCARD std::thread KinematicUpdateSystem(ps_registry& reg, float dt)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg, float dt)
			{
				PROFILE_THREAD("KinematicUpdateThread");

				auto view = reg.view<PositionComponent, VelocityComponent, const AccelerationComponent>();
				std::for_each(EXECUTION_POLICY, view.begin(), view.end(), [&view, dt](auto entity)
					{
						auto [positionComponent, velocityComponent, accelerationComponent] = view.get<PositionComponent, VelocityComponent, const AccelerationComponent>(entity);

						velocityComponent.velocity.x += accelerationComponent.acceleration.x * dt;
						velocityComponent.velocity.y += accelerationComponent.acceleration.y * dt;

						const float half = 0.5f;

						positionComponent.position.x += velocityComponent.velocity.x * dt + half * accelerationComponent.acceleration.x * dt * dt;
						positionComponent.position.y += velocityComponent.velocity.y * dt + half * accelerationComponent.acceleration.y * dt * dt;
					});
			}, std::ref(reg), dt);

		return std::move(thread);
	}

	_NODISCARD std::thread PositionUpdateSystem(ps_registry& reg, float dt)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg, float dt)
			{
				PROFILE_THREAD("PositionUpdateThread");

				auto positionVelocityView = reg.view<PositionComponent, const VelocityComponent>(entt::exclude<AccelerationComponent>);
				std::for_each(EXECUTION_POLICY, positionVelocityView.begin(), positionVelocityView.end(), [&positionVelocityView, dt](auto entity)
					{
						auto [positionComponent, velocityComponent] = positionVelocityView.get<PositionComponent, const VelocityComponent>(entity);
						positionComponent.position.x += velocityComponent.velocity.x * dt;
						positionComponent.position.y += velocityComponent.velocity.y * dt;
					});
			}, std::ref(reg), dt);

		return std::move(thread);
	}

	_NODISCARD std::thread ApplyInterpolatedVelocitySystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("ApplyInterpolatedVelocityThread");

				auto view = reg.view<VelocityComponent, const VelocityOverLifetimeComponent>();
				std::for_each(EXECUTION_POLICY, view.begin(), view.end(), [&view](auto entity)
					{
						auto [velocityComponent, velocityOverLifetimeComponent] = view.get<VelocityComponent, const VelocityOverLifetimeComponent>(entity);
						velocityComponent.velocity = velocityOverLifetimeComponent.interpolated;
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread ApplyInterpolatedSizeSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("ApplyInterpolatedSizeThread");

				auto view = reg.view<SizeComponent, const SizeOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, view.begin(), view.end(), [&view](auto entity)
					{
						auto [sizeComponent, sizeOverLifetimeComponent] = view.get<SizeComponent, const SizeOverLifetimeComponent>(entity);
						sizeComponent.size = sizeOverLifetimeComponent.interpolated;
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread ApplyInterpolatedColorSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("ApplyInterpolatedColorThread");

				auto colorView = reg.view<ColorComponent, const ColorOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, colorView.begin(), colorView.end(), [&colorView](auto entity)
					{
						auto [colorComponent, colorOverLifetimeComponent] = colorView.get<ColorComponent, const ColorOverLifetimeComponent>(entity);
						colorComponent.color = colorOverLifetimeComponent.interpolated;
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	void Interpolate(InterpolatorComponent<float>& interpolator, const LifetimeComponent& lifetime)
	{
		int index1 = 0;
		int index2 = 0;

		for (int i = 0; i < interpolator.length - 1; i++)
		{
			if (lifetime.t >= interpolator.keyValues[i].key && lifetime.t < interpolator.keyValues[i + 1].key)
			{
				index1 = i;
				index2 = i + 1;
				break;
			}
		}

		const auto& a = interpolator.keyValues[index1];
		const auto& b = interpolator.keyValues[index2];
		const float t = (lifetime.t - a.key) / (b.key - a.key);

		interpolator.interpolated = Lerp(a.value, b.value, t);
	}

	void Interpolate(InterpolatorComponent<Vector2>& interpolator, const LifetimeComponent& lifetime)
	{
		int index1 = 0;
		int index2 = 0;

		for (int i = 0; i < interpolator.length - 1; i++)
		{
			if (lifetime.t >= interpolator.keyValues[i].key && lifetime.t < interpolator.keyValues[i + 1].key)
			{
				index1 = i;
				index2 = i + 1;
				break;
			}
		}

		const auto& a = interpolator.keyValues[index1];
		const auto& b = interpolator.keyValues[index2];
		const float t = (lifetime.t - a.key) / (b.key - a.key);

		interpolator.interpolated = Vector2Lerp(a.value, b.value, t);
	}

	void Interpolate(InterpolatorComponent<Color>& interpolator, const LifetimeComponent& lifetime)
	{
		int index1 = 0;
		int index2 = 0;

		for (int i = 0; i < interpolator.length - 1; i++)
		{
			if (lifetime.t >= interpolator.keyValues[i].key && lifetime.t < interpolator.keyValues[i + 1].key)
			{
				index1 = i;
				index2 = i + 1;
				break;
			}
		}

		const auto& a = interpolator.keyValues[index1];
		const auto& b = interpolator.keyValues[index2];
		const float t = (lifetime.t - a.key) / (b.key - a.key);

		Vector3 hsv1 = ColorToHSV(a.value);
		Vector3 hsv2 = ColorToHSV(b.value);
		Vector3 hsv = Vector3Lerp(hsv1, hsv2, t);
		interpolator.interpolated = ColorAlpha(ColorFromHSV(hsv.x, hsv.y, hsv.z), Lerp(a.value.a / 255.0f, b.value.a / 255.0f, t));
	}

	_NODISCARD std::thread InterpolateColorSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("InterpolateColorThread");

				auto colorOverLifetimeView = reg.view<ColorOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, colorOverLifetimeView.begin(), colorOverLifetimeView.end(), [&colorOverLifetimeView](auto entity)
					{
						auto [colorOverLifeComponent, lifetimeComponent] = colorOverLifetimeView.get<ColorOverLifetimeComponent, const LifetimeComponent>(entity);
						Interpolate(colorOverLifeComponent, lifetimeComponent);
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread InterpolateRotationSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("InterpolateRotationThread");

				auto rotationOverLifetimeView = reg.view<RotationOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, rotationOverLifetimeView.begin(), rotationOverLifetimeView.end(), [&rotationOverLifetimeView](auto entity)
					{
						auto [rotOverLifeComponent, lifetimeComponent] = rotationOverLifetimeView.get<RotationOverLifetimeComponent, const LifetimeComponent>(entity);
						Interpolate(rotOverLifeComponent, lifetimeComponent);
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread InterpolateSizeSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("InterpolateSizeThread");

				auto sizeOverLifetimeView = reg.view<SizeOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, sizeOverLifetimeView.begin(), sizeOverLifetimeView.end(), [&sizeOverLifetimeView](auto entity)
					{
						auto [sizeOverLifeComponent, lifetimeComponent] = sizeOverLifetimeView.get<SizeOverLifetimeComponent, const LifetimeComponent>(entity);
						Interpolate(sizeOverLifeComponent, lifetimeComponent);
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread InterpolateVelocitySystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("InterpolateVelocityThread");

				auto velOverLifetimeView = reg.view<VelocityOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, velOverLifetimeView.begin(), velOverLifetimeView.end(), [&velOverLifetimeView](auto entity)
					{
						auto [velOverLifetimeComponent, lifetimeComponent] = velOverLifetimeView.get<VelocityOverLifetimeComponent, const LifetimeComponent>(entity);
						Interpolate(velOverLifetimeComponent, lifetimeComponent);
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	_NODISCARD std::thread InterpolateAngularVelocitySystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		std::thread thread([](ps_registry& reg)
			{
				PROFILE_THREAD("InterpolateAngularVelocityThread");

				auto view = reg.view<AngularVelocityOverLifetimeComponent, const LifetimeComponent>();
				std::for_each(EXECUTION_POLICY, view.begin(), view.end(), [&view](auto entity)
					{
						auto [angularVelOverLifeComponent, lifetimeComponent] = view.get<AngularVelocityOverLifetimeComponent, const LifetimeComponent>(entity);
						Interpolate(angularVelOverLifeComponent, lifetimeComponent);
					});
			}, std::ref(reg));

		return std::move(thread);
	}

	void DrawPixelSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		reg.view<const PositionComponent, const ColorComponent, const PixelDrawComponent>()
			.each([](const PositionComponent& pos, const ColorComponent& color)
				{
					DrawPixelV(pos.position, color.color);
				});
	}

	void DrawCircleSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		reg.view<const PositionComponent, const ColorComponent, const SizeComponent, const CircleDrawComponent>()
			.each([](const PositionComponent& pos, const ColorComponent& color, const SizeComponent& size)
				{
					DrawCircleV(pos.position, size.size.x, color.color);
				});
	}

	void DrawEllipseSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		reg.view<const PositionComponent, const ColorComponent, const SizeComponent, const EllipseDrawComponent>()
			.each([](const PositionComponent& pos, const ColorComponent& color, const SizeComponent& size)
				{
					DrawEllipse((int)pos.position.x, (int)pos.position.y, size.size.x, size.size.y, color.color);
				});
	}

	void DrawRectangleSystem(ps_registry& reg)
	{
		PROFILE_FUNCTION();

		reg.view<const PositionComponent, const ColorComponent, const SizeComponent, const RectDrawComponent>()
			.each([](const PositionComponent& pos, const ColorComponent& color, const SizeComponent& size)
				{
					DrawRectangleV(pos.position, size.size, color.color);
				});
	}

	void DrawPointBatchSystem(ps_registry& reg, PointBatchRenderer& pointBatchRenderer)
	{
		PROFILE_FUNCTION();

		reg.view<const PositionComponent, const ColorComponent, const SizeComponent, const PointBatchDrawComponent>()
			.each([&pointBatchRenderer](const PositionComponent& pos, const ColorComponent& color, const SizeComponent& size)
				{
					pointBatchRenderer.Add(pos.position, size.size.x, color.color);
				});
		pointBatchRenderer.Draw();
	}
}