#include "DemoApp.h"
#include "SDKwavefile.h"
#include <xaudio2.h>
#include <string>
#include <windows.h>

// 마우스 좌표를 전역으로 받는 변수

D2D1_POINT_2F box;
bool clear = false;
bool start = false;
WCHAR Text[100];
WCHAR Text1[100];
float volume = 0.5f;
int life = 3;
bool volumeOX = true;
float tempVolume = 0.5f;

void boxYposition(float x, float y) {
	// y값이 바닥 위인 구간 판별
	if (y < 218.0f) {
		box.y = 218.0f;
	}else if (y < 286.0f && (x > 198 && x < 413)) {
		box.y = 286.0f;
	}

	// y값이 바닥 아래인 구간 판별
	if (y > 332.0f && ((x > 66 && x < 154) || (x > 460 && x < 528))) {
		box.y = 332.0f;
	}else if (y > 385.0f && ((x>153 && x<218)||(x>393&&x<470))) {
		box.y = 385.0f;
	}else if (y > 441 && (x > 213 && x < 403)) {
		box.y = 441;
	}	
}

void boxXposition(float x, float y) {
	if (x < 86) {
		box.x = 86.0f;
	}else if (x > 518) {
		box.x = 518.0f;
	}

	// 좌측
	if ((x < 154 && x>149) && (y > 332 && y < 395)) {
		box.x = 154;
	}else if ((x < 218 && x>213) && (y > 386 && y < 451)) {
		box.x = 218;
	}else if ((x < 213 && x>198) && (y > 208 && y < 286)) {
		box.x = 198;
	}

	//우측
	if ((x < 470 && x>460) && (y > 332 && y < 395)) {
		box.x = 460;
	}else if ((x < 403 && x>393) && (y > 386 && y < 451)) {
		box.x = 393;
	}else if ((x < 413 && x>403) && (y > 208 && y < 286)) {
		box.x = 413;
	}

}

bool startOX(int x) {
	if (x < 144) {
		return true;
	}
	else {
		return false;
	}
}

bool clearOX(int x) {
	if (x > 460) {
		return true;
	}
	else {
		return false;
	}
}

