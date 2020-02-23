#include "GraphicsCommandList.h"


GraphicsCommandList::GraphicsCommandList()
{

}

GraphicsCommandList::GraphicsCommandList(ID3D12GraphicsCommandList *commandList, ID3D12CommandAllocator *commandAllocator)
{
	Set(commandList, commandAllocator);
}

void GraphicsCommandList::Set(ID3D12GraphicsCommandList *commandList, ID3D12CommandAllocator *commandAllocator)
{
	mCommandList = commandList;
	mCommandAllocator = commandAllocator;
}

void GraphicsCommandList::Reset(ID3D12PipelineState *PipelineState)
{
	ThrowIfFailed(mCommandList->Reset(mCommandAllocator, PipelineState));
}

void GraphicsCommandList::Close()
{
	ThrowIfFailed(mCommandList->Close());
}

void GraphicsCommandList::ClearRtv(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, DirectX::XMFLOAT4 color, std::vector<D3D12_RECT> rect)
{
	FLOAT arrColor[4] = {};
	arrColor[0] = color.x;
	arrColor[1] = color.y;
	arrColor[2] = color.z;
	arrColor[3] = color.w;

	UINT rectSize = rect.size();
	const D3D12_RECT* pRect = nullptr;
	pRect = rectSize > 0 ? rect.data() : nullptr;

	mCommandList->ClearRenderTargetView(cpuHandle, arrColor, rectSize, pRect);
}

void GraphicsCommandList::ClearDsv(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_CLEAR_FLAGS flag, FLOAT depth, UINT8 stencil, std::vector<D3D12_RECT> rect)
{
	UINT rectSize = rect.size();
	const D3D12_RECT* pRect = nullptr;
	pRect = rectSize > 0 ? rect.data() : nullptr;
	mCommandList->ClearDepthStencilView(cpuHandle, flag, depth, stencil, rectSize, pRect);
}

void GraphicsCommandList::ResourceBarrier(ID3D12Resource *resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	mCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
		resource,
		beforeState,
		afterState));
}

