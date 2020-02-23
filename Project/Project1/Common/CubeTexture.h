#pragma once

#include "d3dUtil.h"
#include "d3dDefine.h"
#include "TextureResource.h"
#include "UploadResource.h"

class CubeTexture 
{
public:
	CubeTexture();
	CubeTexture(ID3D12Device *device, UINT wight, UINT height, DXGI_FORMAT format);
	~CubeTexture();

	void Create(ID3D12Device* device, UINT wight, UINT height, DXGI_FORMAT format);
	ID3D12Resource* GetTextureResource();
	ID3D12Resource* GetUploadResource();

	void MapFace(UINT index, void *data, UINT64 size);
	void UploadData(ID3D12GraphicsCommandList*commandList, UINT firstSubresource =  0, UINT subresourceNum = 6);
	void SetTextureName(const WCHAR *name);
	void SetULresourceName(const WCHAR *name);
	const WCHAR* GetTextureName();
	const WCHAR* GetULresourceName();

private:
	TextureResource		mCubeTexture;
	UpLoadResource		mUlResource;
	UINT64				mSingleFaceSize;
	UINT64				mUlResourceSize;
	void				*mMapData;
};