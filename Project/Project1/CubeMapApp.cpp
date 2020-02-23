#include "CubeMapApp.h"
#include "./WIC/WICimage.h"

const WCHAR* _CubeImageFile_[] = { L"cube_x-.png", L"cube_x+.png", L"cube_y-.png", L"cube_y+.png", L"cube_z+.png", L"cube_z-.png" };


CubeMapApp::CubeMapApp(HINSTANCE hInstance) :
	D3DApp(hInstance),
	mLastMousePt({ 0, 0 })
{

}

void CubeMapApp::Update(const GameTimer &gt)
{
	//mCamera.SetLens(0.25 * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	//mCamera.LookAt(DirectX::XMFLOAT3(0, 0, -10), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
	UpdateCameraWithCubeIndex(1);
	
	GlobalInfo gbInfo;
	gbInfo.cameraPos = mCamera.GetPosition3f();
	gbInfo.cameraDir = mCamera.GetLook3f();
	mGlobalInfo->CopyData(0, gbInfo);

	ObjectProperty objProperty;
	DirectX::XMStoreFloat4x4(&objProperty.model, DirectX::XMMatrixScaling(5000, 5000, 5000));
	objProperty.view = mCamera.GetView4x4f();
	objProperty.project = mCamera.GetProj4x4f();

	mObjectProperty->CopyData(0, objProperty);
}

