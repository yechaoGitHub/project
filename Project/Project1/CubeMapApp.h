#pragma once
#include "./Common/d3dApp.h"
#include "./Common/DescriptorHeap.h"
#include "./Common/CubeTexture.h"
#include "./Common/Camera.h"
#include "./Common/DescriptorHeap.h"
#include "./Common/GraphicsCommandList.h"
#include "./Common/UploadBuffer.h"
#include "./Common/GeometryGenerator.h"
#include "./Common/RTVresource.h"
#include "./Common/ReadBackResource.h"
#include "./WIC/WICimage.h"
#include <array>

struct VSinput
{
	DirectX::XMFLOAT3 position	= {};
	DirectX::XMFLOAT3 normal	= {};
	DirectX::XMFLOAT3 color		= {};
	DirectX::XMFLOAT2 texCrood	= {};
};

struct ObjectProperty
{
	DirectX::XMFLOAT4X4 modelViewProj	= MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 model			= MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 view			= MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 project			= MathHelper::Identity4x4();
};

struct GlobalInfo 
{
	DirectX::XMFLOAT3 cameraPos = {};
	DirectX::XMFLOAT3 cameraDir = {};
};

class CubeMapApp : public D3DApp
{
public:
	CubeMapApp(HINSTANCE hInstance);
	virtual bool Initialize() override;

protected:
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void OnResize() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void OnKeyboardInput(const GameTimer& gt);

private:
	CubeTexture										mCubeTexture;
	Camera											mCamera;
	DescriptorHeap									mSrvHeap;
	GraphicsCommandList								mCommandList;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		mRootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		mPSO;
	std::array<WICimage, 6>							mArrCubeImage;
	std::unique_ptr<UploadBuffer<ObjectProperty>>	mObjectProperty;
	std::unique_ptr<UploadBuffer<GlobalInfo>>		mGlobalInfo;
	std::unique_ptr<MeshGeometry>					mMesh;
	Microsoft::WRL::ComPtr<ID3DBlob>				mVSshader;
	Microsoft::WRL::ComPtr<ID3DBlob>				mPSshader;
	Microsoft::WRL::ComPtr<ID3DBlob>				mIBLvsShader;
	Microsoft::WRL::ComPtr<ID3DBlob>				mIBLpsShader;
	std::vector<D3D12_INPUT_ELEMENT_DESC>			mInputLayout;

	DescriptorHeap									mRtvHeap;
	RTVresource										mRtv;
	ReadBackResource								mRtvReadBack;

	POINT											mLastMousePt;

	void LoadImageWIC();
	void BuildDescriptorHead();
	void BuildResource();
	void BuildConstantBuffer();
	void BuildRootSignature();
	void BuildPSO();
	void BuildShaderAndInputLayout();
	void BuildShapeGeometry();
	void BuildRtv();
	void UpdateCameraWithCubeIndex(UINT index);

};

