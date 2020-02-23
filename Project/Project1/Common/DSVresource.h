#pragma once

#include "d3dUtil.h"
#include "ResourceBase.h"

class DSVresource : public ResourceBase 
{

public:
	DSVresource();
	DSVresource(ID3D12Device *device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
	~DSVresource();
	

	void Set(ID3D12Device *device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);

};