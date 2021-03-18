#include "pch.h"
#include "FileCommon.h"
#include "Common/Timestep.h"
#include <filesystem>
#include <shobjidl.h> 
#include "Renderer/Dx11Core.h"

namespace Engine::File {

	bool isExistFile(const std::string & filename)
	{
		return std::filesystem::is_regular_file(filename);
	}

	bool isExistDirectroy(const std::string & path)
	{
		return std::filesystem::is_directory(path);
	}

	void CreateDir(const std::string & path)
	{
		std::filesystem::create_directories(path);
	}

	void CreateFile_(const std::string & path)
	{
		std::ofstream ofs(path);
		ofs.close();
	}


	void TryCreateDir(const std::string& path)
	{
		if (isExistFile(path) || isExistDirectroy(path))
			return;
		CreateDir(path);
	}

	void TryCreateFile(const std::string& file)
	{
		if (isExistFile(file) || isExistDirectroy(file))
			return;

		size_t point = file.rfind('/');
		auto dir = file.substr(0, point);
		TryCreateDir(dir);
		CreateFile_(file);
	}

	bool complete = false;
	bool runThread = false;
	std::wstring result;

	void SaveOfn(const std::string path)
	{
		std::wstring w;
		w.assign(path.begin(), path.end());

		OPENFILENAME OFN;
		ZeroMemory(&OFN, sizeof(OFN));
		OFN.lStructSize = sizeof(OPENFILENAME);
		wchar_t file[256]{ 0, };
		OFN.lpstrFile = file;
		OFN.nMaxFile = 256;
		OFN.lpstrFilter = L"Scene File\0*.scene\0";
		OFN.lpstrInitialDir = w.c_str();
		GetSaveFileName(&OFN);

		runThread = false;
		complete = true;

		result = file;
	}

	void ReadOfn(const std::string path)
	{
		std::wstring w;
		w.assign(path.begin(), path.end());

		OPENFILENAME OFN;
		ZeroMemory(&OFN, sizeof(OFN));
		OFN.lStructSize = sizeof(OPENFILENAME);
		wchar_t file[256]{ 0, };
		OFN.lpstrFile = file;
		OFN.nMaxFile = 256;
		OFN.lpstrFilter = L"Scene File\0*.scene\0";
		OFN.lpstrInitialDir = w.c_str();
		GetOpenFileName(&OFN);

		runThread = false;
		complete = true;

		result = file;
	}

	
	void OpenSaveFileDialog(const std::string & path)
	{
		if (runThread) return;
		
		runThread = true;
		complete = false;

		std::thread t(SaveOfn, path);
		t.detach();
	}

	void OpenReadFileDialog(const std::string & path)
	{
		if (runThread) return;

		runThread = true;
		complete = false;

		std::thread t(ReadOfn, path);
		t.detach();
	}

	std::string GetDialogResult()
	{
		if (!complete) return "";

		std::string ret;
		ret.assign(result.begin(), result.end());
		return ret;
	}


	std::string GetCommonPath(CommonPathType pathType)
	{
		switch (pathType)
		{
		case File::Assets:
			return "../../Engine/assets/";
		case File::Texture:
			return "../../Engine/assets/Texture/";
		case File::FBX:
			return "../../Engine/assets/Fbx/";
		case File::Obj:
			return "../../Engine/assets/Obj/";
		case File::FBXCache:
			return "../../Engine/assets/FbxCache/";
		case File::Shader:
			return "../../Engine/assets/Shader/";
		case File::Log:
			return "../../Log/" + Time::Year_Mon_Day();
		case File::Scene:
			return "../../Engine/assets/Scene/";
		}
		return "";
	}

}

