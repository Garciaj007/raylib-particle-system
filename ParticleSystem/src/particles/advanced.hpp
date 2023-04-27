#pragma once

#include "../common.hpp"
#include "../gradient.hpp"

#include "particleemittershape.hpp"
#include "particledrawers.hpp"

namespace advanced
{
	class Particle;
	class ParticleManager;
	class ParticleEmitter;

	class IParticleManager
	{
	public:
		virtual void Reserve(ParticleEmitter* emitter) = 0;
		virtual void Spawn(ParticleEmitter* emitter, uint32_t count, float time) = 0;
		virtual void Release(ParticleEmitter* emitter) = 0;
		virtual void Update(float time, float dt) = 0;
		virtual void Draw() = 0;
		virtual ~IParticleManager() = default;
	};

	Scoped<IParticleManager> manager;

	struct SharedParticleData
	{
		Vector2 acceleration;
		Vector2 velocity;
		Color color;
		float lifeTime;
		Vector2 size;
		Ref<advanced::Gradient> colorOverLifetime;
		Ref<Vector2Interpolator> sizeOverLifetime;
		Ref<IParticleDrawer> drawer;

		SharedParticleData() :
			acceleration({ 0.f, 0.f }),
			velocity({ 0.f, 0.f }),
			color(WHITE),
			lifeTime(1.f),
			size({ 10.0f }),
			colorOverLifetime(nullptr),
			sizeOverLifetime(nullptr),
			drawer(nullptr)
		{
		}
	};

	struct ParticleData
	{
		uint32_t id;
		bool isAlive;
		Color color;
		float spawnTime;
		Vector2 size;
		Vector2 position;
		Vector2 velocity;

		ParticleData() :
			id(0),
			isAlive(false),
			position(Vector2{}),
			velocity(Vector2{}),
			size(Vector2{}),
			spawnTime(0.f),
			color(GRAY)
		{
		}
	};

	class Particle
	{
		ParticleData data;
		Ref<SharedParticleData> sharedData;

		friend ParticleManager;

	public:
		Particle() :
			data({}),
			sharedData(nullptr)
		{
		}

		void InitAndApply(Ref<SharedParticleData> sharedData, Vector2 position, Vector2 velocity, float time)
		{
			this->sharedData = sharedData;
			
			data = ParticleData();
			data.isAlive = true;
			data.size = sharedData->size;
			data.color = sharedData->color;
			data.position = position;
			data.velocity = velocity;
			data.spawnTime = time;
		}

		void Update(float time, float dt)
		{
			float t = time - data.spawnTime;

			if (t > sharedData->lifeTime)
			{
				data.isAlive = false;
				return;
			}

			const auto acceleration = sharedData->acceleration;

			data.velocity.x += acceleration.x * dt;
			data.velocity.y += acceleration.y * dt;

			data.position.x += data.velocity.x * dt + 0.5f * acceleration.x * dt * dt;
			data.position.y += data.velocity.y * dt + 0.5f * acceleration.y * dt * dt;

			t = t / sharedData->lifeTime;

			const auto& sizeOverLifetime = sharedData->sizeOverLifetime;
			if (sizeOverLifetime)
			{
				data.size = sizeOverLifetime->Evaluate(t);
			}

			const auto& colorOverLifetime = sharedData->colorOverLifetime;
			if (colorOverLifetime)
			{
				data.color = colorOverLifetime->Evaluate(t);
			}
		}

		void Draw()
		{
			if (data.isAlive)
			{
				sharedData->drawer->Draw({data.position, data.size, data.color});
			}
		}
	};

	class ParticleEmitter
	{
		int id;
		bool isAlive;
		bool isSpawning;
		uint32_t spawnCapacity;

		uint32_t spawnCount;

		Vector2 position;
		float rotation;
		float spawnRate;
		float lastSpawnTime;

		Ref<SharedParticleData> sharedParticleData;

		Ref<IEmitterShape> emitterShape;

		friend ParticleManager;

		Vector2 GetStartPos()
		{
			return Vector2Add(position, Vector2Rotate(emitterShape->GetStartPos(), rotation * DEG2RAD));
		}

		Vector2 GetStartVel()
		{
			return Vector2Rotate(Vector2Add(sharedParticleData->velocity, emitterShape->GetStartVel()), rotation * DEG2RAD);
		}

	public:
		bool IsSpawning() { return isAlive && isSpawning; }

		float Rotation() { return rotation; }
		void SetRotation(float rotation) { this->rotation = rotation; }

		Vector2 Position() { return position; }
		void SetPosition(Vector2 position) { this->position = position; }

