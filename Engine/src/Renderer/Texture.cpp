#include "pch.h"

#include "Texture.h"
#include "Renderer/Dx11Core.h"

namespace Engine {

	
	Texture::Texture(const std::string& path, UsageType type, int mySlot)
		: MySlot(mySlot)
		, type(type)
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

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		Dx11Core::Get().Device->CreateShaderResourceView(Buffer, &srvDesc, &View);
		Dx11Core::Get().Context->GenerateMips(View);
		ASSERT(View, "Texture::Create texture view failed");

		LOG_MISC("Texture::Create texture done", path);
	}

	void Texture::Bind(int slot) const
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &View);
	}

	void Texture::Bind() const
	{
		Dx11Core::Get().Context->PSSetShaderResources(MySlot, 1, &View);
	}

	void Texture::MultipleBind(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		static ID3D11ShaderResourceView* views[100];
		for (size_t i = 0; i < textures.size(); ++i)
			views[i] = textures[i]->View;

		Dx11Core::Get().Context->PSSetShaderResources(0, (unsigned int)textures.size(), views);
	}

	static std::unordered_map<std::string, std::shared_ptr<Texture>> s_Textures;

	void TextureArchive::Add(const std::string & path, const std::string & name, Texture::UsageType type, int slot)
	{
		if (Has(name)) return;

		s_Textures[name] = std::make_shared<Texture>(path, type, slot);
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