void GraphicsCommandList::UploadTexture(ID3D12Resource *dest, UINT subresourceIndex, UINT numbers, ID3D12Resource *ulRes)
{
	D3D12_HEAP_PROPERTIES heapProperty = {};
	D3D12_HEAP_FLAGS flags = {};
	ulRes->GetHeapProperties(&heapProperty, &flags);
	if (heapProperty.Type != D3D12_HEAP_TYPE_UPLOAD)
	{
		ThrowIfFailed(E_FAIL);
	}

	D3D12_RESOURCE_DESC destDesc = dest->GetDesc();
	if (destDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
	{
		ThrowIfFailed(E_FAIL);
	}


	const UINT numSubresources = destDesc.DepthOrArraySize * destDesc.MipLevels;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT *footPrint = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[numSubresources];
	GetResourceFootPrint(dest, 0, numSubresources, footPrint);

	for (int i = 0; i < numbers; i++)
	{
		CD3DX12_TEXTURE_COPY_LOCATION dstLoaction(dest, subresourceIndex + i);
		CD3DX12_TEXTURE_COPY_LOCATION srcLocation(ulRes, footPrint[subresourceIndex + i]);
		mCommandList->CopyTextureRegion(&dstLoaction, 0, 0, 0, &srcLocation, nullptr);
	}

	delete footPrint;
}

void GraphicsCommandList::UploadResource(ID3D12Resource *dest, UINT start, ID3D12Resource *ulRes, UINT offset, UINT bytes)
{
	D3D12_HEAP_PROPERTIES heapProperty = {};
	D3D12_HEAP_FLAGS flags = {};
	ulRes->GetHeapProperties(&heapProperty, &flags);
	if (heapProperty.Type != D3D12_HEAP_TYPE_UPLOAD) 
	{
		ThrowIfFailed(E_FAIL);
	}

	D3D12_RESOURCE_DESC destDesc = dest->GetDesc();
	if (destDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
	{
		ThrowIfFailed(E_FAIL);
	}

	mCommandList->CopyBufferRegion(dest, start, ulRes, offset, bytes);
}

void GraphicsCommandList::ReadBackTexture(ID3D12Resource *rbRes, ID3D12Resource *src, UINT subresourceIndex, UINT numbers)
{
	D3D12_HEAP_PROPERTIES heapProperty = {};
	D3D12_HEAP_FLAGS flags = {};
	rbRes->GetHeapProperties(&heapProperty, &flags);
	if (heapProperty.Type != D3D12_HEAP_TYPE_READBACK)
	{
		ThrowIfFailed(E_FAIL);
	}

	D3D12_RESOURCE_DESC srcDesc = src->GetDesc();
	if (srcDesc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
	{
		ThrowIfFailed(E_FAIL);
	}

	const UINT numSubresources = srcDesc.DepthOrArraySize * srcDesc.MipLevels;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT *footPrint = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[numSubresources];
	GetResourceFootPrint(src, 0, numSubresources, footPrint);

	for (int i = 0; i < numbers; i++)
	{
		CD3DX12_TEXTURE_COPY_LOCATION dstLoaction(rbRes, footPrint[subresourceIndex + i]);
		CD3DX12_TEXTURE_COPY_LOCATION srcLocation(src, subresourceIndex + i);
		mCommandList->CopyTextureRegion(&dstLoaction, 0, 0, 0, &srcLocation, nullptr);
	}

	delete footPrint;
}

void GraphicsCommandList::ReadBackResource(ID3D12Resource *rbRes, UINT start, ID3D12Resource *src, UINT offset, UINT bytes)
{
	D3D12_HEAP_PROPERTIES heapProperty = {};
	D3D12_HEAP_FLAGS flags = {};
	rbRes->GetHeapProperties(&heapProperty, &flags);

	if (heapProperty.Type != D3D12_HEAP_TYPE_READBACK) 
	{
		ThrowIfFailed(E_FAIL);
	}

	mCommandList->CopyBufferRegion(rbRes, start, src, offset, bytes);
}

void GraphicsCommandList::SetViewPorts(UINT num, const D3D12_VIEWPORT *viewPorts)
{
	mCommandList->RSSetViewports(num, viewPorts);
}

void GraphicsCommandList::SetScissorRects(UINT num, const D3D12_RECT* rects)
{
	mCommandList->RSSetScissorRects(num, rects);
}

void GraphicsCommandList::SetRtvDsv(UINT nums, D3D12_CPU_DESCRIPTOR_HANDLE *rtvHandle, bool bSingle, D3D12_CPU_DESCRIPTOR_HANDLE *dsvHandle)
{
	mCommandList->OMSetRenderTargets(nums, rtvHandle, bSingle, dsvHandle);
}

void GraphicsCommandList::SetDescriptorHeap(UINT num, ID3D12DescriptorHeap* const* ppHead)
{
	mCommandList->SetDescriptorHeaps(num, ppHead);
}

void GraphicsCommandList::SetRootGraphicSignature(ID3D12RootSignature* pRootSignature)
{
	mCommandList->SetGraphicsRootSignature(pRootSignature);
}

void GraphicsCommandList::SetGraphicRoot(EGraphicRootType type, UINT registerNum, D3D12_GPU_VIRTUAL_ADDRESS address)
{
	switch (type)
	{
		case EGraphicRootType::CONSTANT_BUFFER:
			mCommandList->SetGraphicsRootConstantBufferView(registerNum, address);
		break;

		case EGraphicRootType::SRV:
			mCommandList->SetGraphicsRootShaderResourceView(registerNum, address);
		break;

		case EGraphicRootType::UAV:
			mCommandList->SetGraphicsRootUnorderedAccessView(registerNum, address);
		break;

		default:
			ThrowIfFailed(E_FAIL);
		break;
	}
}

void GraphicsCommandList::SetGraphicDescriptorHeap(UINT registerNum, D3D12_GPU_DESCRIPTOR_HANDLE handle)
{
	mCommandList->SetGraphicsRootDescriptorTable(registerNum, handle);
}

GraphicsCommandList::operator ID3D12GraphicsCommandList* const ()
{
	return mCommandList;
}

ID3D12GraphicsCommandList* GraphicsCommandList::operator->()
{
	return mCommandList;
}
