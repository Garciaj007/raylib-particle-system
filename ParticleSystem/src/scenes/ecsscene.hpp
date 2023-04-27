#pragma once

#include "scene.hpp"
#include "../particles/ecs.hpp"

class ECSPSScene : public IScene
{
	Ref<ecs::Entity> emitter1;
	Ref<ecs::Entity> emitter2;
	Ref<ecs::Entity> emitter3;

public:
	ECSPSScene()
	{
		PROFILE_FUNCTION();

		ecs::Init();
	}

	~ECSPSScene()
	{
		PROFILE_FUNCTION();

		ecs::Destroy();
	}

	const char* GetName() override { return "ECS Particle System"; }

	void Start() override
	{
		PROFILE_FUNCTION();

		auto time = (float)GetTime();

		auto colorInterpolator = MakeRef<ecs::ColorOverLifetimeComponent>(ecs::ColorOverLifetimeComponent{
			{0.0f, ColorAlpha(DARKBLUE, 0.f)},
			{0.25f, BLUE},
			{0.5f, ColorAlpha(SKYBLUE, 1.0f)},
			{0.75f, ColorAlpha(LIME, 0.25f)},
			{1.0f, ColorAlpha(GREEN, 0.f)}
			});

		auto sizeInterpolator = MakeRef<ecs::SizeOverLifetimeComponent>(ecs::SizeOverLifetimeComponent{
			{0.f, {0.f, 0.f}},
			{0.9f, {10.f, 0.f}},
			{1.f, {10.f, 0.f}}
			});

		auto boxEmitterShape = MakeRef<BoxEmitterShape>(600.0f, 600.0f, false);

		ecs::SharedParticleData sharedData1;
		sharedData1.lifetime = 2.0f;
		sharedData1.colorOverLifetime = colorInterpolator;
		sharedData1.emitterShape = boxEmitterShape;
		sharedData1.size = {6.0f, 10.0f};
		sharedData1.acceleration = { 0.0f, 98.0f };
		sharedData1.color = GRAY;
		//sharedData1.sizeOverLifetime = sizeInterpolator;
		sharedData1.drawType = ecs::DrawType::POINT;

		auto center = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
		emitter1 = ecs::SpawnEmitter(sharedData1, 25000, center, 0.0f, 0.1f, time);
		emitter1->GetComponent<ecs::EmitterComponent>().isSpawning = true;
	}

	void Resize(int width, int height) override
	{
		emitter1->GetComponent<ecs::PositionComponent>().position = Vector2{ width / 2.0f, height / 2.0f };

		ecs::Resize(width, height);
	}

	void Update(float time, float dt) override
	{
		PROFILE_FUNCTION();

		emitter1->GetComponent<ecs::RotationComponent>().rotation += 0.4f;

		ecs::Update(time, dt);
	}

	void Draw() override
	{
		PROFILE_FUNCTION();

		BeginDrawing();

		ClearBackground(RAYWHITE);

		ecs::Draw();

		DrawFPS(4, 40);
		DrawText(GetName(), 4, 4, 40, LIGHTGRAY);

		EndDrawing();
	}
};

class ECSPSSceneLoader : public ASceneLoader
{
public:
	const char* GetName() override { return "ECS Particle System"; }
protected:
	IScene* Create() { return new ECSPSScene(); }
};