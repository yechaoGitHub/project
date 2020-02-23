#pragma once

#include "d3dUtil.h"
#include "ReadBackResource.h"
#include "UpLoadResource.h"
#include "TextureResource.h"

class RWtexture 
{
public:
	RWtexture();
	RWtexture(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RWtexture();

	void Set(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	D3D12_RESOURCE_DESC GetDesc();
	void WriteData(void* data, UINT64 begin, UINT64 size);
	void* ReadMap(UINT subresource, D3D12_RANGE *range = nullptr);
	void ReadUnmap(UINT subresource, D3D12_RANGE *range = nullptr);

	ID3D12Resource* GetRBresource();
	ID3D12Resource* GetULresource();
	ID3D12Resource* GetTextureresource();

private:

	TextureResource		texResource;
	ReadBackResource	rbResource;
	UpLoadResource		ulResource;
};