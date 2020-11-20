#include "pch.h"

#include "Texture.h"
#include "Renderer/Dx11Core.h"
#include "Common/Material.h"

namespace Engine {
	Texture::Texture(const std::string & path, int mySlot)
		: MySlot(mySlot), isTextureArray(false)
	{
		LOG_MISC("Texture::Create {0} texture", path);

		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &Width, &Height, &Channels, 4);
		uint32_t rowPitch = Width * 4 * sizeof(unsigned char);

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = Width;
		textureDesc.Height = Height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &Buffer);
		ASSERT(Buffer, "Texture::Create texture failed");

		Dx11Core::Get().Context->UpdateSubresource(Buffer, 0, NULL, data, rowPitch, 0);
		stbi_image_free(data);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		Dx11Core::Get().Device->CreateShaderResourceView(Buffer, &srvDesc, &View);
		Dx11Core::Get().Context->GenerateMips(View);
		ASSERT(View, "Texture::Create texture view failed");
	}

	Texture::Texture(const std::vector<std::string>& paths, int mySlot)
		: MySlot(mySlot), isTextureArray(true)
	{
		//LOG_MISC("Texture::Create texture array");

		//int index = 0;
		//for (size_t i = 0; i < paths.size(); ++i)
		//{
		//	
		//	stbi_set_flip_vertically_on_load(1);
		//	stbi_uc* data[30];
		//	LOG_MISC("{0}",paths[i]);
		//	data[i] = stbi_load(paths[i].c_str(), &Width, &Height, &Channels, 4);
		//	if (Width != 1024) continue;

		//	if (Buffer == nullptr)
		//	{
		//		D3D11_TEXTURE2D_DESC textureDesc;
		//		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//		textureDesc.ArraySize = 21;
		//		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		//		textureDesc.CPUAccessFlags = 0;
		//		textureDesc.Width = 1024;
		//		textureDesc.Height = 1024;
		//		textureDesc.MipLevels = 0;
		//		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		//		textureDesc.SampleDesc.Count = 1;
		//		textureDesc.SampleDesc.Quality = 0;
		//		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		//		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &Buffer);
		//		ASSERT(Buffer, "Texture::Create texture failed");
		//	}

		//	ID3D11Texture2D* temp;
		//	D3D11_TEXTURE2D_DESC textureDesc;
		//	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	textureDesc.ArraySize = 1;
		//	textureDesc.BindFlags = 0;
		//	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		//	textureDesc.Width = Width;
		//	textureDesc.Height = Height;
		//	textureDesc.MipLevels = 0;
		//	textureDesc.MiscFlags = 0;
		//	textureDesc.SampleDesc.Count = 1;
		//	textureDesc.SampleDesc.Quality = 0;
		//	textureDesc.Usage = D3D11_USAGE_STAGING;
		//	Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &temp);
		//	Dx11Core::Get().Context->UpdateSubresource(temp, 0, nullptr, data[i], Width * 4 * 4, 0);

		//	D3D11_MAPPED_SUBRESOURCE mapped;
		//	Dx11Core::Get().Context->Map(temp, 0, D3D11_MAP_READ, 0, &mapped);
		//	
		//	Dx11Core::Get().Context->UpdateSubresource(Buffer, index, nullptr, mapped.pData, mapped.RowPitch, 0);
		//	Dx11Core::Get().Context->Unmap(temp, 0);

		//	index++;

		//	//stbi_image_free(data);
		//	temp->Release();
		//}

		//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		//ZeroMemory(&srvDesc, sizeof(srvDesc));
		//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		//srvDesc.Texture2DArray.ArraySize = 21;
		//srvDesc.Texture2DArray.FirstArraySlice = 0;
		//srvDesc.Texture2DArray.MipLevels = 1;
		//srvDesc.Texture2DArray.MostDetailedMip = 0;

		//Dx11Core::Get().Device->CreateShaderResourceView(Buffer, &srvDesc, &View);
		//Dx11Core::Get().Context->GenerateMips(View);

		//ASSERT(View, "Texture::Create texture array view failed");
		//LOG_MISC("Texture::Create texture array done");
	}

	void Texture::Bind(int slot) const
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &View);
	}

	void Texture::Bind() const
	{
		Dx11Core::Get().Context->PSSetShaderResources(MySlot, 1, &View);
	}

	void Texture::MultipleTextureBind(const std::vector<std::string>& textures, int slot)
	{
		static ID3D11ShaderResourceView* views[100];

		for (size_t i = 0; i < textures.size(); ++i)
		{
			views[i] = TextureArchive::Get(textures[i])->View;
		}
		Dx11Core::Get().Context->PSSetShaderResources(slot, (uint32_t)textures.size(), views);
	}

	static std::unordered_map<std::string, std::shared_ptr<Texture>> s_Textures;


	void TextureArchive::Add(const std::string & path, const std::string & name, int slot)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(path, slot));
	}

	void TextureArchive::Add(const std::vector<std::string>& paths, const std::string & name, int slot)
	{
		if (Has(name)) return;

		s_Textures[name].reset(new Texture(paths, slot));
	}

	bool TextureArchive::Has(const std::string & name)
	{
		auto find = s_Textures.find(name);
		return find != s_Textures.end();
	}

	std::shared_ptr<Texture> TextureArchive::Get(const std::string & name)
	{
		if (!Has(name)) return nullptr;
		return s_Textures[name];
	}

	void TextureArchive::Shudown()
	{
		s_Textures.clear();
	}

}
