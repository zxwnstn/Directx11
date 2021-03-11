#include "Scene.h"

class FbxLoadScene : public Scene
{
public:
	void OnUpdate(float dt) override;
	void OnImGui() override;
	void Init() override;

private:
	int curModelIdx = 0;
	float animationSpeed = 0.016f;
	std::vector<std::string> ModelNames;
	std::unordered_map<std::string, int> curAnimtionIdx;
};