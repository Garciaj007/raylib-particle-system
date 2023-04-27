#pragma once

#include "../common.hpp"
#include "../gradient.hpp"
#include "../batchrenderer.hpp"

#include "particledrawers.hpp"
#include "particleemittershape.hpp"

#include "../ecs/common.hpp"
#include "../ecs/systems.hpp"

namespace ecs
{
	class Particle;
	class ParticleManager;
	class ParticleEmitter;

	struct EmitterComponent
	{
		bool isSpawning;
		float lastSpawnTime;

		SharedParticleData data;
		uint32_t spawnCount;
		float spawnRate;

		EmitterComponent(SharedParticleData data, uint32_t spawnCount, float spawnRate) :
			isSpawning(false),
			lastSpawnTime(0.0f),
			data(data),
			spawnCount(spawnCount),
			spawnRate(spawnRate)
		{
		}
	};

	template<typename Component>
	std::pair<std::size_t, std::size_t> ComponentSizeFunction(const ps_registry& reg)
	{
		auto count = reg.view<PositionComponent>().size();
		return std::make_pair(count, (std::size_t)count * sizeof(PositionComponent));
	}

	class Entity;

	class ParticleManager
	{
		ps_registry registry;

		std::vector<std::function<std::pair<std::size_t, std::size_t>(const ps_registry&)>> componentSizeFunctions;

		PointBatchRenderer pointBatchRenderer;

		friend Entity;

	public:
		ParticleManager() :
			pointBatchRenderer(1000000)
		{
			PROFILE_FUNCTION();

			AddComponentSizeFunctions();
		}

		Ref<Entity> SpawnEmitter(SharedParticleData data, uint32_t count, Vector2 position, float rotation, float spawnRate, float time)
		{
			PROFILE_FUNCTION();

			auto entity = registry.create();

			registry.emplace<EmitterComponent>(entity, data, count, spawnRate);
			registry.emplace<PositionComponent>(entity, position);
			registry.emplace<RotationComponent>(entity, rotation);

			return MakeRef<Entity>(entity);
		}

		void Resize(int width, int height)
		{
			PROFILE_FUNCTION();

			pointBatchRenderer.SetProjectionMatrix(MatrixOrtho(0, width, height, 0, -1, 1));
		}

		void Update(float time, float dt)
		{
			PROFILE_FUNCTION();

			ecs::DestroyEntitySystem(registry);
			SpawnParticleSystem(time);
			auto lifetimeUpdateThread = ecs::LifetimeUpdateSystem(registry, time); 
			auto kinematicUpdateThread = ecs::KinematicUpdateSystem(registry, dt);
			auto positionUpdateThread = ecs::PositionUpdateSystem(registry, dt);
			
			auto applyVelocityInterpolatedThread = ecs::ApplyInterpolatedVelocitySystem(registry);
			auto applySizeInterpolatedThread = ecs::ApplyInterpolatedSizeSystem(registry);
			auto applyColorInterpolatedThread = ecs::ApplyInterpolatedColorSystem(registry);

			auto interpolateVelocityThread = ecs::InterpolateVelocitySystem(registry);
			auto interpolateSizeThread = ecs::InterpolateSizeSystem(registry);
			auto interpolateColorThread = ecs::InterpolateColorSystem(registry);
			auto interpolateRotationThread = ecs::InterpolateRotationSystem(registry);
			auto interpolateAngularVelocityThread = ecs::InterpolateAngularVelocitySystem(registry);
		
			lifetimeUpdateThread.join();
			kinematicUpdateThread.join();
			positionUpdateThread.join();

			applyVelocityInterpolatedThread.join();
			applySizeInterpolatedThread.join();
			applyColorInterpolatedThread.join();
			
			interpolateVelocityThread.join();
			interpolateSizeThread.join();
			interpolateColorThread.join();
			interpolateRotationThread.join();
			interpolateAngularVelocityThread.join();

		}

