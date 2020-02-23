#include "RTVresource.h"

RTVresource::RTVresource()
{
}

RTVresource::RTVresource(ID3D12Device *device, UINT width, UINT height, DXGI_FORMAT format)
{
    Set(device, width, height);
}

RTVresource::~RTVresource()
{
    
}

void RTVresource::Set(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
        D3D12_RESOURCE_STATE_PRESENT,
        nullptr,
        IID_PPV_ARGS(&mSrc)));
}

