#pragma once

#include "d3dUtil.h"

class ResourceBase
{
public:
	ResourceBase();
	virtual ~ResourceBase();
	ResourceBase(const ResourceBase&) = delete;
	void operator= (const ResourceBase&) = delete;
	
	virtual void Bind(ID3D12Resource *src);
	virtual void Release();
	virtual ID3D12Resource* Resource() const;
	virtual D3D12_RESOURCE_DESC GetDesc() const;
	virtual D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress();
	virtual void SetName(const WCHAR* name);
	virtual const WCHAR* GetName();
	virtual void GetFootPrint(D3D12_PLACED_SUBRESOURCE_FOOTPRINT *footPrint, UINT *rowNums, UINT64 *rowSizeInBytes, UINT64 *totalSizeInBytes, UINT subresourceIndex = 0, UINT number = 0, UINT64 offset = 0);
	operator ID3D12Resource* ();
	
protected:
	ID3D12Resource							*mSrc;
	bool									mbCreate;
	D3D12_CPU_DESCRIPTOR_HANDLE				mCpuHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE				mGpuHandle;
	std::wstring							mName;
	
	virtual void* Map(UINT subresource, D3D12_RANGE* range = nullptr);
	virtual void Unmap(UINT subresource, D3D12_RANGE* range = nullptr);
	
};
