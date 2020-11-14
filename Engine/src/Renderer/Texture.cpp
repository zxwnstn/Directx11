#include "pch.h"

#include "Texture.h"
#include "Renderer/Dx11Core.h"

namespace Engine {

	DXGI_FORMAT GetDxFormat(int32_t channels)
	{
		if (channels == 3) return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		if (channels == 4) return DXGI_FORMAT_R8G8B8A8_UNORM;
		return DXGI_FORMAT_UNKNOWN;
	}

	Texture::Texture(const std::string& path)
	{
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = nullptr;
		data = stbi_load(path.c_str(), &Width, &Height, &Channels, 0);
		uint32_t rowPitch = (Width * Channels) * sizeof(unsigned char);

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = Width;
		textureDesc.Height = Height;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = GetDxFormat(Channels);
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		Dx11Core::Get().Device->CreateTexture2D(&textureDesc, NULL, &Buffer);

		Dx11Core::Get().Context->UpdateSubresource(Buffer, 0, NULL, data, rowPitch, 0);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		Dx11Core::Get().Device->CreateShaderResourceView(Buffer, &srvDesc, &View);
		Dx11Core::Get().Context->GenerateMips(View);
	}

	void Texture::Bind(int slot) const
	{
		Dx11Core::Get().Context->PSSetShaderResources(slot, 1, &View);
	}


	static std::unordered_map<std::string, std::shared_ptr<Texture>> s_Textures;

	void TextureArchive::Add(const std::string & path, const std::string & name)
	{
		if (Has(name)) return;

		s_Textures[name] = std::make_shared<Texture>(path);
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
