#pragma once
  
#include "d3dUtil.h"
#include "d3dDefine.h"
#include <vector>

class GraphicsCommandList
{
public:
    GraphicsCommandList();
    GraphicsCommandList(ID3D12GraphicsCommandList *commandList, ID3D12CommandAllocator *commandAllocator);

    void Set(ID3D12GraphicsCommandList *commandList, ID3D12CommandAllocator *commandAllocator);
    void Reset(ID3D12PipelineState *PipelineState = nullptr);
    void Close();
    void ClearRtv(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, DirectX::XMFLOAT4 color, std::vector<D3D12_RECT> rect = {});
    void ClearDsv(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_CLEAR_FLAGS flag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, FLOAT depth = 1.0f, UINT8 stencil = 0, std::vector<D3D12_RECT> rect = {});
    void ResourceBarrier(ID3D12Resource *resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
	void UploadTexture(ID3D12Resource *dest, UINT subresourceIndex, UINT numbers, ID3D12Resource *src);
	void UploadResource(ID3D12Resource *dest,UINT start, ID3D12Resource *src, UINT offset, UINT bytes);
	void ReadBackTexture(ID3D12Resource *rbRes, ID3D12Resource *texture, UINT subresourceIndex, UINT numbers);
	void ReadBackResource(ID3D12Resource *dest, UINT start, ID3D12Resource *src, UINT offset, UINT bytes);
    void SetViewPorts(UINT num, const D3D12_VIEWPORT *viewPorts);
    void SetScissorRects(UINT num, const D3D12_RECT* rects);
    void SetRtvDsv(UINT nums, D3D12_CPU_DESCRIPTOR_HANDLE *rtvHandle, bool bSingle, D3D12_CPU_DESCRIPTOR_HANDLE *dsvHandle);
    void SetDescriptorHeap(UINT num, ID3D12DescriptorHeap *const* ppHead);
    void SetRootGraphicSignature(ID3D12RootSignature *pRootSignature);
    void SetGraphicRoot(EGraphicRootType type, UINT registerNum, D3D12_GPU_VIRTUAL_ADDRESS address);
    void SetGraphicDescriptorHeap(UINT registerNum, D3D12_GPU_DESCRIPTOR_HANDLE handle);

    operator ID3D12GraphicsCommandList* const ();
    ID3D12GraphicsCommandList* operator->();

private:
    ID3D12GraphicsCommandList   *mCommandList;
    ID3D12CommandAllocator      *mCommandAllocator;

};