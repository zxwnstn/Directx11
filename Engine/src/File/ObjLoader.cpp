#include "pch.h"

#include "ObjLoader.h"
#include "Common/Mesh.h"
#include "Common/Material.h"
#include "Renderer/Texture.h"

namespace Engine {

	ObjLoader::ObjLoader(const std::filesystem::path & path)
		: stream(path)
	{
		auto filepath = path;

		filename = path.stem().string();
		curpath = filepath.remove_filename().string();
		MeshArchive::AddStaticMesh(filename);
		MaterialArchive::AddSet(filename);
	}

	void ObjLoader::Extract()
	{
		LOG_TRACE("ObjLoader::Extract {0}", filename) {

			ENABLE_ELAPSE
			
			std::string tokken;
			std::vector<vec3> positions;
			std::vector<vec2> uvs;
			std::vector<vec3> normals;

			auto mesh = MeshArchive::GetStaticMesh(filename);
			auto& vertices = mesh->Vertices;
			mesh->Indices = MeshArchive::GetSerialIndices();

			int curMaterial = -1;

			while (stream.peek() != -1)
			{
				stream >> tokken;

				if (tokken == "#")
				{
					std::getline(stream, tokken);
					continue;
				}

				if (tokken == "o")
				{
					std::getline(stream, tokken);
					continue;
				}

				if (tokken == "mtllib")
				{
					stream >> tokken;
					ReadMaterial(tokken);
					continue;
				}

				if (tokken == "usemtl")
				{
					stream >> tokken;
					curMaterial = mappedMaterialName[tokken];
					continue;
				}

				if (tokken == "v")
				{
					vec3 position;
					stream >> position.x;
					stream >> position.y;
					stream >> position.z;
					positions.emplace_back(position);
					continue;
				}

				if (tokken == "vt")
				{
					vec2 uv;
					stream >> uv.x;
					stream >> uv.y;
					uvs.emplace_back(uv);
					continue;
				}

				if (tokken == "vn")
				{
					vec3 normal;
					stream >> normal.x;
					stream >> normal.y;
					stream >> normal.z;
					normals.emplace_back(normal);
					continue;
				}

				if (tokken == "f")
				{
					Vertex vert[3];

					for (int i = 0; i < 3; ++i)
					{
						stream >> tokken;

						std::stringstream ss(tokken);
						std::getline(ss, tokken, '/');
						if (tokken != "") vert[i].Position = positions[std::atoi(tokken.c_str()) - 1];
						std::getline(ss, tokken, '/');
						if (tokken != "") vert[i].UV = uvs[std::atoi(tokken.c_str()) - 1];
						std::getline(ss, tokken, '/');
						if (tokken != "") vert[i].Normal = normals[std::atoi(tokken.c_str()) - 1];
						vert[i].MaterialIndex = curMaterial;

					}
					auto[tan, binorm] = Util::GetTangentAndBinomal(vert[0].Position, vert[1].Position, vert[2].Position,
						vert[0].UV, vert[1].UV, vert[2].UV);

					vert[0].Tangent = tan;
					vert[1].Tangent = tan;
					vert[2].Tangent = tan;
					vert[0].BiNormal = binorm;
					vert[1].BiNormal = binorm;
					vert[2].BiNormal = binorm;

					vertices.emplace_back(vert[0]);
					vertices.emplace_back(vert[1]);
					vertices.emplace_back(vert[2]);
					continue;
				}
			}
			mesh->IndiceCount = (uint32_t)vertices.size();

			LOG_ELAPSE
		}
		stream.close();
	}

	void ObjLoader::ReadMaterial(const std::string& mtlfile)
	{
		std::ifstream matStream(curpath + mtlfile);
		if (!matStream.is_open())
		{
			LOG_WARN("There is no {}'s material file!", filename);
			return;
		}
		auto material = MaterialArchive::GetSet(filename);
		auto& materials = material->Materials;
		auto& materialTestures = material->MaterialTextures;

		std::string tokken;

		int i = -1;
		while (matStream.peek() != -1)
		{
			matStream >> tokken;

			if (tokken == "#")
			{
				std::getline(matStream, tokken);
				continue;
			}

			if (tokken == "newmtl")
			{
				++i;
				matStream >> tokken;
				mappedMaterialName[tokken] = i;
				MaterialTextureInfo info;
				materialTestures[i].push_back(info);
				materialTestures[i].push_back(info);
				materialTestures[i].push_back(info);
				continue;
			}

			if (tokken == "Ns")
			{
				matStream >> materials[i].Shiness;
				continue;
			}

			if (tokken == "Ka")
			{
				matStream >> materials[i].Ambient.x;
				matStream >> materials[i].Ambient.y;
				matStream >> materials[i].Ambient.z;
				continue;
			}

			if (tokken == "Kd")
			{
				matStream >> materials[i].Diffuse.x;
				matStream >> materials[i].Diffuse.y;
				matStream >> materials[i].Diffuse.z;
				continue;
			}

			if (tokken == "Ks")
			{
				matStream >> materials[i].Specular.x;
				matStream >> materials[i].Specular.y;
				matStream >> materials[i].Specular.z;
				continue;
			}

			if (tokken == "Ke")
			{
				matStream >> materials[i].Emissive.x;
				matStream >> materials[i].Emissive.y;
				matStream >> materials[i].Emissive.z;
				continue;
			}

			if (tokken == "Ni")
			{
				matStream >> tokken;
				continue;
			}


			if (tokken == "illum")
			{
				matStream >> tokken;
				continue;
			}

			if (tokken == "map_Kd")
			{
				matStream >> tokken;
				std::filesystem::path temp(tokken);
				materialTestures[i][0].Path = curpath + tokken;
				materialTestures[i][0].Name = temp.stem().string();
				materials[i].MapMode |= 1;
				TextureArchive::Add(materialTestures[i][0].Path, materialTestures[i][0].Name);
				continue;
			}

			if (tokken == "map_Bump")
			{
				matStream >> tokken;
				if (tokken == "-bm")
				{
					matStream >> tokken;
					matStream >> tokken;
				}
				std::filesystem::path temp(tokken);
				materialTestures[i][1].Path = curpath + tokken;
				materialTestures[i][1].Name = temp.stem().string();
				materials[i].MapMode |= 2;
				TextureArchive::Add(materialTestures[i][1].Path, materialTestures[i][1].Name);
				continue;
			}

			if (tokken == "map_Ks")
			{
				matStream >> tokken;
				std::filesystem::path temp(tokken);
				materialTestures[i][2].Path = curpath + tokken;
				materialTestures[i][2].Name = temp.stem().string();
				materials[i].MapMode |= 4;
				TextureArchive::Add(materialTestures[i][2].Path, materialTestures[i][2].Name);
				continue;
			}
		}

	}

}