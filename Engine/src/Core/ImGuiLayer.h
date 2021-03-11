#pragma once

namespace Engine {

	class ImGuiLayer
	{
		static void Init(int windowHandle);
		static void ShutDown();
		friend class ModuleCore;

	public:
		static void Begin();
		static void End();
	};

}
