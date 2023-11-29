#pragma once

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include "Animation.h"
#include "SDKwavefile.h"
#include <xaudio2.h>

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize();
	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT LoadWave(LPWSTR filename, IXAudio2SourceVoice** ppsv, XAUDIO2_BUFFER* buf);
	HRESULT LoadBitmapFromResource(ID2D1RenderTarget* pRT, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, __deref_out ID2D1Bitmap** ppBitmap);
	float GetVolumeLinear() const;

private:
	HWND m_hwnd;

	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pBlackBrush;
	ID2D1SolidColorBrush* m_pFloorBrush;
	ID2D1SolidColorBrush* m_pBlueBrush;
	ID2D1SolidColorBrush* m_pGreenBrush;
	ID2D1RadialGradientBrush* m_pRadialGradientBrush;
	ID2D1RectangleGeometry* m_pRectGeo;
	ID2D1PathGeometry* m_pSunGeometry;
	ID2D1PathGeometry* m_pAnimationGeometry;
	ID2D1PathGeometry* m_pAnimationGeometry1;
	ID2D1PathGeometry* m_pAnimationGeometry2;
	ID2D1PathGeometry* m_pAnimationGeometry3;
	ID2D1PathGeometry* squareGeometry;

	// bitmaps
	ID2D1Bitmap* backgroundBitmap;
	ID2D1Bitmap* floorBitmap;
	ID2D1Bitmap* boxBitmap;
	ID2D1Bitmap* myboxBitmap;
	ID2D1Bitmap* lifeBitmap;

	// bitmap brushes
	ID2D1BitmapBrush* backgroundBitmapBrush;
	ID2D1BitmapBrush* floorBitmapBrush;
	ID2D1BitmapBrush* boxBitmapBrush;
	ID2D1BitmapBrush* myboxBitmapBrush;
	ID2D1BitmapBrush* lifeBitmapBrush;

	IWICImagingFactory* m_pWICFactory;

	AnimationLinear<float> m_Animation;
	AnimationLinear<float> m_Animation1;
	AnimationLinear<float> m_Animation2;
	AnimationLinear<float> m_Animation3;

	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;

	IXAudio2* m_pXAudio2;
	IXAudio2MasteringVoice* soundupdown;

	IXAudio2SourceVoice* move;
	XAUDIO2_BUFFER move_buffer;

	IXAudio2SourceVoice* die;
	XAUDIO2_BUFFER die_buffer;

	IXAudio2SourceVoice* victory;
	XAUDIO2_BUFFER victory_buffer;

	IXAudio2SourceVoice* gameover;
	XAUDIO2_BUFFER gameover_buffer;

	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* pDWriteFactory;	// IDWriteFactory는 DWriteCreateFactory 함수 호출 생성.
	ID2D1SolidColorBrush* textBrush;
	IDWriteTextFormat* textFormat;
	IDWriteTextFormat* lifeFormat;
	IDWriteTextFormat* keyFormat;

	int m_codeVK;
	int m_VolumeLog;
	bool m_IsMuted;	
};
