#pragma once

namespace Engine {

	class ObjLoader 
	{
	public:
		ObjLoader(const std::filesystem::path& path);
		void extract();

	};

}