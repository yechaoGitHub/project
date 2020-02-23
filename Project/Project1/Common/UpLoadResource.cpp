#include "UpLoadResource.h"

UpLoadResource::UpLoadResource() :
    mMapData(nullptr),
	mMapCount(0)
{

}

UpLoadResource::UpLoadResource(ID3D12Device* device, UINT length, bool persistented) :
    UpLoadResource()
{
	Create(device, length);

    if (persistented) 
    {
        mSrc->Map(0, nullptr, reinterpret_cast<void**>(&mMapData));
    }
}

UpLoadResource::UpLoadResource(ID3D12Device* device, ID3D12Resource* bindResource, bool persistented) :
    UpLoadResource()
{
    BindResource(device, bindResource);

    if (persistented)
    {
        mSrc->Map(0, nullptr, reinterpret_cast<void**>(&mMapData));
    }
}

UpLoadResource::~UpLoadResource()
{
   if (mMapCount != 0)
    {
	   while (mMapCount--) 
	   {
		   Unmap();
	   }
    }

    mMapData = nullptr;

	ResourceBase::~ResourceBase();
}

void UpLoadResource::Bind(ID3D12Resource *res)
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
		heapProp.Type != D3D12_HEAP_TYPE_UPLOAD) 
	{
		ThrowIfFailed(E_FAIL);
	}

	mSrc = res;
	mbCreate = false;
}

void* UpLoadResource::Map()
{
	mMapCount++;
    mSrc->Map(0, nullptr, reinterpret_cast<void**>(&mMapData));
    return mMapData;
}

void UpLoadResource::CopyData(void *data, UINT64 start, UINT64 size)
{
	UINT64 resSize = GetRequiredIntermediateSize(mSrc, 0, 1);
	if (resSize < start + size) 
	{
		ThrowIfFailed(E_FAIL);
	}

	if (mMapCount <= 0) 
	{
		Map();
	}

	memcpy(mMapData + start, data, size);
}

void UpLoadResource::Unmap()
{
	if (mMapCount > 0) 
	{
		mSrc->Unmap(0, nullptr);
		mMapCount--;
	}
}

void UpLoadResource::Create(ID3D12Device* device, UINT64 length)
{
	if (mSrc != nullptr) 
	{
		ThrowIfFailed(E_FAIL);
	}

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}

void UpLoadResource::BindResource(ID3D12Device *device, ID3D12Resource *bindResource)
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
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(length),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}