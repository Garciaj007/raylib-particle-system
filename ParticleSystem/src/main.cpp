#include "common.hpp"

#include "scenes/scene.hpp"

#include "scenes/naivescene.hpp"
#include "scenes/simplescene.hpp"
#include "scenes/advancedscene.hpp"
#include "scenes/ecsscene.hpp"

int main()
{
	SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE);
	InitWindow(1440, 810, PrependSolutionName("Particle System"));

	gladLoadGL();

	Scoped<SceneManager> sceneManager = Scoped<SceneManager>(new SceneManager{
			{KEY_ONE, new NaivePSSceneLoader()},
			{KEY_TWO, new SimplePSSceneLoader()},
			{KEY_THREE, new AdvancedPSSceneLoader()},
			{KEY_FOUR, new ECSPSSceneLoader()},
		});

	while (!WindowShouldClose())
	{
		if (IsWindowResized())
		{
			sceneManager->Resize(GetScreenWidth(), GetScreenHeight());
		}
		sceneManager->Update((float)GetTime(), GetFrameTime());
		sceneManager->Draw();
	}

	// End any open profiling sessions;
	PROFILE_END_SESSION();

	CloseWindow();

	return 0;
}