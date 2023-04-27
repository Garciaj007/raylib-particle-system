#pragma once

#include "scene.hpp"
#include "../particles/advanced.hpp"

class AdvancedPsScene : public IScene
{
	Scoped<advanced::ParticleEmitter> emitter1;
	Scoped<advanced::ParticleEmitter> emitter2;
	Scoped<advanced::ParticleEmitter> emitter3;

public:
	AdvancedPsScene()
	{
		advanced::manager = MakeScoped<advanced::ParticleManager>();
	}

	// Inherited via IScene
	const char* GetName() override { return "Advanced Particle System"; }

	void Start() override
	{
		PROFILE_FUNCTION();

		auto gradient = MakeRef<advanced::Gradient>(advanced::Gradient{
				{ 0.0f, ColorAlpha(DARKGREEN, 0.f)},
				{0.25f, GREEN},
				{0.5f, ColorAlpha(GOLD, 1.0f)},
				{0.75f, ColorAlpha(YELLOW, 0.25f)},
				{1.0f, ColorAlpha(ORANGE, 0.f)}
			});

		auto sizeInterpolator = MakeRef<advanced::Vector2Interpolator>(advanced::Vector2Interpolator{
				{0.f, {0.f, 0.f}},
				{0.9f, {10.f, 0.f}},
				{1.f, {10.f, 0.f}}
			});

		// Particle Emitter Shapes
		auto lineEmitterShape = MakeRef<LineEmitterShape>(200.0f);
		auto boxEmitterShape = MakeRef<BoxEmitterShape>(400.0f, 400.0f, false);
		auto circleEmitterShape = MakeRef<CircleEmitterShape>(400.0f, true);
		//auto coneEmitterShape = MakeRef<ConeEmitterShape>(20.0f, 30.0f, 200.0f); 
		// OR
		auto coneEmitterShape = MakeRef<ConeEmitterShape>(20.0f, 30.0f, Vector2{ 100.0f, 200.0f });


		// Particle Drawers
		auto pixelDrawer = MakeRef<PixelParticleDrawer>();
		auto circleDrawer = MakeRef<CircleParticleDrawer>();
		auto rectDrawer = MakeRef<RectParticleDrawer>();
		auto roundedRectDrawer = MakeRef<RoundedRectParticleDrawer>(0.5f);
		auto rectGradientDrawer = MakeRef<RectGradientParticleDrawer>(BLANK);
		auto ringDrawer = MakeRef<RingParticleDrawer>(6);

		auto sharedData1 = MakeRef<advanced::SharedParticleData>();
		sharedData1->lifeTime = 2.0f;
		sharedData1->drawer = pixelDrawer;
		//sharedData1->sizeOverLifetime = sizeInterpolator;
		sharedData1->color = GRAY;
		sharedData1->colorOverLifetime = gradient;

		auto sharedData2 = MakeRef<advanced::SharedParticleData>();
		sharedData2->lifeTime = 4.0f;
		sharedData2->drawer = ringDrawer;
		sharedData2->size = { 20.0f, 40.0f };
		sharedData2->sizeOverLifetime = sizeInterpolator;
		sharedData2->colorOverLifetime = gradient;

		auto sharedData3 = MakeRef<advanced::SharedParticleData>();
		sharedData3->lifeTime = 5.0f;
		sharedData3->drawer = roundedRectDrawer;
		sharedData3->size = { 40.0f, 40.0f };
		sharedData3->colorOverLifetime = gradient;

		emitter1 = Scoped<advanced::ParticleEmitter>(
			new advanced::ParticleEmitter(
				boxEmitterShape,
				sharedData1,
				{ GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },
				45.0f,
				0.1f,
				25000
			));
		emitter1->Start();

		//emitter2 = Scoped<advanced::ParticleEmitter>(
		//	new advanced::ParticleEmitter(
		//		circleEmitterShape,
		//		sharedData2,
		//		{ GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },
		//		0.0f,
		//		0.2f,
		//		8
		//	));
		//emitter2->Start();

		//emitter3 = Scoped<advanced::ParticleEmitter>(
		//	new advanced::ParticleEmitter(
		//		coneEmitterShape,
		//		sharedData3,
		//		{ GetScreenWidth() / 2.f, GetScreenHeight() / 2.f },
		//		180.0f,
		//		0.2f,
		//		4
		//	));
		//emitter3->Start();
	}

	void Resize(int width, int height) override
	{
		emitter1->SetPosition({ width / 2.0f, height / 2.0f });
		//emitter2->SetPosition({ width / 2.0f, height / 2.0f });
		//emitter3->SetPosition({ width / 2.0f, height / 2.0f });
	}

	void Update(float time, float dt) override
	{
		PROFILE_FUNCTION();

		//if (IsKeyReleased(KEY_A))
		//{
		//	if (emitter2->IsSpawning())
		//	{
		//		emitter2->Stop();
		//	}
		//	else
		//	{
		//		emitter2->Start();
		//	}
		//}

		emitter1->SetRotation(45.0f + time * 2.0f);

		advanced::manager->Update(time, dt);
	}

	void Draw() override
	{
		PROFILE_FUNCTION();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		advanced::manager->Draw();

		DrawFPS(4, 40);
		DrawText(GetName(), 4, 4, 40, LIGHTGRAY);

		EndDrawing();
	}
};

class AdvancedPSSceneLoader : public ASceneLoader
{
public:
	const char* GetName() override { return "Advanced Particle System"; }
protected:
	IScene* Create() { return new AdvancedPsScene(); }
};