#include "./ResourceBase.h"

ResourceBase::ResourceBase() :
	mSrc(nullptr),
	mbCreate(false),
	mCpuHandle({0}),
	mGpuHandle({0})
{

}

ResourceBase::~ResourceBase()
{
	if (mSrc && mbCreate)
	{
		Release();
	}
}

void ResourceBase::Bind(ID3D12Resource *src)
{
	mSrc = src;
}

void ResourceBase::Release()
{
	if (mSrc != nullptr) 
	{
		mSrc->Release();
		mSrc = nullptr;
	}
	
	mbCreate = false;
	mCpuHandle = { 0 };
	mGpuHandle = { 0 };
	mName = L"";

}

ID3D12Resource* ResourceBase::Resource() const
{
	return mSrc;
}

ResourceBase::operator ID3D12Resource* ()
{
	return mSrc;
}

D3D12_RESOURCE_DESC ResourceBase::GetDesc() const
{
	return mSrc->GetDesc();
}

D3D12_GPU_VIRTUAL_ADDRESS ResourceBase::GetGpuAddress()
{
	return mSrc->GetGPUVirtualAddress();
}

void ResourceBase::SetName(const WCHAR* name)
{
	if (name != nullptr)
	{
		mName = name;
		mSrc->SetName(name);
	}
	else
	{
		UINT size = 0;
		ThrowIfFailed(mSrc->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, nullptr));
	}
}

const WCHAR* ResourceBase::GetName()
{
	UINT size = mName.size();

	ThrowIfFailed(mSrc->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, (void*)mName.data()));
}

void ResourceBase::GetFootPrint(D3D12_PLACED_SUBRESOURCE_FOOTPRINT *footPrint, UINT *rowNums, UINT64 *rowSizeInBytes, UINT64 *totalSizeInBytes, UINT subresourceIndex, UINT number, UINT64 offset)
{
	ID3D12Device *device = nullptr;
	ThrowIfFailed(mSrc->GetDevice(IID_PPV_ARGS(&device)));
	D3D12_RESOURCE_DESC desc = mSrc->GetDesc();

	device->GetCopyableFootprints(&desc, subresourceIndex, number, offset, footPrint, rowNums, rowSizeInBytes, totalSizeInBytes);
	device->Release();
}

void* ResourceBase::Map(UINT subresource, D3D12_RANGE* range)
{
	void* data = nullptr;
	ThrowIfFailed(mSrc->Map(subresource, range, reinterpret_cast<void**>(&data)));
	return data;
}

void ResourceBase::Unmap(UINT subresource, D3D12_RANGE* range)
{
	mSrc->Unmap(subresource, range);
}
