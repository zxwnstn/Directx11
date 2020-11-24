#pragma once

namespace Engine {

	class ObjLoader 
	{
	public:
		ObjLoader(const std::filesystem::path& path);
		void Extract();

		void ReadMaterial(const std::string& mtlfile);

	private:
		std::ifstream stream;
		std::string filename;
		std::string curpath;

		std::unordered_map<std::string, int> mappedMaterialName;
	};

}