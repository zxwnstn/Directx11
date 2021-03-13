#include "Scene.h"

class PhongShadingScene : public Scene
{
public:
	void OnUpdate(float dt) override;
	void OnImGui() override;
	void Init() override;
};