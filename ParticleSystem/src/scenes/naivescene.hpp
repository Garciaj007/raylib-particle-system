#pragma once

#include "scene.hpp"
#include "../particles/naive.hpp"

class NaivePsScene : public IScene
{
	Scoped<naive::ParticleSystem> particleSystem;

public:
	const char* GetName() { return "Naive Particle System"; }

	void Start() override
	{
		PROFILE_FUNCTION();

		naive::Gradient gradient = naive::Gradient({
				{ 0.0f, ColorAlpha(YELLOW, 0.f)},
				{0.25f, GREEN},
				{0.5f, ColorAlpha(SKYBLUE, 0.25f)},
				{0.75f, ColorAlpha(VIOLET, 0.5f)},
				{1.0f, ColorAlpha(RED, 0.f)}
													   });

		naive::Particle protoParticle;
		protoParticle.acceleration.y = 100.0f;
		protoParticle.velocity.y = -200.f;
		protoParticle.velocity.x = -50.f;
		protoParticle.lifeTime = 5.0f;
		protoParticle.hasColorOverLifetime = true;
		protoParticle.colorOverLifetime = gradient;
		protoParticle.hasSizeOverLifetime = true;
		protoParticle.sizeOverLifetime = Vector2 { 0, 10 };

		particleSystem = Scoped<naive::ParticleSystem>(
			new naive::ParticleSystem(
				protoParticle,
				{ GetScreenWidth() / 2.f , GetScreenHeight() / 2.f },
				0.1f,
				200
			));
		particleSystem->Spawn(100, (float)GetTime());
	}

	void Resize(int width, int height) override
	{
		particleSystem->position = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	}

	void Update(float time, float dt) override
	{
		PROFILE_FUNCTION();

		particleSystem->Update(time, dt);
	}

	void Draw() override
	{
		PROFILE_FUNCTION();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		particleSystem->Draw();

		DrawFPS(4, 40);
		DrawText(GetName(), 4, 4, 40, LIGHTGRAY);

		EndDrawing();
	}
};

class NaivePSSceneLoader : public ASceneLoader
{
public:
	const char* GetName() { return "Naive Particle System"; }
protected:
	IScene* Create() override { return new NaivePsScene(); }
};