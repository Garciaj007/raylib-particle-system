#pragma once

#include "../common.hpp"
#include "../gradient.hpp"

namespace naive
{
	class Particle
	{
	private:
		bool isAlive;

	public:
		Vector2 position;
		Vector2 velocity;
		Vector2 acceleration;
		float size;
		float spawnTime;
		float lifeTime;
		float emission;
		Color color;
		bool hasColorOverLifetime;
		Gradient colorOverLifetime;
		bool hasSizeOverLifetime;
		Vector2 sizeOverLifetime;

		friend class ParticleSystem;

		Particle() :
			isAlive(false),
			position(Vector2 { 0.f, 0.f }),
			velocity(Vector2 { 0.f, 0.f }),
			acceleration(Vector2 { 0.f, 0.f }),
			size(1.f),
			spawnTime(0.f),
			lifeTime(1.f),
			emission(0.f),
			color(GRAY),
			hasColorOverLifetime(false),
			colorOverLifetime(),
			hasSizeOverLifetime(false),
			sizeOverLifetime(Vector2 { 0.0f, 0.0f })
		{
		}

		virtual ~Particle() {}

		Particle(const Particle& other)
		{
			velocity = other.velocity;
			position = other.position;
			acceleration = other.acceleration;
			size = other.size;
			spawnTime = other.spawnTime;
			lifeTime = other.lifeTime;
			emission = other.emission;
			color = other.color;
			hasColorOverLifetime = other.hasColorOverLifetime;
			colorOverLifetime = other.colorOverLifetime;
			hasSizeOverLifetime = other.hasSizeOverLifetime;
			sizeOverLifetime = other.sizeOverLifetime;
		}

		virtual void Update(float time, float dt)
		{
			float t = time - spawnTime;

			if (t > lifeTime)
			{
				isAlive = false;
				return;
			}

			velocity.x += acceleration.x * dt;
			velocity.y += acceleration.y * dt;

			position.x += velocity.x * dt + 0.5f * acceleration.x * dt * dt;
			position.y += velocity.y * dt + 0.5f * acceleration.y * dt * dt;

			t = t / lifeTime;

			if (hasSizeOverLifetime)
			{
				size = Lerp(sizeOverLifetime.x, sizeOverLifetime.y, t);
			}

			if (hasColorOverLifetime)
			{
				color = colorOverLifetime.Evaluate(t);
			}
		}

		virtual void Draw()
		{
			if (isAlive)
			{
				DrawCircleV(position, size, color);
			}
		}
	};

	class ParticleSystem
	{
		std::vector<Scoped<Particle>> particles;
		Particle baseParticleData;
		uint32_t spawnCount;
		float spawnRate;
		float lastSpawnTime;
		bool isSpawning;

	public:
		Vector2 position;

		ParticleSystem() = delete;

		ParticleSystem(Particle baseParticleData, Vector2 position, float spawnRate = 0.f, uint32_t spawnCount = 1) :
			baseParticleData(baseParticleData),
			position(position),
			spawnRate(spawnRate),
			spawnCount(spawnCount),
			lastSpawnTime(0.f),
			isSpawning(spawnRate > 0.0f)
		{
		}

		void Spawn(uint32_t count, float time)
		{
			PROFILE_FUNCTION();

			for (int i = count - 1; i >= 0; --i)
			{
				Spawn(time);
			}
		}

		void Update(float time, float dt)
		{
			PROFILE_FUNCTION();

			if (isSpawning && time - lastSpawnTime > spawnRate)
			{
				Spawn(spawnCount, time);
				lastSpawnTime = time;
			}

			for (int i = (int)particles.size() - 1; i >= 0; --i)
			{
				particles [i]->Update(time, dt);
			}

			CleanUp();
		}

		void Draw()
		{
			PROFILE_FUNCTION();

			for (int i = (int)particles.size() - 1; i >= 0; --i)
			{
				particles [i]->Draw();
			}

			DrawText(TextFormat("Particle Count: %d", particles.size()), 4, 60, 20, LIME);
			DrawText(TextFormat("Size in Memory : %s", FormatBytes(particles.size() * sizeof(Particle))), 4, 80, 20, LIME);
		}

	private:
		void CleanUp()
		{
			PROFILE_FUNCTION();

			auto deadParticles = std::remove_if(particles.begin(), particles.end(), [] (auto& particle)
												{
													return !particle->isAlive;
												});
			particles.erase(deadParticles, particles.end());
		}

		void Spawn(float time)
		{
			PROFILE_FUNCTION();

			auto particle = new Particle(baseParticleData);
			particle->spawnTime = time;
			particle->position = Vector2Add(position, RandomVector2(-300, 300));
			particle->isAlive = true;
			particles.push_back(Scoped<Particle>(particle));
		}
	};
}