//#pragma once
//
//#include "d3dUtil.h"
//#include "RTVresource.h"
//#include "DSVresource.h"
//#include "DescriptorHeap.h"
//
//class PipeLine 
//{
//public:
//	PipeLine();
//	~PipeLine();
//
//	void Create(ID3D12Device *D3Ddevice, ID3D12CommandList *commandList, UINT rtvWidth, UINT rtvHeight, const WCHAR *vsShader, const WCHAR *psShader);
//	void BindCubeTexture(ID3D12Resource *resCubeMap, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
//
//
//private:
//	ID3D12Device									*mD3Ddevice;
//	ID3D12CommandList								*mCommandList;
//	UINT											mRtvWidth;
//	UINT											mRtvHeight;
//
//	Microsoft::WRL::ComPtr<ID3D10Blob>				mVSshaderData;
//	Microsoft::WRL::ComPtr<ID3D10Blob>				mPSshaderData;
//	Microsoft::WRL::ComPtr<ID3D12RootSignature>		mRootSignature;
//	Microsoft::WRL::ComPtr<ID3D12PipelineState>		mPSO;
//
//	std::unique_ptr<MeshGeometry>					mMesh;
//
//	DescriptorHeap									mRtvHeap;
//	DescriptorHeap									mDsvHeap;
//	DescriptorHeap									mSrvHeap;
//	RTVresource										mRtv;
//	DSVresource										mDsv;
//
//	DXGI_FORMAT										mRTVformat;
//	DXGI_FORMAT										mDSVformat;
//
//	std::wstring									mVSshader;
//	std::wstring									mPSshader;
//	std::vector<D3D12_INPUT_ELEMENT_DESC>			mVSinputLayout;
//
//
//	void BuildDescriptorHeap();
//	void BuildResource();
//	void BuildRootSignature();
//	void BuildPipeLineState();
//	void BuildGeometry();
//	
//	void Draw(UINT face);
//};
