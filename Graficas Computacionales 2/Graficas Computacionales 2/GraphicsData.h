#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include "Texture.h"

using namespace DirectX;
using namespace std;

class GraphicsData
{
private:
	int width;
	int height;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	//ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	Texture m_depthStencilBuffer;

public:
	GraphicsData()
	{
		m_swapChain = 0;
		m_device = 0;
		m_deviceContext = 0;
		m_renderTargetView = 0;
		//m_depthStencilBuffer = 0;
		m_depthStencilView = 0;
	}

	~GraphicsData()
	{

	}

	bool Initialize(HWND hWnd, float screenDepth, float screenNear)
	{
		HRESULT hr = S_OK;

		//Obtenemos el tamaño de la ventana
		RECT rc;
		GetClientRect(hWnd, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;

		unsigned int createDeviceFlags = 0;
#if defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		vector<D3D_DRIVER_TYPE> driverTypes;
		driverTypes.push_back(D3D_DRIVER_TYPE_HARDWARE);
		driverTypes.push_back(D3D_DRIVER_TYPE_WARP);
		driverTypes.push_back(D3D_DRIVER_TYPE_REFERENCE);

		vector<D3D_FEATURE_LEVEL> featureLevels;
		featureLevels.push_back(D3D_FEATURE_LEVEL_11_0);
		featureLevels.push_back(D3D_FEATURE_LEVEL_10_1);
		featureLevels.push_back(D3D_FEATURE_LEVEL_10_0);

		DXGI_SWAP_CHAIN_DESC sd;
		memset(&sd, 0, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;

		D3D_FEATURE_LEVEL selectedFL;

		for (size_t driverTypeIndex = 0; driverTypeIndex < driverTypes.size(); ++driverTypeIndex)
		{
			D3D_DRIVER_TYPE& dt = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(nullptr, dt, nullptr, createDeviceFlags, &featureLevels[0], static_cast<UINT>(featureLevels.size()), D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, &selectedFL, &m_deviceContext);
			if (SUCCEEDED(hr))
			{
				break;
			}
		}

		if (FAILED(hr))
		{
			return false;
		}

		//Creamos nuestro render target view
		ID3D11Texture2D* pbackBuffer = nullptr;
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pbackBuffer));
		if (FAILED(hr))
		{
			return false;
		}

		hr = m_device->CreateRenderTargetView(pbackBuffer, nullptr, &m_renderTargetView);
		pbackBuffer->Release();
		if (FAILED(hr))
		{
			return false;
		}

		////Creamos la textura de Depth Stencil
		//D3D11_TEXTURE2D_DESC descDepth;
		//memset(&descDepth, 0, sizeof(descDepth));
		//descDepth.Width = width;
		//descDepth.Height = height;
		//descDepth.MipLevels = 1;
		//descDepth.ArraySize = 1;
		//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//descDepth.SampleDesc.Count = 1;
		//descDepth.SampleDesc.Quality = 0;
		//descDepth.Usage = D3D11_USAGE_DEFAULT;
		//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//descDepth.CPUAccessFlags = 0;
		//descDepth.MiscFlags = 0;

		/*hr = m_device->CreateTexture2D(&descDepth, nullptr, &m_depthStencilBuffer);
		if (FAILED(hr))
		{
			return false;
		}*/

		if (!m_depthStencilBuffer.createTexture(m_device, TEXTURE_TYPE::DEPTH, width, height))
		{
			return false;
		}

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

		// Initialize the description of the stencil state.
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(hr))
		{
			return false;
		}

		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		memset(&descDSV, 0, sizeof(descDSV));

		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.getTexture(), &descDSV, &m_depthStencilView);
		if (FAILED(hr))
		{
			return false;
		}

		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

		D3D11_RASTERIZER_DESC rasterDesc;

		// Setup the raster description which will determine how and what polygons will be drawn.
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		if (FAILED(hr))
		{
			return false;
		}

		// Now set the rasterizer state.
		m_deviceContext->RSSetState(m_rasterState);

		D3D11_VIEWPORT viewport;

		// Setup the viewport for rendering.
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// Create the viewport.
		m_deviceContext->RSSetViewports(1, &viewport);


		float fieldOfView, screenAspect;

		// Setup the projection matrix.
		fieldOfView = 3.141592654f / 4.0f;
		screenAspect = (float)width / (float)height;

		// Create the projection matrix for 3D rendering.
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

		// Initialize the world matrix to the identity matrix.
		m_worldMatrix = XMMatrixIdentity();

		// Create an orthographic projection matrix for 2D rendering.
		m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, screenNear, screenDepth);

		return true;
	}

	void Shutdown()
	{
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(false, NULL);
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = 0;
		}

		if (m_depthStencilBuffer.getTexture())
		{
			m_depthStencilBuffer.clear();
			//m_depthStencilBuffer = 0;
		}

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = 0;
		}

		if (m_deviceContext)
		{
			m_deviceContext->Release();
			m_deviceContext = 0;
		}

		if (m_device)
		{
			m_device->Release();
			m_device = 0;
		}

		if (m_swapChain)
		{
			m_swapChain->Release();
			m_swapChain = 0;
		}

		return;
	}

	void BeginScene()
	{
		float color[4] = { 0.4f,0.4f,1.0f,1.0f };
		m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

		// Clear the depth buffer.
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void EndScene()
	{
		m_swapChain->Present(0, 0);
	}

	ID3D11Device* GetDevice()
	{
		return m_device;
	}

	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_deviceContext;
	}

	void GetProjectionMatrix(XMMATRIX& projectionMatrix)
	{
		projectionMatrix = m_projectionMatrix;
		return;
	}

	void GetWorldMatrix(XMMATRIX& worldMatrix)
	{
		worldMatrix = m_worldMatrix;
		return;
	}

	void GetOrthoMatrix(XMMATRIX& orthoMatrix)
	{
		orthoMatrix = m_orthoMatrix;
		return;
	}
};

