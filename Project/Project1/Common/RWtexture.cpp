#include "RWtexture.h"

RWtexture::RWtexture()
{

}

RWtexture::RWtexture(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    Set(device, width, height, format);
}

RWtexture::~RWtexture()
{
}

void RWtexture::Set(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
    texResource.Create(device, width, height, format);
    rbResource.BindResource(device, texResource);
    ulResource.BindResource(device, texResource);
}

D3D12_RESOURCE_DESC RWtexture::GetDesc()
{
    return texResource.GetDesc();
}

void RWtexture::WriteData(void *data, UINT64 begin, UINT64 size)
{
	ulResource.CopyData(data, begin, size);
}

void* RWtexture::ReadMap(UINT subresource, D3D12_RANGE* range)
{
    return rbResource.Map(0, nullptr);
}

void RWtexture::ReadUnmap(UINT subresource, D3D12_RANGE* range)
{
    rbResource.Unmap(0, range);
}

ID3D12Resource* RWtexture::GetRBresource()
{
    return rbResource;
}

ID3D12Resource* RWtexture::GetULresource()
{
    return ulResource;
}

ID3D12Resource* RWtexture::GetTextureresource()
{
    return texResource;
}
