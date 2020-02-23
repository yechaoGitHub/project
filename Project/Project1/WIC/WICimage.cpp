#include "WICimage.h"
#include "../Common/d3dUtil.h"
#include <math.h>

struct WICTranslate
{
	GUID		wic;
	DXGI_FORMAT format;
};

static WICTranslate g_WICFormats[] =
{
	{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },
	{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
	{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },
	{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
	{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
	{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },
	{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
	{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },
	{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
	{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
	{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
	{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },
	{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },
};

// WIC 像素格式转换表.
struct WICConvert
{
	GUID source;
	GUID target;
};

static WICConvert g_WICConvert[] =
{
	{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
	{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT
	{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT
	{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM
	{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM
	{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
	{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
	{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
	{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
	{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
};

Microsoft::WRL::ComPtr<IWICImagingFactory>	WICimage::mFactory;

WICimage::WICimage() :
	mDXGIformat(DXGI_FORMAT_UNKNOWN),
	mWICformatGUID({}),
	mWidth(0),
	mHeight(0),
	mBPP(0),
	mRowPitch(0)
{
}

WICimage::~WICimage()
{
}

void WICimage::Initialize()
{
	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mFactory)));
}

void WICimage::Reset()
{
	mFrameDecode.Reset();
	mDecoder.Reset();
}

void WICimage::OpenFile(const WCHAR* fileName)
{
	Reset();

	ThrowIfFailed(mFactory->CreateDecoderFromFilename(
		fileName, 
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&mDecoder));

	ThrowIfFailed(mDecoder->GetFrame(0, &mFrameDecode));
	ThrowIfFailed(mFrameDecode->GetPixelFormat(&mWICformatGUID));

	GUID tgFormat = {};
	if (GetTargetPixelFormat(&mWICformatGUID, &tgFormat))
	{
		mDXGIformat = GetDXGIFormatFromPixelFormat(&tgFormat);
		mWICformatGUID = tgFormat;
	}
	else 
	{
		mDXGIformat = GetDXGIFormatFromPixelFormat(&mWICformatGUID);
	}

	if (DXGI_FORMAT_UNKNOWN == mDXGIformat)
	{
		ThrowIfFailed(E_FAIL);
	}
	
	Microsoft::WRL::ComPtr<IWICComponentInfo> WICmntinfo;
	ThrowIfFailed(mFactory->CreateComponentInfo(mWICformatGUID, &WICmntinfo));
	ThrowIfFailed(mFrameDecode->GetSize(&mWidth, &mHeight));
	WICmntinfo.As(&mPixelFormatInfo);
	ThrowIfFailed(mPixelFormatInfo->GetBitsPerPixel(&mBPP));


	mRowPitch = ceil((UINT64)mWidth * (UINT64)mBPP / 8.0f);
	mData.assign((UINT64)mRowPitch * (UINT64)mHeight, 0);
	mFrameDecode->CopyPixels(nullptr, mRowPitch, mData.size(), mData.data());
}

void WICimage::GetData(void **data, UINT *size)
{
	if (mData.size() != 0) 
	{
		*data = mData.data();
		*size = mData.size();
	}
	else 
	{
		*data = nullptr;
		*size = 0;
	}
}

void WICimage::LoadFormMemory(void *data, UINT width, UINT height, UINT rowPitch, UINT stride, DXGI_FORMAT format)
{
	mBitmap.Reset();
	mWidth = width;
	mHeight = height;
	
	ThrowIfFailed(mFactory->CreateBitmap(mWidth, mHeight, GUID_WICPixelFormat32bppRGBA, WICBitmapCacheOnDemand, &mBitmap));
	WICRect rcLock = { 0, 0, mWidth, mHeight };
	Microsoft::WRL::ComPtr<IWICBitmapLock> lockPtr;
	ThrowIfFailed(mBitmap->Lock(&rcLock, WICBitmapLockWrite, &lockPtr));

	UINT bmpBufferSize = 0;
	UINT bmpStride = 0;
	BYTE *bmpBuffer = nullptr;
	ThrowIfFailed(lockPtr->GetStride(&bmpStride));
	ThrowIfFailed(lockPtr->GetDataPointer(&bmpBufferSize, &bmpBuffer));
	
	for (int i = 0; i < height; i++) 
	{
		BYTE *srcLocation = reinterpret_cast<BYTE*>(data) + i * stride;
		BYTE *destLocation = bmpBuffer + i * rowPitch;
		memcpy(destLocation, srcLocation, rowPitch);
	}
}


void WICimage::Save(const WCHAR *fileName)
{
	mFileStream.Reset();
	mEncoder.Reset();
	mFrameEncode.Reset();

	ThrowIfFailed(mFactory->CreateStream(&mFileStream));
	ThrowIfFailed(mFileStream->InitializeFromFilename(fileName, GENERIC_WRITE));
	ThrowIfFailed(mFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &mEncoder));
	ThrowIfFailed(mEncoder->Initialize(mFileStream.Get(), WICBitmapEncoderNoCache));
	ThrowIfFailed(mEncoder->CreateNewFrame(&mFrameEncode, nullptr));

	mFrameEncode->Initialize(nullptr);
	mFrameEncode->SetSize(mWidth, mHeight);

	WICPixelFormatGUID pixelFormat = {};
	memcpy(&pixelFormat, &GUID_WICPixelFormat32bppRGBA, sizeof(WICPixelFormatGUID));
	ThrowIfFailed(mFrameEncode->SetPixelFormat(&pixelFormat));
	ThrowIfFailed(mFrameEncode->WriteSource(mBitmap.Get(), nullptr));

	ThrowIfFailed(mFrameEncode->Commit());
	ThrowIfFailed(mEncoder->Commit());
	ThrowIfFailed(mFileStream->Commit(STGC_DEFAULT));
}

bool WICimage::GetTargetPixelFormat(const GUID* pSourceFormat, GUID* pTargetFormat)
{
	*pTargetFormat = *pSourceFormat;
	for (size_t i = 0; i < _countof(g_WICConvert); ++i)
	{
		if (InlineIsEqualGUID(g_WICConvert[i].source, *pSourceFormat))
		{
			*pTargetFormat = g_WICConvert[i].target;
			return true;
		}
	}
	return false;
}

DXGI_FORMAT WICimage::GetDXGIFormatFromPixelFormat(const GUID* pPixelFormat)
{
	for (size_t i = 0; i < _countof(g_WICFormats); ++i)
	{
		if (InlineIsEqualGUID(g_WICFormats[i].wic, *pPixelFormat))
		{
			return g_WICFormats[i].format;
		}
	}
	return DXGI_FORMAT_UNKNOWN;
}