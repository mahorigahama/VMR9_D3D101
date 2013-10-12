#pragma once

class ATL_NO_VTABLE CD3D101VmrAllocator :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IVMRSurfaceAllocator9, IVMRImagePresenter9
{
	BEGIN_COM_MAP(CD3D101VmrAllocator)
		COM_INTERFACE_ENTRY(IVMRSurfaceAllocator9)
		COM_INTERFACE_ENTRY(IVMRImagePresenter9)
	END_COM_MAP()
	DECLARE_PROTECT_FINAL_CONSTRUCT();
private:
	CComMultiThreadModel::AutoCriticalSection m_CS;
	CDGrph * m_DGrph;
	CComPtr<IVMRSurfaceAllocatorNotify9> m_SurfAllocNotify;
	CInterfaceArray<IDirect3DSurface9> m_Surfaces;
	CAtlArray<HANDLE> m_SharedHandle;
	CInterfaceArray<ID3D10Texture2D> m_SharedTexture10;
	CInterfaceArray<ID2D1RenderTarget> m_D2DTarget;
	CInterfaceArray<ID2D1SolidColorBrush> m_Brush;
	CInterfaceArray<IDWriteTextFormat> m_TextFormat;
public:
	HRESULT FinalConstruct();
	void SetDGrph(CDGrph * dgrph);
	// IVMRSurfaceAllocator9
	STDMETHODIMP InitializeDevice(DWORD_PTR dwUserID,
		VMR9AllocationInfo * lpAllocInfo, DWORD * lpNumBuffers);
	STDMETHODIMP TerminateDevice(DWORD_PTR dwID);
	STDMETHODIMP GetSurface(DWORD_PTR dwUserID, DWORD SurfaceIndex,
		DWORD SurfaceFlags, IDirect3DSurface9 ** lplpSurface);
	STDMETHODIMP AdviseNotify(
		IVMRSurfaceAllocatorNotify9 * lpIVMRSurfAllocNotify);
	// IVMRImagePresenter9
	STDMETHODIMP StartPresenting(DWORD_PTR dwUserID);
	STDMETHODIMP StopPresenting(DWORD_PTR dwUserID);
	STDMETHODIMP PresentImage(DWORD_PTR dwUserID,
		VMR9PresentationInfo *lpPresInfo);
private:
	void ReleaseResources();
};
