#include "CubeTexture.h"
#include "GraphicsCommandList.h"

CubeTexture::CubeTexture() :
	mSingleFaceSize(0),
	mMapData(nullptr)
{
}

CubeTexture::CubeTexture(ID3D12Device* device, UINT wight, UINT height, DXGI_FORMAT format) :
	CubeTexture()
{

}

CubeTexture::~CubeTexture()
{
}

void CubeTexture::Create(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 6;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	mCubeTexture.Create(device, &desc);

	mSingleFaceSize = GetRequiredIntermediateSize(mCubeTexture, 0, 1);

	UINT64 totalSize = mSingleFaceSize * 6;
	mUlResource.Create(device, totalSize);
	mMapData = mUlResource.Map();
}

ID3D12Resource* CubeTexture::GetTextureResource()
{
	return mCubeTexture.Resource();
}

ID3D12Resource* CubeTexture::GetUploadResource()
{
	return mUlResource.Resource();
}

void CubeTexture::MapFace(UINT index, void *data, UINT64 size)
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footPrint[6] = {};
	mCubeTexture.GetFootPrint(footPrint, nullptr, nullptr, nullptr, 0, 6);
	
	if (size > mSingleFaceSize)
	{
		ThrowIfFailed(E_FAIL);
	}

	BYTE* dest = (BYTE*)mMapData + footPrint[index].Offset;
	memcpy(dest, data, size);
}

void CubeTexture::UploadData(ID3D12GraphicsCommandList *commandList, UINT firstSubresource, UINT subresourceNum)
{
	GraphicsCommandList graphicsCommandList(commandList, nullptr);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footPrint[6] = {};
	UINT64 sizeInBytes = 0;
	mCubeTexture.GetFootPrint(footPrint, nullptr, nullptr, nullptr, 0, 6);
	sizeInBytes = GetRequiredIntermediateSize(mCubeTexture, firstSubresource, subresourceNum);


	graphicsCommandList.ResourceBarrier(mCubeTexture, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	graphicsCommandList.UploadTexture(mCubeTexture, firstSubresource, subresourceNum, mUlResource);
	graphicsCommandList.ResourceBarrier(mCubeTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
}

void CubeTexture::SetTextureName(const WCHAR *name)
{
	mCubeTexture.SetName(name);
}

void CubeTexture::SetULresourceName(const WCHAR *name)
{
	mUlResource.SetName(name);
}

const WCHAR* CubeTexture::GetTextureName()
{
	return mCubeTexture.GetName();
}

const WCHAR* CubeTexture::GetULresourceName()
{
	return mUlResource.GetName();
}



