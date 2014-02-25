// tbservDoc.cpp : implementation of the CTBServDoc class
//

#include "stdafx.h"
#include "servapp.h"
#include <sys/timeb.h>
#include <time.h>

#include "tbservDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double EPS = 1e-9;
// CTBServDoc

IMPLEMENT_DYNCREATE(CTBServDoc, CDocument)

BEGIN_MESSAGE_MAP(CTBServDoc, CDocument)
END_MESSAGE_MAP()


// CTBServDoc construction/destruction

CTBServDoc::CTBServDoc()
:
	m_gameflag(FALSE,TRUE)
	,m_gameworld(m_map,m_gameflag)
	,m_connectionserver(m_gameworld)
	,m_control(m_gameworld,m_map)
{
	// TODO: add one-time construction code here
	start();
}

CTBServDoc::~CTBServDoc()
{
	predestruct();
}

BOOL CTBServDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	size_t sz = 0;
	m_map.resize(singleton<CGameConsts>::get().width(),singleton<CGameConsts>::get().height());
	m_map.generate(singleton<CGameConsts>::get().width(),singleton<CGameConsts>::get().rockheight());
	m_map.save();
	m_gameworld.generate_artefacts();

	UpdateAllViews(NULL,DocUpdate_Link);

	return TRUE;
}




// CTBServDoc serialization

void CTBServDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CTBServDoc diagnostics

#ifdef _DEBUG
void CTBServDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTBServDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTBServDoc commands

DWORD CTBServDoc::thread_main()
{
	struct _timeb t1,t2;
	_ftime64_s(&t1);
	for(;!is_aborted();)
	{
		DWORD dwWait = WaitForSingleObject(m_gameflag,100);
		if(dwWait==WAIT_OBJECT_0)
		{
			_ftime64_s(&t2);
			double dt = t2.time + t2.millitm/1000.0
				- (t1.time + t1.millitm/1000.0)
				;
			t1 = t2;
			m_control.process_commands(dt);
		}
		else
		{
			_ftime64_s(&t1);
		}
	}
	return 0;
}

void CTBServDoc::start_game()
{
	m_gameflag.SetEvent();
}

void CTBServDoc::pause_game()
{
	m_gameflag.ResetEvent();
}
