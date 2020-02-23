#pragma once

#include "d3dUtil.h"
#include "ResourceBase.h"

class RTVresource : public ResourceBase
{
public:
	RTVresource();
	RTVresource(ID3D12Device *device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RTVresource();

	void Set(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

};