		void Draw()
		{
			PROFILE_FUNCTION();

			ecs::DrawPixelSystem(registry);
			ecs::DrawCircleSystem(registry);
			ecs::DrawEllipseSystem(registry);
			ecs::DrawRectangleSystem(registry);
			ecs::DrawPointBatchSystem(registry, pointBatchRenderer);

			// Calculate Registry size
			const auto registrySize = registry.size();
			const auto registryAliveSize = registry.alive();

			// Calculate the total size of all components
			std::size_t componentsCount = 0;
			std::size_t componentsSize = 0;
			for (const auto& componentSizeFunction : componentSizeFunctions)
			{
				const auto componentSizeResult = componentSizeFunction(registry);
				componentsCount += componentSizeResult.first;
				componentsSize += componentSizeResult.second;
			}

			DrawText(TextFormat("Entities: %d / %d", registryAliveSize, registrySize), 4, 60, 20, LIME);
			DrawText(TextFormat("Components: %d / Size: %s", componentsCount, FormatBytes(componentsSize)), 4, 80, 20, LIME);
		}

	private:
		template <typename Component, typename T>
		void CheckAndAddComponent(ps_entity entity, std::optional<T> value)
		{
			if (value) registry.emplace<Component>(entity, value.value());
		}

		template <typename Component, typename T>
		void CheckAndAddComponent(ps_entity entity, Ref<T> reference)
		{
			if (reference) registry.emplace<Component>(entity, *reference);
		}

		void AddComponentSizeFunctions()
		{
			componentSizeFunctions.push_back(ComponentSizeFunction<ColorOverLifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RotationOverLifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<SizeOverLifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<VelocityOverLifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<AngularVelocityOverLifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<DestroyEntityComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<PixelDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<CircleDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<EllipseDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RectDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RingDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RectGradientDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RoundedRectDrawComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<LifetimeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<PositionComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<VelocityComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<AccelerationComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<RotationComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<AngularVelocityComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<AngularAccelerationComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<SizeComponent>);
			componentSizeFunctions.push_back(ComponentSizeFunction<ColorComponent>);
		}

		// Spawning Particles from Emitters
		void SpawnParticleSystem(float time)
		{
			PROFILE_FUNCTION();

			registry.view<EmitterComponent, const PositionComponent, const RotationComponent>().each([this, time](auto entity,
				EmitterComponent& emitter,
				const PositionComponent& position,
				const RotationComponent& rotation)
				{
					if (emitter.isSpawning && time - emitter.lastSpawnTime > emitter.spawnRate)
					{
						Spawn(emitter.data, emitter.spawnCount, position.position, rotation.rotation, time);
						emitter.lastSpawnTime = time;
					}
				});
		}