bool collisionOX(int boxX, int boxY, int colboxX, int colboxY) {
	if ((colboxX - 12 < boxX + 10 && colboxX + 12 > boxX - 10)&&(colboxY - 12 < boxY + 10 && colboxY + 12 > boxY - 10)) {
		life -= 1;
		return true;
	}
	else {
		return false;
	}
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pBlackBrush(NULL),
	m_pWICFactory(NULL),
	backgroundBitmap(NULL),
	backgroundBitmapBrush(NULL),
	m_pRadialGradientBrush(NULL),
	m_pRectGeo(NULL),
	floorBitmap(NULL),
	floorBitmapBrush(NULL),
	boxBitmap(NULL),
	myboxBitmap(NULL),
	boxBitmapBrush(NULL),
	myboxBitmapBrush(NULL),
	lifeBitmap(NULL),
	lifeBitmapBrush(NULL),
	m_pSunGeometry(NULL),
	m_pFloorBrush(NULL),
	m_pAnimationGeometry(NULL),
	m_pAnimationGeometry1(NULL),
	m_pAnimationGeometry2(NULL),
	squareGeometry(NULL),
	m_pBlueBrush(NULL),
	m_pGreenBrush(NULL),
	m_Animation(),
	m_Animation1(),
	m_Animation2(),
	m_pXAudio2(NULL),
	m_VolumeLog(5),
	m_IsMuted(false),
	move(NULL),
	move_buffer({ 0 }),
	die(NULL),
	die_buffer({ 0 }),
	victory(NULL),
	victory_buffer({ 0 }),
	textBrush(NULL),
	textFormat(NULL),
	soundupdown(NULL),
	gameover(NULL),
	gameover_buffer({0})
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(backgroundBitmap);
	SAFE_RELEASE(backgroundBitmapBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pRectGeo);
	SAFE_RELEASE(myboxBitmap);
	SAFE_RELEASE(boxBitmap);
	SAFE_RELEASE(boxBitmapBrush);
	SAFE_RELEASE(myboxBitmapBrush);
	SAFE_RELEASE(floorBitmap);
	SAFE_RELEASE(floorBitmapBrush);
	SAFE_RELEASE(m_pSunGeometry);
	SAFE_RELEASE(m_pFloorBrush);
	SAFE_RELEASE(squareGeometry);
	SAFE_RELEASE(m_pBlueBrush);
	SAFE_RELEASE(m_pGreenBrush);
	SAFE_RELEASE(m_pXAudio2);
	SAFE_RELEASE(textBrush);
	SAFE_RELEASE(textFormat);
	SAFE_RELEASE(lifeFormat);
	SAFE_RELEASE(keyFormat);
	SAFE_RELEASE(lifeBitmap);
	SAFE_RELEASE(lifeBitmapBrush);
}

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"Obstacle Avoid", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			616, 639, NULL, NULL, HINST_THISCOMPONENT, this);

		hr = m_hwnd ? S_OK : E_FAIL;

		// 애니메이션
		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pAnimationGeometry->ComputeLength(NULL, &length);

			if (SUCCEEDED(hr))
			{
				m_Animation.SetStart(0); //start at beginning of path
				m_Animation.SetEnd(length); //length at end of path
				m_Animation.SetDuration(4.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		if (SUCCEEDED(hr))
		{
			float length1 = 0;
			hr = m_pAnimationGeometry1->ComputeLength(NULL, &length1);

			if (SUCCEEDED(hr))
			{
				m_Animation1.SetStart(0); //start at beginning of path
				m_Animation1.SetEnd(length1); //length at end of path
				m_Animation1.SetDuration(4.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		if (SUCCEEDED(hr))
		{
			float length2 = 0;
			hr = m_pAnimationGeometry2->ComputeLength(NULL, &length2);

			if (SUCCEEDED(hr))
			{
				m_Animation2.SetStart(0); //start at beginning of path
				m_Animation2.SetEnd(length2); //length at end of path
				m_Animation2.SetDuration(4.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

		if (SUCCEEDED(hr)) {
			hr = XAudio2Create(&m_pXAudio2);
		}

		if (SUCCEEDED(hr)) {
			hr = m_pXAudio2->CreateMasteringVoice(&soundupdown);
			soundupdown->SetVolume(volume);
		}

		if (SUCCEEDED(hr)) {
			WCHAR filename[] = L".\\sound\\move.wav";
			hr = LoadWave(filename, &move, &move_buffer);
		}

		if (SUCCEEDED(hr)) {
			WCHAR filename[] = L".\\sound\\die.wav";
			hr = LoadWave(filename, &die, &die_buffer);
		}

		if (SUCCEEDED(hr)) {
			WCHAR filename[] = L".\\sound\\victory.wav";
			hr = LoadWave(filename, &victory, &victory_buffer);
		}

		if (SUCCEEDED(hr)) {
			WCHAR filename[] = L".\\sound\\gameover.wav";
			hr = LoadWave(filename, &gameover, &gameover_buffer);
		}
	}

	QueryPerformanceFrequency(&m_nFrequency);
	QueryPerformanceCounter(&m_nPrevTime);
	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	if (SUCCEEDED(hr))
	{
		// 영역을 전체 창 크기인 600*600으로 설정
		hr = m_pD2DFactory->CreateRectangleGeometry(D2D1::RectF(0, 0, 600, 600), &m_pRectGeo);
	}

	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
	}

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Verdana",                  // 폰트 패밀리 이름의 문자열
			NULL,                        // 폰트 컬렉션 객체, NULL=시스템 폰트 컬렉션
			DWRITE_FONT_WEIGHT_NORMAL,   // 폰트 굵기. LIGHT, NORMAL, BOLD 등.
			DWRITE_FONT_STYLE_NORMAL,    // 폰트 스타일. NORMAL, OBLIQUE, ITALIC.
			DWRITE_FONT_STRETCH_NORMAL,  // 폰트 간격. CONDENSED, NORMAL, MEDIUM, EXTEXDED 등.
			12,                          // 폰트 크기.
			L"",                         // 로케일을 문자열로 명시.  영어-미국=L"en-us", 한국어-한국=L"ko-kr"
			&textFormat
		);
	}

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Verdana",                  // 폰트 패밀리 이름의 문자열
			NULL,                        // 폰트 컬렉션 객체, NULL=시스템 폰트 컬렉션
			DWRITE_FONT_WEIGHT_NORMAL,   // 폰트 굵기. LIGHT, NORMAL, BOLD 등.
			DWRITE_FONT_STYLE_NORMAL,    // 폰트 스타일. NORMAL, OBLIQUE, ITALIC.
			DWRITE_FONT_STRETCH_NORMAL,  // 폰트 간격. CONDENSED, NORMAL, MEDIUM, EXTEXDED 등.
			12,                          // 폰트 크기.
			L"",                         // 로케일을 문자열로 명시.  영어-미국=L"en-us", 한국어-한국=L"ko-kr"
			&lifeFormat
		);
	}

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Verdana",                  // 폰트 패밀리 이름의 문자열
			NULL,                        // 폰트 컬렉션 객체, NULL=시스템 폰트 컬렉션
			DWRITE_FONT_WEIGHT_NORMAL,   // 폰트 굵기. LIGHT, NORMAL, BOLD 등.
			DWRITE_FONT_STYLE_NORMAL,    // 폰트 스타일. NORMAL, OBLIQUE, ITALIC.
			DWRITE_FONT_STRETCH_NORMAL,  // 폰트 간격. CONDENSED, NORMAL, MEDIUM, EXTEXDED 등.
			12,                          // 폰트 크기.
			L"",                         // 로케일을 문자열로 명시.  영어-미국=L"en-us", 한국어-한국=L"ko-kr"
			&keyFormat
		);
	}

	if (SUCCEEDED(hr))
	{

		hr = m_pD2DFactory->CreatePathGeometry(&m_pSunGeometry);
		if (SUCCEEDED(hr))
		{
			// 태양 경로 기하 그리기
			ID2D1GeometrySink* pSink = NULL;

			hr = m_pSunGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(400, 207),
					D2D1_FIGURE_BEGIN_HOLLOW
				);

				// 한번에 태양을 나타낼 반원을 그리려니까 찌그러진 반원이되서 반으로 나눠 2개의 곡선으로 반원을 그림
				pSink->AddBezier(
					D2D1::BezierSegment(
						// 시작점
						D2D1::Point2F(400, 207),	
						// 베지에 커브로 그릴 가중치가 되는 점
						D2D1::Point2F(422, 150),
						// 마지막 점(중간지점)
						D2D1::Point2F(462, 150)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
						// 마지막 점에서 다시 이어서
						D2D1::Point2F(462, 150),
						D2D1::Point2F(508, 150),
						// 최종 점까지 나눠서 반원을 그림
						D2D1::Point2F(525, 207)
					)); 

				pSink->AddLine(D2D1::Point2F(400, 207));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				// 태양 주위의 곡선들
				pSink->BeginFigure(
					D2D1::Point2F(392, 173),
					D2D1_FIGURE_BEGIN_HOLLOW
				);
				pSink->AddBezier(
					D2D1::BezierSegment(
						// 하나하나 그려보면서 임의의 점으로 설정
						D2D1::Point2F(392, 173),
						D2D1::Point2F(380, 182),
						D2D1::Point2F(384, 168)
					));

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(384, 168),
						D2D1::Point2F(384, 153),
						D2D1::Point2F(374, 159)
					));
				
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(412, 147),
					D2D1_FIGURE_BEGIN_HOLLOW
				);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(412, 147),
						D2D1::Point2F(400, 156),
						D2D1::Point2F(404, 133)
					));

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(404, 133),
						D2D1::Point2F(404, 118),
						D2D1::Point2F(397, 125)
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(450, 133),
					D2D1_FIGURE_BEGIN_HOLLOW
				);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(450, 133),
						D2D1::Point2F(432, 126),
						D2D1::Point2F(449, 116)
					));

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(449, 116),
						D2D1::Point2F(454, 109),
						D2D1::Point2F(445, 104)
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(483, 138),
					D2D1_FIGURE_BEGIN_HOLLOW
				);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(483, 138),
						D2D1::Point2F(493, 145),
						D2D1::Point2F(499, 128)
					));

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(499, 128),
						D2D1::Point2F(503, 113),
						D2D1::Point2F(521, 119)
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(522, 175),
					D2D1_FIGURE_BEGIN_HOLLOW
				);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(522, 175),
						D2D1::Point2F(532, 182),
						D2D1::Point2F(536, 166)
					));

				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(536, 166),
						D2D1::Point2F(540, 151),
						D2D1::Point2F(553, 167)
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				hr = pSink->Close();

				SAFE_RELEASE(pSink);
			}

			if (SUCCEEDED(hr))
			{

				// 애니메이션의 경로가 되는 원을 그림
				hr = m_pD2DFactory->CreatePathGeometry(&m_pAnimationGeometry);
				hr = m_pAnimationGeometry->Open(&pSink);
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				// 
					pSink->BeginFigure(
						// 반지름에 해당하는 165만큼 뺀 곳부터 시작해서
						D2D1::Point2F(-165, 0), 
						D2D1_FIGURE_BEGIN_FILLED
					);

					pSink->AddArc(
						D2D1::ArcSegment(
							// 반대쪽 지름지점을 종점으로 
							D2D1::Point2F(165, 0), 
							// 165의 반지름을 가진 반원을 그린다
							D2D1::SizeF(165, 165), 
							0.0f, // 회전 각도
							D2D1_SWEEP_DIRECTION_CLOCKWISE,
							D2D1_ARC_SIZE_SMALL
						));

					// 나머지 반원을 그려서 원이 되게 그림
					pSink->AddArc(
						D2D1::ArcSegment(
							D2D1::Point2F(-165, 0), 
							D2D1::SizeF(165, 165),  
							0.0f,
							D2D1_SWEEP_DIRECTION_CLOCKWISE,
							D2D1_ARC_SIZE_SMALL
						));

					pSink->EndFigure(D2D1_FIGURE_END_OPEN);
				
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
			}

			if (SUCCEEDED(hr))
			{

				hr = m_pD2DFactory->CreatePathGeometry(&m_pAnimationGeometry1);
				hr = m_pAnimationGeometry1->Open(&pSink);
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				// 50씩 반지름을 줄이면서 장애물 박스의 원 경로를 그림
				pSink->BeginFigure(
					D2D1::Point2F(-100, 0), // Start point of the top half circle
					D2D1_FIGURE_BEGIN_FILLED
				);

				// Add the top half circle
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(100, 0), // end point of the top half circle, also the start point of the bottom half circle
						D2D1::SizeF(100, 100), // radius
						0.0f, // rotation angle
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));

				// Add the bottom half circle
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(-100, 0), // end point of the bottom half circle
						D2D1::SizeF(100, 100),   // radius of the bottom half circle, same as previous one.
						0.0f, // rotation angle
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				hr = pSink->Close();
				SAFE_RELEASE(pSink);
			}

			if (SUCCEEDED(hr))
			{

				hr = m_pD2DFactory->CreatePathGeometry(&m_pAnimationGeometry2);
				hr = m_pAnimationGeometry2->Open(&pSink);
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(-35, 0), // Start point of the top half circle
					D2D1_FIGURE_BEGIN_FILLED
				);

				// Add the top half circle
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(35, 0), // end point of the top half circle, also the start point of the bottom half circle
						D2D1::SizeF(35, 35), // radius
						0.0f, // rotation angle
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));

				// Add the bottom half circle
				pSink->AddArc(
					D2D1::ArcSegment(
						D2D1::Point2F(-35, 0), // end point of the bottom half circle
						D2D1::SizeF(35, 35),   // radius of the bottom half circle, same as previous one.
						0.0f, // rotation angle
						D2D1_SWEEP_DIRECTION_CLOCKWISE,
						D2D1_ARC_SIZE_SMALL
					));

				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				hr = pSink->Close();
				SAFE_RELEASE(pSink);
			}
		}
	}
	return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"background", L"PNG", &backgroundBitmap);
			}

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"floor", L"PNG", &floorBitmap);
			}

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"box", L"PNG", &boxBitmap);
			}

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"mybox", L"PNG", &myboxBitmap);
			}

			if (SUCCEEDED(hr))
			{
				hr = LoadBitmapFromResource(m_pRenderTarget, m_pWICFactory, L"heart", L"PNG", &lifeBitmap);
			}

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				hr = m_pRenderTarget->CreateBitmapBrush(backgroundBitmap, propertiesXClampYClamp, &backgroundBitmapBrush);
			}

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				hr = m_pRenderTarget->CreateBitmapBrush(floorBitmap, propertiesXClampYClamp, &floorBitmapBrush);
			}

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				hr = m_pRenderTarget->CreateBitmapBrush(myboxBitmap, propertiesXClampYClamp, &myboxBitmapBrush);
			}

			if (SUCCEEDED(hr))
			{
				D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

				hr = m_pRenderTarget->CreateBitmapBrush(lifeBitmap, propertiesXClampYClamp, &lifeBitmapBrush);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.f), &m_pFloorBrush);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.f), &m_pGreenBrush);
			}

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 1.f), &m_pBlueBrush);
			}

			if (SUCCEEDED(hr))
			{
				// 좌측 상단에 텍스트 출력을 위한 빨간색 브러시
				hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &textBrush);
			}

			if (SUCCEEDED(hr))
			{
				// 방사형 계조 붓으로 불투명 마스크를 적용
				ID2D1GradientStopCollection* pGradientStops = NULL;

				static const D2D1_GRADIENT_STOP gradientStops[] =
				{
					{   0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f)  },
					{   1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f)  },
				};

				hr = m_pRenderTarget->CreateGradientStopCollection(gradientStops, 2, &pGradientStops);

				if (SUCCEEDED(hr))
				{
					hr = m_pRenderTarget->CreateRadialGradientBrush(
						D2D1::RadialGradientBrushProperties(D2D1::Point2F(300, 300), D2D1::Point2F(0, 0), 400, 400),
						pGradientStops, &m_pRadialGradientBrush);
				}
				pGradientStops->Release();
			}
		}
	}
	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBlackBrush);
	SAFE_RELEASE(backgroundBitmap);
	SAFE_RELEASE(backgroundBitmapBrush);
	SAFE_RELEASE(m_pRadialGradientBrush);
	SAFE_RELEASE(m_pRectGeo);
	SAFE_RELEASE(floorBitmap);
	SAFE_RELEASE(floorBitmapBrush);
	SAFE_RELEASE(myboxBitmap);
	SAFE_RELEASE(boxBitmap);
	SAFE_RELEASE(boxBitmapBrush);
	SAFE_RELEASE(myboxBitmapBrush);
	SAFE_RELEASE(m_pSunGeometry);
	SAFE_RELEASE(m_pFloorBrush);
	SAFE_RELEASE(squareGeometry);
	SAFE_RELEASE(m_pGreenBrush);
	SAFE_RELEASE(m_pBlueBrush);
	SAFE_RELEASE(textBrush);
	SAFE_RELEASE(textFormat);
	SAFE_RELEASE(lifeFormat);
	SAFE_RELEASE(keyFormat);

}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{

		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		D2D1_MATRIX_3X2_F Matrix;
		D2D1_MATRIX_3X2_F Matrix1;
		D2D1_MATRIX_3X2_F Matrix2;
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 600;

		// 중앙지점으로 이동
		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);
		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(rtSize.width / 2, rtSize.height / 2);

		m_pRenderTarget->BeginDraw();

		// 렌더타겟 초기화
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));


		for (int i = 0; i < life; i++) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			m_pRenderTarget->DrawBitmap(lifeBitmap, D2D1::RectF(0+(i*50), 0, 50+(i * 50), 50));
		}

		// 배경화면이 맨 나중에 오면 위에 배경화면이 덮어져 배경화면만 그려지므로 배경이 맨 밑에 그려지게 우선 그림
		m_pRenderTarget->FillGeometry(m_pRectGeo, backgroundBitmapBrush, m_pRadialGradientBrush);
		m_pRenderTarget->DrawRectangle(D2D1::RectF(0, 0, 600, 600), m_pBlackBrush, 1.f);

		// 그 위에는 바닥을 그리고
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->DrawBitmap(floorBitmap, D2D1::RectF(0, 0, 600, 600));

		// 경로 기하로 그렸던 태양을 그림
		m_pRenderTarget->DrawGeometry(m_pSunGeometry, m_pFloorBrush, 1.f);

		static float anim_time = 0.0f;

		float length = m_Animation.GetValue(anim_time);

		// 애니메이션의 현재 시간에 해당하는 기하 길이에 일치하는 이동 동선 상의 지점을 얻음.
		m_pAnimationGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

		//  방향을 조절하여 이동 동선을 따라가는 방향이 되도록 함.
		Matrix = D2D1::Matrix3x2F(
			tangent.x, tangent.y,
			-tangent.y, tangent.x,
			point.x, point.y);

		m_pRenderTarget->SetTransform(Matrix * scale * translation);

		float tempboxX = box.x - 300;
		float tempboxY = box.y - 300;

		if (start || clear) {

		}
		else if (collisionOX(tempboxX, tempboxY, point.x, point.y)) {

				die->Stop();
				die->FlushSourceBuffers();
				die->SubmitSourceBuffer(&die_buffer);
				die->Start();

				if (life <= 0) {
					gameover->Start();
					gameover->SubmitSourceBuffer(&gameover_buffer);
				}

				box.x = 110.0f;
				box.y = 274.5f;
		}
		


		D2D1_SIZE_F size = boxBitmap->GetSize();
		D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0F, 0.0F);
		m_pRenderTarget->DrawBitmap(
			boxBitmap,
			D2D1::RectF(
				-size.width / 2, -size.height / 2,
				size.width / 2, size.height / 2
			)
		);

		{
			float length1 = m_Animation1.GetValue(anim_time);
			D2D1_POINT_2F point1;
			D2D1_POINT_2F tangent1;

			m_pAnimationGeometry1->ComputePointAtLength(length1, NULL, &point1, &tangent1);

			Matrix1 = D2D1::Matrix3x2F(
				tangent1.x, tangent1.y,
				-tangent1.y, tangent1.x,
				point1.x, point1.y);
			
			m_pRenderTarget->SetTransform(Matrix1 * scale * translation);

			if (collisionOX(tempboxX, tempboxY, point1.x, point1.y)) {

				die->Stop();
				die->FlushSourceBuffers();
				die->SubmitSourceBuffer(&die_buffer);
				die->Start();
				
				if (life <= 0) {
					gameover->Start();
					gameover->SubmitSourceBuffer(&gameover_buffer);
				}

				box.x = 110.0f;
				box.y = 274.5f;
			}

			D2D1_SIZE_F size = boxBitmap->GetSize();
			D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0F, 0.0F);
			m_pRenderTarget->DrawBitmap(
				boxBitmap,
				D2D1::RectF(
					-size.width / 2, -size.height / 2,
					size.width / 2, size.height / 2
				)
			);
		}

		{
			float length2 = m_Animation2.GetValue(anim_time);
			D2D1_POINT_2F point2;
			D2D1_POINT_2F tangent2;

			m_pAnimationGeometry2->ComputePointAtLength(length2, NULL, &point2, &tangent2);

			Matrix2 = D2D1::Matrix3x2F(
				tangent2.x, tangent2.y,
				-tangent2.y, tangent2.x,
				point2.x, point2.y);

			m_pRenderTarget->SetTransform(Matrix2 * scale * translation);

			if (collisionOX(tempboxX, tempboxY, point2.x, point2.y)) {
				box.x = 110.0f;
				box.y = 274.5f;
			}

			D2D1_SIZE_F size = boxBitmap->GetSize();
			D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.0F, 0.0F);
			m_pRenderTarget->DrawBitmap(
				boxBitmap,
				D2D1::RectF(
					-size.width / 2, -size.height / 2,
					size.width / 2, size.height / 2
				)
			);
		}


		// 자신 캐릭터를 그림
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(box.x, box.y));
		m_pRenderTarget->FillRectangle(D2D1::RectF(-10, -10, 10, 10), myboxBitmapBrush);

		// 사운드 출력
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		_swprintf(Text, L"현재 사운드 : %.2f   (F1 : 소리 줄이기, F2 : 소리 올리기, F3 : 음소거)", volume*10);
		m_pRenderTarget->DrawText(
			Text,									// WCHAR* 문자열
			100,                                             // 문자의 개수
			textFormat,                                    // IDWriteTextFormat 텍스트 포맷 
			D2D1::RectF(0, rtSize.width - 15, rtSize.width, rtSize.height),   // 그려질 영역의 크기와 위치.
			textBrush                                      // 붓(brush)
		);

		if (life <= 0) {
			// 게임오버 출력
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			_swprintf(Text1, L"GameOver");
			m_pRenderTarget->DrawText(
				Text1,									// WCHAR* 문자열
				100,                                             // 문자의 개수
				textFormat,                                    // IDWriteTextFormat 텍스트 포맷 
				D2D1::RectF(0, rtSize.width - 30, rtSize.width, rtSize.height),   // 그려질 영역의 크기와 위치.
				textBrush                                      // 붓(brush)
			);
		}else if (clear) {
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			_swprintf(Text1, L"GameClear");
			m_pRenderTarget->DrawText(
				Text1,									// WCHAR* 문자열
				100,                                             // 문자의 개수
				textFormat,                                    // IDWriteTextFormat 텍스트 포맷 
				D2D1::RectF(0, rtSize.width - 30, rtSize.width, rtSize.height),   // 그려질 영역의 크기와 위치.
				textBrush                                      // 붓(brush)
			);
		}

		hr = m_pRenderTarget->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		// 애니메이션의 끝에 도달하면 다시 처음으로 되돌려서 반복되도록 함.
		if (anim_time >= m_Animation.GetDuration())
		{
			anim_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;
			anim_time += elapsedTime;
		}
	}
	return hr;
}

