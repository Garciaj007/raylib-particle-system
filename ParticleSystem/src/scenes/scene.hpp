#pragma once

#include "../common.hpp"
#include "../instrumentation.hpp"

class IScene
{
public:
	virtual const char* GetName() = 0;
	virtual void Start() = 0;
	virtual void Resize(int width, int height) = 0;
	virtual void Update(float time, float dt) = 0;
	virtual void Draw() = 0;
	virtual ~IScene() = default;
};

class ASceneLoader
{
	Scoped<IScene> scene;

protected:
	virtual IScene* Create() = 0;

public:
	virtual const char* GetName() = 0;
	IScene* Get() { return scene.get(); }
	void Load() { scene = Scoped<IScene>(Create()); }
	void Unload() { scene.reset(); }
	virtual ~ASceneLoader() = default;
};

class SceneManager
{
	KeyboardKey active;
	std::unordered_map<KeyboardKey, Scoped<ASceneLoader>> sceneLoadersByKey;
	std::vector<KeyboardKey> keys;

	void Switch(KeyboardKey key)
	{
		if (key == active) return;

		const auto it = sceneLoadersByKey.find(key);
		if (it != sceneLoadersByKey.end())
		{
			if(sceneLoadersByKey.find(active) != sceneLoadersByKey.end())
				sceneLoadersByKey[active]->Unload();

			active = it->first;

			PROFILE_END_SESSION();

			const auto name = it->second->GetName();
			PROFILE_BEGIN_SESSION(TextFormat("%s Scene", name), TextFormat("./%s-scene-profile.json", name));
			
			sceneLoadersByKey[active]->Load();

			it->second->Get()->Start();
			it->second->Get()->Resize(GetScreenWidth(), GetScreenHeight());
		}

		SetWindowTitle(PrependSolutionName(it->second->GetName()));
	}

	void Randomize()
	{
		auto randomize = dynamic_cast<IRandomize*>(sceneLoadersByKey[active]->Get());
		if (randomize) randomize->Randomize();
	}

public:
	SceneManager() = delete;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(std::initializer_list<std::pair<KeyboardKey, ASceneLoader*>> skList)
	{
		for (auto& pair : skList)
		{
			keys.push_back(pair.first);
			sceneLoadersByKey [pair.first] = Scoped<ASceneLoader>(pair.second);
		}
		Switch(skList.begin()->first);
	}

	const char* GetActiveSceneName() { return sceneLoadersByKey[active]->GetName(); }

	void Resize(int width, int height) { sceneLoadersByKey[active]->Get()->Resize(width, height); }

	void Update(float time, float dt)
	{
		sceneLoadersByKey[active]->Get()->Update(time, dt);

		for (const auto& key : keys)
		{
			if (IsKeyReleased(key))
			{
				Switch(key);
			}
		}

		if (IsKeyReleased(KEY_R))
		{
			Randomize();
		}
	}

	void Draw() { sceneLoadersByKey[active]->Get()->Draw(); }
};