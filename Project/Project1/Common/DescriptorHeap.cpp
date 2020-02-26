#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap() :
	mDescriptorSize(0)
{
}

DescriptorHeap::DescriptorHeap(ID3D12Device *device, UINT number, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag) :
	DescriptorHeap()
{
	Set(device, number, type, flag);
}

DescriptorHeap::~DescriptorHeap()
{
}

void DescriptorHeap::Set(ID3D12Device *device, UINT number, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag)
{
	Clear();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};   
	heapDesc.NumDescriptors = number;
	heapDesc.Type = type;
	heapDesc.Flags = flag;

	ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mDescriptorHeap)));
	mDescriptorSize = device->GetDescriptorHandleIncrementSize(type);
	mBindResource.assign(number, nullptr);
}

D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeap::GetDesc()
{
	return mDescriptorHeap->GetDesc();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuDescriptorHandle(UINT index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	return cpuHandle.Offset(index, mDescriptorSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuDescriptorHandle(UINT index)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(mDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	return gpuHandle.Offset(index, mDescriptorSize);
}

void DescriptorHeap::BindView(UINT index, ID3D12Resource *resource)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = mDescriptorHeap->GetDesc();

	switch (heapDesc.Type)
	{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			BuildSrvView(index, resource);
		break;

		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			ThrowIfFailed(E_FAIL);
		break;

		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			BuildRtvView(index, resource);
		break;

		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			BuildDsvView(index, resource);
		break;

		case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
			ThrowIfFailed(E_FAIL);
		break;

		default:
			ThrowIfFailed(E_FAIL);
		break;
	}

	mBindResource[index] = resource;
}

DescriptorHeap::operator ID3D12DescriptorHeap* ()
{
	return mDescriptorHeap.Get();
}

void DescriptorHeap::BuildRtvView(UINT index, ID3D12Resource *resource)
{
	D3D12_RESOURCE_DESC desc = resource->GetDesc();
	ID3D12Device* device = nullptr;
	resource->GetDevice(IID_PPV_ARGS(&device));

	//D3D12_RENDER_TARGET_VIEW_DESC rtvView = {};
	//rtvView.ViewDimension = 

	device->CreateRenderTargetView(resource, nullptr, GetCpuDescriptorHandle(index));
	device->Release();
}

void DescriptorHeap::BuildSrvView(UINT index, ID3D12Resource *resource)
{
	D3D12_RESOURCE_DESC desc = resource->GetDesc();
	ID3D12Device* device = nullptr;
	resource->GetDevice(IID_PPV_ARGS(&device));

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constBuffView = {};
		constBuffView.BufferLocation = resource->GetGPUVirtualAddress();
		constBuffView.SizeInBytes = GetRequiredIntermediateSize(resource, 0, 1);
		device->CreateConstantBufferView(&constBuffView, GetCpuDescriptorHandle(index));
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC textView = {};

		if (desc.DepthOrArraySize == 6) 
		{
			textView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			textView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			textView.TextureCube.MipLevels = desc.MipLevels;
			textView.TextureCube.MostDetailedMip = 0;
			textView.TextureCube.ResourceMinLODClamp = 0;
		}
		else 
		{
			textView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			textView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			textView.Format = desc.Format;
			textView.Texture2D.MostDetailedMip = 0;
			textView.Texture2D.MipLevels = desc.MipLevels;
			textView.Texture2D.ResourceMinLODClamp = 0;
		}

		device->CreateShaderResourceView(resource, &textView, GetCpuDescriptorHandle(index));
	}
	break;

	case D3D12_RESOURCE_DIMENSION_UNKNOWN:
	default:
		ThrowIfFailed(E_FAIL);
		break;
	}

	device->Release();
}

void DescriptorHeap::BuildDsvView(UINT index, ID3D12Resource *resource)
{
	D3D12_RESOURCE_DESC desc = resource->GetDesc();
	ID3D12Device* device = nullptr;
	resource->GetDevice(IID_PPV_ARGS(&device));

	device->CreateDepthStencilView(resource, nullptr, GetCpuDescriptorHandle(index));
	device->Release();
}

void DescriptorHeap::Clear()
{
	mDescriptorHeap.Reset();
	mDescriptorSize = 0;
	mBindResource.clear();
}
