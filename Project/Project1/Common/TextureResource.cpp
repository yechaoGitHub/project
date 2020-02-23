#include "TextureResource.h"

TextureResource::TextureResource()
{
    
}

TextureResource::TextureResource(ID3D12Device* device, D3D12_RESOURCE_DESC* desc, D3D12_HEAP_FLAGS flag, D3D12_RESOURCE_STATES intitState)
{
}

TextureResource::TextureResource(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	Create(device, width, height, format);
}

TextureResource::~TextureResource()
{

}

void TextureResource::Create(ID3D12Device* device, D3D12_RESOURCE_DESC* desc, D3D12_HEAP_FLAGS flag, D3D12_RESOURCE_STATES intitState)
{
	if (mSrc != nullptr)
	{
		ThrowIfFailed(E_FAIL);
	}

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        flag,
        desc,
        intitState,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}

void TextureResource::Create(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	if (mSrc != nullptr)
	{
		ThrowIfFailed(E_FAIL);
	}

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 0, 1, 0),
        D3D12_RESOURCE_STATE_PRESENT,
        nullptr,
        IID_PPV_ARGS(&mSrc)));

	mbCreate = true;
}