		void Spawn(SharedParticleData data, uint32_t count, Vector2 position, float rotation, float time)
		{
			PROFILE_FUNCTION();

			auto emitterShape = data.emitterShape;

			auto velocityOverLifetime = data.velocityOverLifetime;
			auto rotationOverLifetime = data.rotationOverLifetime;
			auto angularVelocityOverLifetime = data.angularVelocityOverLifetime;
			auto sizeOverLifetime = data.sizeOverLifetime;
			auto colorOverLifetime = data.colorOverLifetime;

			auto drawType = data.drawType;

			for (int i = count - 1; i >= 0; --i)
			{
				auto entity = registry.create();

				data.Randomize();

				registry.emplace<LifetimeComponent>(entity, data.GetLifetime(), time);

				Vector2 pos = Vector2Add(position, Vector2Rotate(emitterShape->GetStartPos(), rotation * DEG2RAD));
				registry.emplace<PositionComponent>(entity, pos);

				Vector2 vel = Vector2Rotate(Vector2Add(data.GetVelocity(), emitterShape->GetStartVel()), rotation * DEG2RAD);
				registry.emplace<VelocityComponent>(entity, vel);
				CheckAndAddComponent<VelocityOverLifetimeComponent>(entity, velocityOverLifetime);
				CheckAndAddComponent<AccelerationComponent>(entity, data.GetAcceleration());

				CheckAndAddComponent<RotationComponent>(entity, data.GetRotation());
				CheckAndAddComponent<RotationOverLifetimeComponent>(entity, rotationOverLifetime);
				CheckAndAddComponent<AngularVelocityComponent>(entity, data.GetAngularVelocity());
				CheckAndAddComponent<AngularVelocityOverLifetimeComponent>(entity, angularVelocityOverLifetime);
				CheckAndAddComponent<AngularAccelerationComponent>(entity, data.GetAngularAcceleration());

				CheckAndAddComponent<SizeOverLifetimeComponent>(entity, sizeOverLifetime);
				registry.emplace<SizeComponent>(entity, data.GetSize());

				CheckAndAddComponent<ColorOverLifetimeComponent>(entity, colorOverLifetime);
				registry.emplace<ColorComponent>(entity, data.GetColor());

				switch (drawType)
				{
				default:
				case DrawType::PIXEL: registry.emplace<PixelDrawComponent>(entity);
					break;
				case DrawType::CIRCLE: registry.emplace<CircleDrawComponent>(entity);
					break;
				case DrawType::ELLIPSE: registry.emplace<EllipseDrawComponent>(entity);
					break;
				case DrawType::RING: registry.emplace<RingDrawComponent>(entity, data.ringDrawCompProto);
					break;
				case DrawType::RECT: registry.emplace<RectDrawComponent>(entity);
					break;
				case DrawType::RECT_GRADIENT: registry.emplace<RectGradientDrawComponent>(entity, data.rectGradDrawCompProto);
					break;
				case DrawType::ROUNDED_RECT: registry.emplace<RoundedRectDrawComponent>(entity, data.roundedRectDrawCompProto);
					break;
				case DrawType::BATCH_CIRCLE: registry.emplace<CircleBatchDrawComponent>(entity);
					break;
				case DrawType::POINT: registry.emplace<PointBatchDrawComponent>(entity);
					break;
				}
			}
		}
	};

	static Scoped<ParticleManager> manager;

	class Entity
	{
		ps_entity entity;

	public:
		Entity() : entity(entt::null) {}
		Entity(ps_entity entity) : entity(entity) {}

		template<typename... Component>
		[[nodiscard]] decltype(auto) GetComponent() const
		{
			return manager->registry.get<Component...>(entity);
		}

		template<typename... Component>
		[[nodiscard]] decltype(auto) GetComponent()
		{
			return manager->registry.get<Component...>(entity);
		}

		template<typename... Component>
		[[nodiscard]] decltype(auto) TryGetComponent() const
		{
			return manager->registry.try_get<Component...>(entity);
		}

		template<typename... Component>
		[[nodiscard]] decltype(auto) TryGetComponent()
		{
			return manager->registry.try_get<Component>(entity);
		}

		template<typename Component, typename... Args>
		decltype(auto) AddComponent(Args &&...args)
		{
			return manager->registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component, typename... Args>
		decltype(auto) GetOrAddComponent(Args &&...args)
		{
			return manager->registry.get_or_emplace<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component, typename... Other>
		void RemoveComponent()
		{
			manager->registry.remove<Component, Other...>(entity);
		}
	};

	/// Accessable Functions...

	void Init()
	{
		manager.reset(new ParticleManager());
	}

	Ref<Entity> SpawnEmitter(SharedParticleData data, uint32_t count, Vector2 position, float rotation, float spawnRate, float time)
	{
		return manager->SpawnEmitter(data, count, position, rotation, spawnRate, time);
	}

	void Update(float time, float dt)
	{
		manager->Update(time, dt);
	}

	void Resize(int width, int height)
	{
		manager->Resize(width, height);
	}

	void Draw()
	{
		manager->Draw();
	}

	void Destroy()
	{
		manager.reset();
	}
}


