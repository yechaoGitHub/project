#pragma once

#include "d3dUtil.h"
#include "ResourceBase.h"

class UpLoadResource : public ResourceBase
{
public:
	UpLoadResource();
	UpLoadResource(ID3D12Device *device, UINT length, bool persistented = false);
	UpLoadResource(ID3D12Device *device, ID3D12Resource* bindResource, bool persistented = false);
	~UpLoadResource();

	void Create(ID3D12Device* device, UINT64 length);
	void BindResource(ID3D12Device *device, ID3D12Resource *bindResource);
	void Bind(ID3D12Resource *res) override;
	void* Map();
	void CopyData(void *data, UINT64 start, UINT64 size);
	void Unmap();
	
private:
	BYTE			*mMapData;
	UINT			mMapCount;
};