void CubeMapApp::Draw(const GameTimer &gt)
{
	mDirectCmdListAlloc->Reset();

	mCommandList.Reset();

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvHeap };
	mCommandList->SetDescriptorHeaps(1, descriptorHeaps);

	mCommandList->SetPipelineState(mPSO.Get());
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	mCommandList->SetGraphicsRootConstantBufferView(0, mObjectProperty->GetGpuAddress(0));
	mCommandList->SetGraphicsRootConstantBufferView(1, mGlobalInfo->GetGpuAddress(0));
	mCommandList->SetGraphicsRootDescriptorTable(2, mSrvHeap.GetGpuDescriptorHandle(0));

	mCommandList->IASetVertexBuffers(0, 1, &mMesh->VertexBufferView());
	mCommandList->IASetIndexBuffer(&mMesh->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mCommandList->DrawIndexedInstanced(mMesh->DrawArgs["sphere"].IndexCount, 1, 0, 0, 0);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));

	mCommandList.ResourceBarrier(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
	mCommandList.ReadBackTexture(mRtvReadBack, CurrentBackBuffer(), 0, 1);
	mCommandList.ResourceBarrier(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdList[] = { mCommandList };
	mCommandQueue->ExecuteCommandLists(1, cmdList);
	FlushCommandQueue();

	D3D12_RESOURCE_DESC rtvDesc = CurrentBackBuffer()->GetDesc();

	void *data = mRtvReadBack.Map(0, nullptr);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footPrint = {};
	UINT rowNum = 0;
	UINT64 rowBytes = 0;
	UINT64 totalBytes = 0;

	md3dDevice->GetCopyableFootprints(&rtvDesc, 0, 1, 0, &footPrint, &rowNum, &rowBytes, &totalBytes);
	
	WICimage image;
	image.LoadFormMemory(data, rtvDesc.Width, rowNum, rowBytes, footPrint.Footprint.RowPitch, rtvDesc.Format);
	image.Save(L"IBL.png");


	mRtvReadBack.Unmap(0);

	ThrowIfFailed(mSwapChain->Present(1, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

}

void CubeMapApp::OnResize()
{
	D3DApp::OnResize();

	mCamera.SetLens(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void CubeMapApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePt.x = x;
	mLastMousePt.y = y;

	SetCapture(mhMainWnd);
}

void CubeMapApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CubeMapApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	/*if((btnState & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25 * (x - mLastMousePt.x));
		float dy = DirectX::XMConvertToRadians(0.25 * (y - mLastMousePt.y));

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);

	}

	mLastMousePt.x = x;
	mLastMousePt.y = y;*/
}

void CubeMapApp::OnKeyboardInput(const GameTimer& gt)
{

}

bool CubeMapApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	WICimage::Initialize();
	mCommandList.Set(D3DApp::mCommandList.Get(), mDirectCmdListAlloc.Get());

	// Reset the command list to prep for initialization commands.
	mCommandList.Reset();

	BuildDescriptorHead();
	LoadImageWIC();
	BuildResource();
	BuildConstantBuffer();
	BuildRootSignature();
	BuildShaderAndInputLayout();
	BuildPSO();
	BuildShapeGeometry();

	mCommandList.Close();
	ID3D12CommandList* cmdsLists[] = { mCommandList };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void CubeMapApp::LoadImageWIC()
{
	for (int i = 0; i < _countof(_CubeImageFile_); i++) 
	{
		mArrCubeImage[i].OpenFile(_CubeImageFile_[i]);
	}
}

void CubeMapApp::BuildDescriptorHead()
{
	mSrvHeap.Set(md3dDevice.Get(), 2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	mRtvHeap.Set(md3dDevice.Get(), 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
}

void CubeMapApp::BuildResource()
{
	mCubeTexture.Create(md3dDevice.Get(), 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM);
	mCubeTexture.SetTextureName(L"CubeTexture");
	mCubeTexture.SetULresourceName(L"CubeTextureUpload");

	mSrvHeap.BindView(0, mCubeTexture.GetTextureResource());

	for (int i = 0; i < mArrCubeImage.size(); i++) 
	{
		BYTE *data = nullptr;
		UINT size = 0;
		WICimage &image = mArrCubeImage[i];
		image.GetData(reinterpret_cast<void**>(&data), &size);

		mCubeTexture.MapFace(i, data, size);
	}
	
	mCubeTexture.UploadData(mCommandList);

	mRtvReadBack.BindResource(md3dDevice.Get(), CurrentBackBuffer());
}

void CubeMapApp::BuildConstantBuffer()
{
	mObjectProperty = std::make_unique<UploadBuffer<ObjectProperty>>(md3dDevice.Get(), 1, true);
	mGlobalInfo = std::make_unique<UploadBuffer<GlobalInfo>>(md3dDevice.Get(), 1, true);
}

void CubeMapApp::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 1, &pointWrap,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));

}

void CubeMapApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS =
	{
		mVSshader->GetBufferPointer(),
		mVSshader->GetBufferSize()
	};
	psoDesc.PS =
	{
		mPSshader->GetBufferPointer(),
		mPSshader->GetBufferSize()
	};
	/*psoDesc.VS =
	{
		mIBLvsShader->GetBufferPointer(),
		mIBLvsShader->GetBufferSize()
	};
	psoDesc.PS =
	{
		mIBLpsShader->GetBufferPointer(),
		mIBLpsShader->GetBufferSize()
	};*/
	

	CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	CD3DX12_DEPTH_STENCIL_DESC dsDesc(D3D12_DEFAULT);
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState = dsDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.DSVFormat = mDepthStencilFormat;

	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}


void CubeMapApp::BuildShaderAndInputLayout()
{
	mVSshader = d3dUtil::CompileShader(L"./Shader/Sky.hlsl", nullptr, "VS", "vs_5_1");
	mPSshader = d3dUtil::CompileShader(L"./Shader/Sky.hlsl", nullptr, "PS", "ps_5_1");
	mIBLvsShader = d3dUtil::CompileShader(L"./Shader/IBL.hlsl", nullptr, "VS", "vs_5_1");
	mIBLpsShader = d3dUtil::CompileShader(L"./Shader/IBL.hlsl", nullptr, "PS", "ps_5_1");
	
	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void CubeMapApp::BuildShapeGeometry()
{
	mMesh = std::make_unique<MeshGeometry>();
	mMesh->Name = "Mesh";

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData shpere = geoGen.CreateSphere(1, 20, 20);

	UINT vertexSize = shpere.Vertices.size() * sizeof(GeometryGenerator::Vertex);
	UINT indicsSize = shpere.Indices32.size() * sizeof(UINT);
	
	ThrowIfFailed(D3DCreateBlob(vertexSize, &mMesh->VertexBufferCPU));
	CopyMemory(mMesh->VertexBufferCPU->GetBufferPointer(), shpere.Vertices.data(), vertexSize);

	ThrowIfFailed(D3DCreateBlob(indicsSize, &mMesh->IndexBufferCPU));
	CopyMemory(mMesh->IndexBufferCPU->GetBufferPointer(), shpere.Indices32.data(), indicsSize);

	mMesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList, shpere.Vertices.data(), vertexSize, mMesh->VertexBufferUploader);
	mMesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList, shpere.Indices32.data(), indicsSize, mMesh->IndexBufferUploader);

	mMesh->IndexBufferGPU->SetName(L"IndexResource");
	mMesh->VertexBufferGPU->SetName(L"VertexResource");
	mMesh->VertexBufferUploader->SetName(L"VertexUploader");
	mMesh->IndexBufferUploader->SetName(L"IndexUploader");

	mMesh->VertexByteStride = sizeof(GeometryGenerator::Vertex);
	mMesh->VertexBufferByteSize = vertexSize;
	mMesh->IndexFormat = DXGI_FORMAT_R32_UINT;
	mMesh->IndexBufferByteSize = indicsSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = shpere.Indices32.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;
	submesh.Bounds.Center = {};
	submesh.Bounds.Extents = { 1.5, 1.5, 1.5 };

	mMesh->DrawArgs["sphere"] = submesh;
}

void CubeMapApp::BuildRtv()
{
	mRtv.Set(md3dDevice.Get(), 64, 64);
	mRtvReadBack.BindResource(md3dDevice.Get(), mRtv);
	mRtvHeap.BindView(0, mRtv);
}

void CubeMapApp::UpdateCameraWithCubeIndex(UINT index)
{
	static DirectX::XMFLOAT3 lookAtParam[6][3] =
	{
		{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}},
		{{0, 0, 0}, {-1, 0, 0}, {0, 1, 0}},
		{{0, 0, 0}, {0, 1, 0}, {0, 0, -1}},
		{{0, 0, 0}, {0, -1, 0}, {0, 0, +1}},
		{{0, 0, 0}, {0, 0, 1}, {0, +1, 0}},
		{{0, 0, 0}, {0, 0, -1}, {0, +1, 0}}
	};

	mCamera.SetPosition(0, 0, 0);
	mCamera.SetLens(0.5f * MathHelper::Pi, 1.0, 0.1f, 10.0f);
	mCamera.LookAt(lookAtParam[index][0], lookAtParam[index][1], lookAtParam[index][2]);
	mCamera.UpdateViewMatrix();
}


