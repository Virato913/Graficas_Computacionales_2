#pragma once

#include <d3d11.h>
#include <exception>

enum class TEXTURE_TYPE
{
	TEXTURE2D,
	BUFFER,
	DEPTH
};

class Texture
{
public:
	Texture()
	{
		m_Texture = nullptr;
	}

	~Texture()
	{
		this->clear();
	}

	void clear()
	{
		m_Texture->Release();
	}

	bool createTexture(ID3D11Device* device, TEXTURE_TYPE type, int width, int height)
	{
		HRESULT hr = S_OK;
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(desc));
		switch (type)
		{
		case TEXTURE_TYPE::TEXTURE2D:

			break;
		case TEXTURE_TYPE::BUFFER:

			break;
		case TEXTURE_TYPE::DEPTH:
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			hr = device->CreateTexture2D(&desc, nullptr, &m_Texture);
			if (FAILED(hr))
			{
				return false;
			}
			break;
		default:
			break;
		}
		return true;
	}

	ID3D11Texture2D* getTexture()
	{
		return m_Texture;
	}

private:
	ID3D11Texture2D* m_Texture;
};