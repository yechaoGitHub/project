#pragma once

#include "d3dUtil.h"
#include "ResourceBase.h"

class TextureResource : public ResourceBase
{
public:
	TextureResource();
	TextureResource(ID3D12Device* device, D3D12_RESOURCE_DESC* desc, D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATES intitState = D3D12_RESOURCE_STATE_PRESENT);
	TextureResource(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~TextureResource();

	void Create(ID3D12Device* device, D3D12_RESOURCE_DESC* desc, D3D12_HEAP_FLAGS flag = D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATES intitState = D3D12_RESOURCE_STATE_PRESENT);
	void Create(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

private:

};