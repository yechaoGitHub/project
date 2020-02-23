#pragma once

#include "d3dUtil.h"
#include "ResourceBase.h"

class ReadBackResource : public ResourceBase
{
public:
	ReadBackResource();
	ReadBackResource(ID3D12Device *device, UINT length);
	ReadBackResource(ID3D12Device *device, ID3D12Resource *bindResource);
	~ReadBackResource();

	void Create(ID3D12Device *device, UINT length);
	void BindResource(ID3D12Device *device, ID3D12Resource *bindResource);
	void Bind(ID3D12Resource *res);
	
	virtual void* Map(UINT subresource, D3D12_RANGE* range = nullptr) override;
	virtual void Unmap(UINT subresource, D3D12_RANGE* range = nullptr) override;
};