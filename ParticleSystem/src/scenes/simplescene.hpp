#pragma once

#include "scene.hpp"
#include "../particles/simple.hpp"

class SimplePsScene : public IScene
{
	Scoped<simple::ParticleEmitter> emitter1;
	Scoped<simple::ParticleEmitter> emitter2;

public:
	SimplePsScene()
	{
		simple::manager = MakeScoped<simple::ParticleManager>();
	}

	// Inherited via IScene
	const char* GetName() override { return "Simple Particle System"; }

	void Start() override
	{
		PROFILE_FUNCTION();

		auto gradient = Ref<naive::Gradient>(
			new naive::Gradient({
				{ 0.0f, ColorAlpha(PURPLE, 0.f)},
				{0.25f, VIOLET},
				{0.5f, ColorAlpha(SKYBLUE, 0.5f)},
				{0.75f, ColorAlpha(BLUE, 0.25f)},
				{1.0f, ColorAlpha(DARKBLUE, 0.f)}
								  }));

		//auto lineEmitterShape = MakeRef<ps::LineEmitterShape>(200.0f);
		auto boxEmitterShape = MakeRef<BoxEmitterShape>(100.0f, 100.0f);
		auto circleEmitterShape = MakeRef<CircleEmitterShape>(200.0f, true);

		auto sharedData1 = MakeRef<simple::SharedParticleData>();
		sharedData1->lifeTime = 1.0f;
		sharedData1->sizeOverLifetime = Ref<Vector2>(new Vector2 { 0.0f, 20.0f });
		sharedData1->colorOverLifetime = gradient;

		auto sharedData2 = MakeRef<simple::SharedParticleData>();
		sharedData2->lifeTime = 4.0f;
		sharedData2->sizeOverLifetime = Ref<Vector2>(new Vector2 { 0.0f, 10.0f });
		sharedData2->colorOverLifetime = gradient;

		emitter1 = Scoped<simple::ParticleEmitter>(
			new simple::ParticleEmitter(
				boxEmitterShape,
				sharedData1,
				{ GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },
				45.0f,
				0.1f,
				200
			));
		emitter1->Start();

		emitter2 = Scoped<simple::ParticleEmitter>(
			new simple::ParticleEmitter(
				circleEmitterShape,
				sharedData2,
				{ GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },
				0.0f,
				0.1f,
				200
			));
		emitter2->Start();
	}

	void Resize(int width, int height) override
	{
		PROFILE_FUNCTION();

		emitter1->SetPosition({ width / 2.0f, height / 2.0f });
		emitter2->SetPosition({ width / 2.0f, height / 2.0f });
	}

	void Update(float time, float dt) override
	{
		PROFILE_FUNCTION();

		if (IsKeyReleased(KEY_A))
		{
			if (emitter2->IsSpawning())
			{
				emitter2->Stop();
			}
			else
			{
				emitter2->Start();
			}
		}

		emitter1->SetRotation(45.0f + time * 0.5f);
		emitter1->SetPosition(Vector2AddValue(emitter1->Position(), 1.0f));

		simple::manager->Update(time, dt);
	}

	void Draw() override
	{
		PROFILE_FUNCTION();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		simple::manager->Draw();

		DrawFPS(4, 40);
		DrawText(GetName(), 4, 4, 40, LIGHTGRAY);

		EndDrawing();
	}
};

class SimplePSSceneLoader : public ASceneLoader
{
public:
	const char* GetName() override { return "Simple Particle System"; }
protected:
	IScene* Create() override { return new SimplePsScene(); }
};