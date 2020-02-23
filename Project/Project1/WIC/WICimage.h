#pragma once

#include <wincodec.h>
#include <Wincodecsdk.h>
#include <wrl.h>
#include <vector>

class WICimage
{
public:
	WICimage();
	~WICimage();

	WICimage(const WICimage&) = delete;
	void operator= (const WICimage&) = delete;

	static void Initialize();
	void Reset();
	void OpenFile(const WCHAR *fileName);
	void GetData(void **data, UINT *size);
	void LoadFormMemory(void *data, UINT width, UINT height, UINT rowPitch, UINT stride, DXGI_FORMAT format);
	void Save(const WCHAR *fileName);

	static bool GetTargetPixelFormat(const GUID *pSourceFormat, GUID *pTargetFormat);
	static DXGI_FORMAT GetDXGIFormatFromPixelFormat(const GUID *pPixelFormat);

private:
	static Microsoft::WRL::ComPtr<IWICImagingFactory>			mFactory;
	Microsoft::WRL::ComPtr<IWICBitmapDecoder>					mDecoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode>				mFrameDecode;
	Microsoft::WRL::ComPtr<IWICPixelFormatInfo>					mPixelFormatInfo;

	Microsoft::WRL::ComPtr<IWICStream>							mFileStream;
	Microsoft::WRL::ComPtr<IWICBitmapEncoder>					mEncoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameEncode>				mFrameEncode;
	Microsoft::WRL::ComPtr<IWICBitmap>							mBitmap;

	DXGI_FORMAT													mDXGIformat;
	WICPixelFormatGUID											mWICformatGUID;
	UINT														mWidth;
	UINT														mHeight;
	UINT														mBPP;
	UINT														mRowPitch;
	std::vector<BYTE>											mData;
};

