// tbservDoc.h : interface of the CTBServDoc class
//
#pragma once

#include <utils/thread.h>
#include "MapGenerator.h"
#include "connectserver.h"
#include "gameworld.h"

enum DocUpdateEn
{
	DocUpdate_Null = 0
	,DocUpdate_Link
	,DocUpdate_Redraw
};

class CTBServDoc 
:
	public CDocument
	,public CWorkedThreadImpl<CTBServDoc>
{
protected: // create from serialization only
	CTBServDoc();
	DECLARE_DYNCREATE(CTBServDoc)

// Attributes
protected:
public:
	CGameMap m_map;
	CConnectionServer m_connectionserver;
	CGameWorld m_gameworld;
	CControl m_control;
	CEvent m_gameflag;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CTBServDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	DWORD thread_main();
	void start_game();
	void pause_game();

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