HRESULT DemoApp::LoadWave(LPWSTR filename, IXAudio2SourceVoice** ppsv, XAUDIO2_BUFFER* buf) {
	HRESULT hr;

	CWaveFile wav;
	hr = wav.Open(filename, nullptr, WAVEFILE_READ);
	if (SUCCEEDED(hr)) {
		buf->AudioBytes = wav.GetSize();
		buf->Flags = XAUDIO2_END_OF_STREAM;
		BYTE* pbWaveData = new BYTE[buf->AudioBytes];
		buf->pAudioData = pbWaveData;
		hr = wav.Read(
			pbWaveData,
			buf->AudioBytes,
			reinterpret_cast<DWORD*>(&buf->AudioBytes)
		);
		wav.Close();
		hr = m_pXAudio2->CreateSourceVoice(ppsv, wav.GetFormat());
		(*ppsv)->Start();
	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}


LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
				{
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					box.x = 110.0f;
					box.y = 274.5f;
					pDemoApp->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;

			case WM_DISPLAYCHANGE:
				{
					InvalidateRect(hwnd, NULL, FALSE);
				}
				result = 0;
				wasHandled = true;
				break;

			case WM_PAINT:
				{
					pDemoApp->OnRender();

				}
				result = 0;
				wasHandled = true;
				break;

			case WM_DESTROY:
				{
					PostQuitMessage(0);
				}
				result = 1;
				wasHandled = true;
				break;

			case WM_KEYDOWN:
			{
				pDemoApp->m_codeVK = wParam;
				switch (wParam) {
				case VK_UP:
					box.y -= 3;
					boxYposition(box.x,box.y);
					pDemoApp->move->Stop();
					pDemoApp->move->FlushSourceBuffers();
					pDemoApp->move->Start();
					pDemoApp->move->SubmitSourceBuffer(&pDemoApp->move_buffer);
					break;

				case VK_DOWN:
					box.y += 3;
					boxYposition(box.x, box.y);
					pDemoApp->move->Stop();
					pDemoApp->move->FlushSourceBuffers();
					pDemoApp->move->Start();
					pDemoApp->move->SubmitSourceBuffer(&pDemoApp->move_buffer);
					break;

				case VK_LEFT:
					box.x -= 3;
					boxXposition(box.x, box.y);

					if (startOX(box.x)) {
						start = true;
					}
					else {
						start = false;
					}

					if (clearOX(box.x)&&!clear) {
						pDemoApp->victory->Stop();
						pDemoApp->victory->FlushSourceBuffers();
						pDemoApp->victory->Start();
						pDemoApp->victory->SubmitSourceBuffer(&pDemoApp->victory_buffer);
						clear = true;
					}
					pDemoApp->move->Stop();
					pDemoApp->move->FlushSourceBuffers();
					pDemoApp->move->Start();
					pDemoApp->move->SubmitSourceBuffer(&pDemoApp->move_buffer);
					break;

				case VK_RIGHT:
					box.x += 3;
					boxXposition(box.x, box.y);

					if (startOX(box.x)) {
						start = true;
					}
					else {
						start = false;
					}

					if (clearOX(box.x) && !clear) {
						pDemoApp->victory->Stop();
						pDemoApp->victory->FlushSourceBuffers();
						pDemoApp->victory->Start();
						pDemoApp->victory->SubmitSourceBuffer(&pDemoApp->victory_buffer);
						clear = true;
					}
					pDemoApp->move->Stop();
					pDemoApp->move->FlushSourceBuffers();
					pDemoApp->move->Start();
					pDemoApp->move->SubmitSourceBuffer(&pDemoApp->move_buffer);
					break;

				case VK_F2:
					if (volume <= 2.0) {
						volume += 0.1;
						volumeOX = false;
					}
					pDemoApp->soundupdown->SetVolume(volume);
					break;

				case VK_F1:
					if (volume >= 0) {
						volume -= 0.1;
						volumeOX = false;
					}
					pDemoApp->soundupdown->SetVolume(volume);
					break;

				case VK_F3:
					if (volumeOX) {
						volume = tempVolume;
					}
					else {
						tempVolume = volume;
						volume = 0;
					}
					
					pDemoApp->soundupdown->SetVolume(volume);
					volumeOX = !volumeOX;
					break;
				}

				pDemoApp->OnRender();
				InvalidateRect(hwnd, NULL, FALSE);
				return 0;
			}
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}

HRESULT DemoApp::LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);

	return hr;
}
