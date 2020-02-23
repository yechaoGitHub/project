//#include "PipeLine.h"
//
//
//PipeLine::PipeLine() :
//	mD3Ddevice(nullptr),
//	mCommandList(nullptr),
//	mRtvWidth(0),
//	mRtvHeight(0),
//	mRTVformat(DXGI_FORMAT_R8G8B8A8_UNORM),
//	mDSVformat(DXGI_FORMAT_D24_UNORM_S8_UINT)
//
//{
//}
//
//PipeLine::~PipeLine()
//{
//
//}
//
//void PipeLine::Create(ID3D12Device *D3Ddevice, ID3D12CommandList *commandList, UINT rtvWidth, UINT rtvHeight, const WCHAR *vsShader, const WCHAR *psShader)
//{
//	mD3Ddevice = D3Ddevice;
//	mCommandList = commandList;
//	mRtvWidth = rtvWidth;
//	mRtvHeight = rtvHeight;
//
//	mVSshaderData = d3dUtil::CompileShader(vsShader, nullptr, "VS", "vs_5_1");
//	mPSshaderData = d3dUtil::CompileShader(psShader, nullptr, "PS", "vs_5_1");
//
//	mVSinputLayout =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	};
//
//}
//
//void PipeLine::BuildResource()
//{
//	mRtv.Set(mD3Ddevice, mRtvWidth, mRtvHeight, mRTVformat);
//	mDsv.Set(mD3Ddevice, mRtvWidth, mRtvHeight, mDSVformat);
//}
//
//void PipeLine::Draw(UINT face)
//{
//	mCommandList->RSSetViewports(1, &mScreenViewport);
//	mCommandList->RSSetScissorRects(1, &mScissorRect);
//
//	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));
//
//	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightBlue, 0, nullptr);
//	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 0, nullptr);
//
//	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
//
//	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvHeap };
//	mCommandList->SetDescriptorHeaps(1, descriptorHeaps);
//
//	mCommandList->SetPipelineState(mPSO.Get());
//	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
//
//	mCommandList->SetGraphicsRootConstantBufferView(0, mObjectProperty->GetGpuAddress(0));
//	mCommandList->SetGraphicsRootConstantBufferView(1, mGlobalInfo->GetGpuAddress(0));
//	mCommandList->SetGraphicsRootDescriptorTable(2, mSrvHeap.GetGpuDescriptorHandle(0));
//
//	mCommandList->IASetVertexBuffers(0, 1, &mMesh->VertexBufferView());
//	mCommandList->IASetIndexBuffer(&mMesh->IndexBufferView());
//	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	mCommandList->DrawIndexedInstanced(mMesh->DrawArgs["sphere"].IndexCount, 1, 0, 0, 0);
//
//	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));
//
//}
//
//void PipeLine::BuildDescriptorHeap()
//{
//	mRtvHeap.Set(mD3Ddevice, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
//	mDsvHeap.Set(mD3Ddevice, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
//	mSrvHeap.Set(mD3Ddevice, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
//}
//
//void PipeLine::BuildRootSignature()
//{
//	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
//	slotRootParameter[0].InitAsConstantBufferView(0);
//	slotRootParameter[1].InitAsConstantBufferView(1);
//
//	CD3DX12_DESCRIPTOR_RANGE texTable;
//	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//	slotRootParameter[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
//
//	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
//		0, // shaderRegister
//		D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR, // filter
//		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
//		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
//		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW
//
//	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 1, &pointWrap,
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
//		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
//
//	if (errorBlob != nullptr)
//	{
//		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
//	}
//	ThrowIfFailed(hr);
//
//	ThrowIfFailed(mD3Ddevice->CreateRootSignature(
//		0,
//		serializedRootSig->GetBufferPointer(),
//		serializedRootSig->GetBufferSize(),
//		IID_PPV_ARGS(&mRootSignature)));
//}
//
//void PipeLine::BuildPipeLineState()
//{
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//	psoDesc.InputLayout = { mVSinputLayout.data(), (UINT)mVSinputLayout.size() };
//	psoDesc.pRootSignature = mRootSignature.Get();
//	psoDesc.VS =
//	{
//		mVSshaderData->GetBufferPointer(),
//		mVSshaderData->GetBufferSize()
//	};
//	psoDesc.PS =
//	{
//		mPSshaderData->GetBufferPointer(),
//		mPSshaderData->GetBufferSize()
//	};
//
//	CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
//	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
//	psoDesc.RasterizerState = rasterizerDesc;
//	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//
//	CD3DX12_DEPTH_STENCIL_DESC dsDesc(D3D12_DEFAULT);
//	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//	psoDesc.DepthStencilState = dsDesc;
//	psoDesc.SampleMask = UINT_MAX;
//	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	psoDesc.NumRenderTargets = 1;
//	psoDesc.RTVFormats[0] = mRTVformat;
//	psoDesc.SampleDesc.Count = 1;
//	psoDesc.SampleDesc.Quality = 0;
//	psoDesc.DSVFormat = mDSVformat;
//
//	ThrowIfFailed(mD3Ddevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
//
//}
