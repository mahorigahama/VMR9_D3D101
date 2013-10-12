#pragma once

#include "resource.h"

class CMyVideoWindow : public CWindowImpl<CMyVideoWindow> 
{
	CDGrph m_DGrph;
	CComPtr<IGraphBuilder> m_Graph;
	CComPtr<IBaseFilter> m_Vmr;
	CComPtr<CComObject<CD3D101VmrAllocator> > m_VmrAlloc;
	
	BEGIN_MSG_MAP(CMyWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
	HRESULT BuildGraph(LPCWSTR filename);
};

class CVmrTestApp : public CAtlExeModuleT<CVmrTestApp> {
	CMyVideoWindow m_win;
public:
	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw();
	HRESULT PreMessageLoop(int nShowCmd) throw();
};
