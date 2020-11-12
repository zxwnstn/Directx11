#pragma once

class Texture
{
public:
	Texture(const std::string& path);

	void Bind() const;

private:
	int32_t Width;
	int32_t Height;
	int32_t Channels;

	ID3D11Texture2D* Buffer;
	ID3D11ShaderResourceView* View;
};

class TextureArchive
{

};