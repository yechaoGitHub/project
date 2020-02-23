#include "ReadBackResource.h"

ReadBackResource::ReadBackResource()
{

}

ReadBackResource::ReadBackResource(ID3D12Device* device, UINT length)
{
	Create(device, length);
}

ReadBackResource::ReadBackResource(ID3D12Device* device, ID3D12Resource* bindResource)
{
    BindResource(device, bindResource);
}

ReadBackResource::~ReadBackResource()
{

}

void ReadBackResource::Create(ID3D12Device* device, UINT length)
{
	if (mSrc != nullptr) 
	{
		ThrowIfFailed(E_FAIL);
	}

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}

void ReadBackResource::BindResource(ID3D12Device* device, ID3D12Resource* bindResource)
{
	if (mSrc != nullptr)
	{
		ThrowIfFailed(E_FAIL);
	}

    UINT64 length = GetRequiredIntermediateSize(bindResource, 0, 1);
    if (length <= 0)
    {
        ThrowIfFailed(HRESULT(-1));
    }

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}

void ReadBackResource::Bind(ID3D12Resource *res)
{
	if (mSrc != nullptr)
	{
		ThrowIfFailed(E_FAIL);
	}

	D3D12_HEAP_PROPERTIES heapProp = {};
	D3D12_HEAP_FLAGS heapFlag = {};
	ThrowIfFailed(res->GetHeapProperties(&heapProp, &heapFlag));
	D3D12_RESOURCE_DESC desc = res->GetDesc();

	if (desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
		heapProp.Type != D3D12_HEAP_TYPE_READBACK)
	{
		ThrowIfFailed(E_FAIL);
	}

	mSrc = res;
	mbCreate = false;
}

void* ReadBackResource::Map(UINT subresource, D3D12_RANGE* range)
{
	return ResourceBase::Map(subresource, range);
}

void ReadBackResource::Unmap(UINT subresource, D3D12_RANGE* range)
{
    ResourceBase::Unmap(subresource, range);
}
