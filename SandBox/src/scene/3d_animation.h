#include "Scene.h"

class Animation3D : public Scene
{
public:
	void OnUpdate(float dt) override;
	void OnImGui() override;
	void Init() override;
};