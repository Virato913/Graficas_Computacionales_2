#pragma once

#include "GraphicsBuffer.h"

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	VertexBuffer<VertexType> myVertices;
	IndexBuffer<unsigned int> myIndices;
	int m_vertexCount, m_indexCount;

	bool InitializeBuffers(ID3D11Device* device)
	{		
		// Set the number of vertices in the vertex array.
		m_vertexCount = 6;

		// Set the number of indices in the index array.
		m_indexCount = 9;

		// Create the vertex array.
		myVertices.reserve(m_vertexCount);

		// Create the index array.
		myIndices.reserve(m_indexCount);

		VertexType* vertices = new VertexType();

		// Load the vertex array with data.
		vertices->position = XMFLOAT3(0.0f, sqrt(0.75f), 0.0f);  // Bottom left. 0
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		
		myVertices.add(*vertices);

		vertices->position = XMFLOAT3(-0.5f, 0.0f, 0.0f);  // Top middle. 1
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		
		myVertices.add(*vertices);

		vertices->position = XMFLOAT3(0.5f, 0.0f, 0.0f);  // Bottom right.2
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		
		myVertices.add(*vertices);

		vertices->position = XMFLOAT3(-1.0f, -sqrt(0.75f), 0.0f);  // Bottom left. 0
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		
		myVertices.add(*vertices);

		vertices->position = XMFLOAT3(0.0f, -sqrt(0.75f), 0.0f);  // Top middle. 1
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		
		myVertices.add(*vertices);

		vertices->position = XMFLOAT3(1.0f, -sqrt(0.75f), 0.0f);  // Bottom right.2
		vertices->color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

		myVertices.add(*vertices);

		unsigned int* indices = new unsigned int;

		// Load the index array with data.
		*indices = 0;  // Bottom left.

		myIndices.add(*indices);

		*indices = 2;  // Top middle.

		myIndices.add(*indices);

		*indices = 1;  // Bottom right.

		myIndices.add(*indices);

		*indices = 1;

		myIndices.add(*indices);

		*indices = 4;  // Bottom right.

		myIndices.add(*indices);

		*indices = 3;

		myIndices.add(*indices);

		*indices = 2;  // Bottom right.

		myIndices.add(*indices);

		*indices = 5;

		myIndices.add(*indices);

		*indices = 4;  // Bottom right.

		myIndices.add(*indices);

		myVertices.createHardwareBuffer(device);

		myIndices.createHardwareBuffer(device);

		// Release the arrays now that the vertex and index buffers have been created and loaded.
		delete[] vertices;
		vertices = nullptr;

		delete[] indices;
		indices = nullptr;

		return true;
	}

	void ShutdownBuffers()
	{
		// Release the index buffer.
		if (myIndices.m_pBuffer)
		{
			myIndices.clear();
			myIndices.m_pBuffer = nullptr;
		}

		// Release the vertex buffer.
		if (myVertices.m_pBuffer)
		{
			myVertices.clear();
			myVertices.m_pBuffer = nullptr;
		}

		return;
	}

	void RenderBuffers(ID3D11DeviceContext* deviceContext)
	{
		unsigned int stride;
		unsigned int offset;


		// Set vertex buffer stride and offset.
		stride = sizeof(VertexType);
		offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &myVertices.m_pBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(myIndices.m_pBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return;
	}

public:
	Model()
	{
		myVertices.m_pBuffer = nullptr;
		myIndices.m_pBuffer = nullptr;
	}

	~Model()
	{

	}

	bool Initialize(ID3D11Device* device)
	{
		bool result;


		// Initialize the vertex and index buffers.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void Shutdown()
	{
		// Shutdown the vertex and index buffers.
		ShutdownBuffers();
	}

	void Render(ID3D11DeviceContext* deviceContext)
	{
		// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
		RenderBuffers(deviceContext);
	}

	int GetIndexCount()
	{
		return m_indexCount;
	}
};