		ParticleEmitter(Ref<IEmitterShape> emitterShape,
			Ref<SharedParticleData> sharedParticleData,
			Vector2 position,
			float rotation = 0,
			float spawnRate = 0.0f,
			uint32_t spawnCount = 1) :
			id(0),
			isAlive(false),
			emitterShape(emitterShape),
			sharedParticleData(sharedParticleData),
			position(position),
			rotation(rotation),
			spawnRate(spawnRate),
			spawnCount(spawnCount),
			lastSpawnTime(0.0f),
			isSpawning(spawnRate > 0.0f),
			spawnCapacity(isSpawning ? (int)ceilf((1 / spawnRate) * spawnCount * sharedParticleData->lifeTime) : spawnCount)
		{
			manager->Reserve(this);
		}

		~ParticleEmitter()
		{
			manager->Release(this);
		}

		void Spawn(int count, float time)
		{
			manager->Spawn(this, count, time);
		}

		void Start() { isAlive = true; }

		void Stop() { isAlive = false; }

		void Update(float time)
		{
			PROFILE_FUNCTION();

			if (isAlive && isSpawning && time - lastSpawnTime > spawnRate)
			{
				manager->Spawn(this, spawnCount, time);
				lastSpawnTime = time;
			}
		}
	};

	class ParticleManager : public IParticleManager
	{
		std::vector<Particle> particles;
		std::vector<Particle> particlePool;
		std::unordered_map<uint16_t, ParticleEmitter*> emitters;
		TUID<uint32_t> emitterTUID;
		TUID<uint32_t> particleTUID;

		// Non copyable & moveable
		ParticleManager(const ParticleManager&) = delete;
		ParticleManager& operator=(const ParticleManager&) = delete;

	public:
		ParticleManager() = default;
		~ParticleManager() = default;

		void Reserve(ParticleEmitter* emitter) override
		{
			PROFILE_FUNCTION();

			emitter->id = emitterTUID.GetNext();
			emitters.emplace(emitter->id, emitter);
			ReserveCapacity(emitter->spawnCapacity);
		}

		void Spawn(ParticleEmitter* emitter, uint32_t count, float time) override
		{
			PROFILE_FUNCTION();

			if (particlePool.size() < count)
			{
				ReserveCapacity(count);
			}

			const auto first = particlePool.begin();
			const auto last = std::next(first, count);

			for (auto it = first; it != last; ++it)
			{
				(*it).InitAndApply(emitter->sharedParticleData, emitter->GetStartPos(), emitter->GetStartVel(), time);
				particles.push_back(std::move(*it));
			}

			particlePool.erase(first, last);
		}

		void Release(ParticleEmitter* emitter) override
		{
			PROFILE_FUNCTION();

			if (particlePool.size() > emitter->spawnCapacity)
			{
				const auto first = particlePool.begin();
				const auto last = std::next(first, emitter->spawnCapacity);
				particlePool.erase(first, last);
			}
			emitters.erase(emitter->id);
		}

		void Update(float time, float dt) override
		{
			PROFILE_FUNCTION();

			for (const auto& emitter : emitters)
			{
				emitter.second->Update(time);
			}

			FilterAndClean();

			for (auto& particle : particles)
			{
				particle.Update(time, dt);
			}
		}

		void Draw() override
		{
			PROFILE_FUNCTION();

			std::for_each(particles.begin(), particles.end(), [](Particle p) { p.Draw(); });

			const auto activeCount = particles.size();
			const auto totalCount = activeCount + particlePool.size();
			const auto activeSize = activeCount * sizeof(Particle);
			const auto totalSize = totalCount * sizeof(Particle);

			DrawText(TextFormat("Particle Count: %d / %d", activeCount, totalCount), 4, 60, 20, LIME);
			DrawText(TextFormat("Size in Memory : %s / %s", FormatBytes(activeSize), FormatBytes(totalSize)), 4, 80, 20, LIME);
		}

	private:
		void ReserveCapacity(uint32_t count)
		{
			PROFILE_FUNCTION();

			for (uint32_t i = 0; i < count; i++)
			{
				auto particle = Particle();
				particle.data.id = particleTUID.GetNext();
				particlePool.push_back(particle);
			}
		}

		void FilterAndClean()
		{
			PROFILE_FUNCTION();

			auto removeIt = std::remove_if(particles.begin(), particles.end(),
				[](const Particle& particle)
				{
					return !particle.data.isAlive;
				});

			for (auto it = removeIt; it != particles.end(); ++it)
			{
				particlePool.push_back(std::move(*it));
			}

			particles.erase(removeIt, particles.end());
		}
	};
}