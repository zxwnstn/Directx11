#pragma once

class Texture
{
public:
	Texture(const std::string& path);

	void Bind(int slot) const;

private:
	int32_t Width;
	int32_t Height;
	int32_t Channels;

	ID3D11Texture2D* Buffer;
	ID3D11ShaderResourceView* View;
};

class TextureArchive
{
public:
	static void Add(const std::string& path, const std::string& name);
	static bool Has(const std::string& name);
	static std::shared_ptr<Texture> Get(const std::string& name);
	static void Shudown();
};