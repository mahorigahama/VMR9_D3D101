#include "StdAfx.h"
#include "DGrph.h"
#include "D3D101VmrAllocator.h"

// {F6810BC0-FB04-49ae-BAA9-CAF0D0874C1F}
static const GUID SURFACE_PRIVATE_DATA_GUID = 
{ 0xf6810bc0, 0xfb04, 0x49ae,
{ 0xba, 0xa9, 0xca, 0xf0, 0xd0, 0x87, 0x4c, 0x1f } };

HRESULT CD3D101VmrAllocator::FinalConstruct()
{
	m_DGrph = NULL;
	return S_OK;
}


void CD3D101VmrAllocator::SetDGrph(CDGrph * dgrph)
{
	m_DGrph = dgrph;
}

// IVMRSurfaceAllocator9
STDMETHODIMP CD3D101VmrAllocator::InitializeDevice(DWORD_PTR dwUserID,
	VMR9AllocationInfo * lpAllocInfo, DWORD * lpNumBuffers)
{
	ATLASSERT(lpAllocInfo->dwFlags == VMR9AllocFlag_3DRenderTarget);
	HRESULT hr;
	m_Surfaces.SetCount(*lpNumBuffers);
	m_SharedHandle.SetCount(*lpNumBuffers);
	m_SharedTexture10.SetCount(*lpNumBuffers);
	m_D2DTarget.SetCount(*lpNumBuffers);
	m_Brush.SetCount(*lpNumBuffers);
	m_TextFormat.SetCount(*lpNumBuffers);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	for (size_t n = 0;n < *lpNumBuffers; n++) {
		hr = m_DGrph->GetDevice9Ex()->CreateRenderTarget(
			lpAllocInfo->dwWidth, lpAllocInfo->dwHeight, D3DFMT_A8R8G8B8,
			D3DMULTISAMPLE_NONE, 0, TRUE, &m_Surfaces.GetAt(n),
			&m_SharedHandle.GetAt(n));
		if (FAILED(hr)) return hr;
		m_Surfaces.GetAt(n)->SetPrivateData(SURFACE_PRIVATE_DATA_GUID,
			(void *)&n, sizeof(size_t), 0);
		m_DGrph->GetDevice101()->OpenSharedResource(m_SharedHandle.GetAt(n),
			__uuidof(ID3D10Texture2D),
			reinterpret_cast<void **>(&m_SharedTexture10.GetAt(n)));
		CComQIPtr<IDXGISurface> dxgi_surf(m_SharedTexture10.GetAt(n));
		m_DGrph->GetD2D1Factory()->CreateDxgiSurfaceRenderTarget(dxgi_surf,
			&props, &m_D2DTarget.GetAt(n));
		m_D2DTarget.GetAt(n)->CreateSolidColorBrush(
			D2D1::ColorF(0x55ff55, 1.0f), &m_Brush.GetAt(n));
		m_DGrph->GetDWriteFactory()->CreateTextFormat(L"Consolas", NULL,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			50.f, L"en-US", &m_TextFormat.GetAt(n));
	}
	m_DGrph->GetDevice101()->IASetPrimitiveTopology(
		D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return S_OK;
}

STDMETHODIMP CD3D101VmrAllocator::TerminateDevice(DWORD_PTR dwID)
{
	ReleaseResources();
	return S_OK;
}

STDMETHODIMP CD3D101VmrAllocator::GetSurface(DWORD_PTR dwUserID,
	DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface)
{
	CComCritSecLock<CComMultiThreadModel::AutoCriticalSection> lck(m_CS);
	*lplpSurface = m_Surfaces.GetAt(SurfaceIndex);
	(*lplpSurface)->AddRef();
	return S_OK;
}

STDMETHODIMP CD3D101VmrAllocator::AdviseNotify(
	IVMRSurfaceAllocatorNotify9 * lpIVMRSurfAllocNotify)
{
	CComCritSecLock<CComMultiThreadModel::AutoCriticalSection> lck(m_CS);
	m_SurfAllocNotify = lpIVMRSurfAllocNotify;
	HMONITOR monitor =
		m_DGrph->GetD3D9Ex()->GetAdapterMonitor(D3DADAPTER_DEFAULT);
	return m_SurfAllocNotify->SetD3DDevice(m_DGrph->GetDevice9Ex(), monitor);
}

// IVMRImagePresenter9
STDMETHODIMP CD3D101VmrAllocator::StartPresenting(DWORD_PTR dwUserID)
{
	return S_OK;
}

STDMETHODIMP CD3D101VmrAllocator::StopPresenting(DWORD_PTR dwUserID)
{
	return S_OK;
}

STDMETHODIMP CD3D101VmrAllocator::PresentImage(DWORD_PTR dwUserID,
	VMR9PresentationInfo * lpPresInfo)
{
	HRESULT hr;
	CComCritSecLock<CComMultiThreadModel::AutoCriticalSection> lck(m_CS);
	IDirect3DSurface9 * src_surface = lpPresInfo->lpSurf;
	ID3D10Texture2D * back_buffer = m_DGrph->GetBackBuffer();
	D3DSURFACE_DESC src_desc;
	src_surface->GetDesc(&src_desc);
	D3D10_TEXTURE2D_DESC dst_desc;
	back_buffer->GetDesc(&dst_desc);

	const DWORD a = timeGetTime();
	const DWORD b = timeGetTime() - a;
	D3DLOCKED_RECT locked_rect;
	src_surface->LockRect(&locked_rect, NULL, D3DLOCK_READONLY);
	src_surface->UnlockRect();

	// Use Direct2D and DirectWrite
	DWORD size_of_data = sizeof(size_t);
	size_t locked_surf_num;
	src_surface->GetPrivateData(SURFACE_PRIVATE_DATA_GUID,
		(void*)&locked_surf_num, &size_of_data);

	D2D1_ROUNDED_RECT rounded_rect = { {(FLOAT)10.f, (FLOAT)10.f,
		(FLOAT)src_desc.Width - 10.f, (FLOAT)src_desc.Height - 10.f},
		20.f, 20.f};
	ID2D1RenderTarget * d2d_target = m_D2DTarget.GetAt(locked_surf_num);
	d2d_target->BeginDraw();
	d2d_target->DrawRoundedRectangle(rounded_rect,
		m_Brush.GetAt(locked_surf_num), 10.f);
	D2D1_RECT_F layout_rect = D2D1::RectF(30.f, 30.f,
		(FLOAT)src_desc.Width, (FLOAT)src_desc.Height);
	CAtlString text;
	text.Format(_T("PLAY % 9I64d %d (%d:%d)"),
		lpPresInfo->rtStart / 10000, b,
		lpPresInfo->szAspectRatio.cx, lpPresInfo->szAspectRatio.cy);
	d2d_target->DrawText(text, text.GetLength(),
		m_TextFormat.GetAt(locked_surf_num), layout_rect,
		m_Brush.GetAt(locked_surf_num));
	d2d_target->EndDraw();

	D3D10_BOX src_box = {0}, dst_box = {0};
	src_box.right = src_desc.Width;
	src_box.bottom = src_desc.Height;
	dst_box.right = dst_desc.Width;
	dst_box.bottom = dst_desc.Height;
	D3DX10_TEXTURE_LOAD_INFO tli;
	tli.pSrcBox = &src_box;
	tli.pDstBox = &dst_box;
	tli.SrcFirstMip  = D3D10CalcSubresource(0, 0, 1);
	tli.DstFirstMip  = D3D10CalcSubresource(0, 0, 1);
	tli.NumMips = 1;
	tli.SrcFirstElement = 0;
	tli.DstFirstElement = 0;
	tli.NumElements = 0;
	tli.Filter = D3DX10_FILTER_POINT;
	tli.MipFilter = D3DX10_FILTER_POINT;
	D3DX10LoadTextureFromTexture(
		m_SharedTexture10.GetAt(locked_surf_num), &tli, back_buffer);
	hr = m_DGrph->GetSwapChain()->Present(0, 0);
	// TODO : Present が失敗した、デバイスの復帰などを行う必要がある
	return hr;
}

void CD3D101VmrAllocator::ReleaseResources()
{
	CComCritSecLock<CComMultiThreadModel::AutoCriticalSection> lck(m_CS);
	m_Surfaces.RemoveAll();
	m_SharedTexture10.RemoveAll();
	m_D2DTarget.RemoveAll();
	m_Brush.RemoveAll();
	m_TextFormat.RemoveAll();
}
