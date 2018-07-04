#pragma once

#include <d3d11.h>
#include <vector>

using std::vector;

class InputLayout
{
private:
	ID3D11InputLayout* m_inputLayout;

public:
	vector<D3D11_INPUT_ELEMENT_DESC> m_descArray;

	InputLayout()
	{
		m_inputLayout = nullptr;
	}

	~InputLayout()
	{
		m_descArray.clear();
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}

	void reserve(size_t numObjects)
	{
		m_descArray.reserve(numObjects);
	}

	void add(LPCSTR SemanticName, UINT SemanticIndex, DXGI_FORMAT Format, UINT InputSlot, UINT AlignedByteOffset, D3D11_INPUT_CLASSIFICATION InputSlotClass, UINT InstanceDataStepRate)
	{
		D3D11_INPUT_ELEMENT_DESC ilDesc;
		ilDesc.SemanticName = SemanticName;
		ilDesc.SemanticIndex = SemanticIndex;
		ilDesc.Format = Format;
		ilDesc.InputSlot = InputSlot;
		ilDesc.AlignedByteOffset = AlignedByteOffset;
		ilDesc.InputSlotClass = InputSlotClass;
		ilDesc.InstanceDataStepRate = InstanceDataStepRate;
		m_descArray.push_back(ilDesc);
	}

	void add(std::vector<D3D11_INPUT_ELEMENT_DESC>& constants)
	{
		m_descArray.insert(m_descArray.end(), constants.begin(), constants.end());
	}
	
	void add(D3D11_INPUT_ELEMENT_DESC* pConstants, size_t numConst)
	{
		m_descArray.insert(m_descArray.end(), pConstants, pConstants + numConst);
	}
	
	void clear()
	{
		m_descArray.clear();
	}
	
	void createHardwareLayout(ID3D11Device* pd3dDevice, LPVOID bufferPointer, SIZE_T bufferSize)
	{
		HRESULT hr = pd3dDevice->CreateInputLayout(&m_descArray[0], m_descArray.size(), bufferPointer, bufferSize, &m_inputLayout);
		if (FAILED(hr))
		{
			throw std::exception("Failed to create Input Layout.");
		}
	}
};

