#pragma once

namespace Engine {

	class ImGuiLayer
	{
		static void Init(int64_t windowHandle);
		static void ShutDown();
		friend class ModuleCore;

	public:
		static void Begin();
		static void End();
	};

}
