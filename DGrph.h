#pragma once

class CDGrph
{
	HWND m_Window;
	CComPtr<IDirect3D9Ex> m_D3D9Ex;
	CComPtr<IDirect3DDevice9Ex> m_Device9Ex;
	CComPtr<ID3D10Device1> m_Device101;
	CComPtr<IDXGISwapChain> m_SwapChain;
	CComPtr<ID3D10Texture2D> m_BackBuffer;
	CComPtr<ID2D1Factory> m_D2D1Factory;
	CComPtr<IDWriteFactory> m_DWriteFactory;
public:
	CDGrph(void);
	HRESULT CreateDevice(HWND window);
	void ReleaseResources();
	IDirect3D9Ex * GetD3D9Ex() const { return m_D3D9Ex; }
	IDirect3DDevice9Ex * GetDevice9Ex() const { return m_Device9Ex; }
	ID3D10Device1 * GetDevice101() const { return m_Device101; }
	ID3D10Texture2D * GetBackBuffer() const { return m_BackBuffer; }
	IDXGISwapChain * GetSwapChain() const { return m_SwapChain; }
	ID2D1Factory * GetD2D1Factory() const { return m_D2D1Factory; }
	IDWriteFactory * GetDWriteFactory() const { return m_DWriteFactory; }
private:
	void CreateDXGISwapDesc(DXGI_SWAP_CHAIN_DESC &dscd) const;
	void CreatePresParam9(D3DPRESENT_PARAMETERS &pp) const;
};
