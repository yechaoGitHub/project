#pragma once

#include "d3dUtil.h"
#include <vector>

class DescriptorHeap
{
public:
	DescriptorHeap();
	DescriptorHeap(ID3D12Device *device, UINT number, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);
	~DescriptorHeap();

	void Set(ID3D12Device *device, UINT number, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag);
	D3D12_DESCRIPTOR_HEAP_DESC GetDesc();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(UINT index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(UINT index);
	void BindView(UINT index, ID3D12Resource *resource);

	operator ID3D12DescriptorHeap* ();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>	mDescriptorHeap;
	UINT											mDescriptorSize;
	std::vector<ID3D12Resource*>					mBindResource;

	void BuildRtvView(UINT index, ID3D12Resource *resource);
	void BuildSrvView(UINT index, ID3D12Resource *resource);
	void BuildDsvView(UINT index, ID3D12Resource *resource);
	void Clear();

};