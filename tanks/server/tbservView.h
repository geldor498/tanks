// tbservView.h : interface of the CTBServView class
//
#pragma once

#include "models.h"
#include "TankInfoDlg.h"

#define REDRAWTIMER 1

class CTBServView : public CView
{
private:
	// OpenGL rendering context
	HGLRC m_hRC;					
	double m_fProjectiveAngle;		
	double m_fAspect;
	double m_fNear;
	double m_fFar;
	double m_shiftZ;		
	double m_shiftY;
	double m_maxshiftY;
	GLsizei m_width;
	GLsizei m_height;
protected:

	CFPoint3D m_shift;		// view position at the map
	double m_rotate;		// view direction over ground	
	double m_r1;			// view angle
	//double m_scale;			// ground scale

	UINT_PTR m_redrawtimerid;		// redraw timer

	CGameView m_gameview;			// game view. main context

	CTankInfoDlg m_infodlg;
protected: // create from serialization only
	CTBServView();
	DECLARE_DYNCREATE(CTBServView)

// Attributes
public:
	CTBServDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	void CreateContext(HDC _hdc,const CRect& _rc);
	BOOL SetupPixelFormat(HDC _hdc);
	//void CreateContext(HDC _hdc,const CRect& _rc);
	void postdraw();
	void GetOGLPos(int _x, int _y,double _zbase,CFPoint3D& _pt);

	virtual void draw();
	virtual void resize(GLsizei _width, GLsizei _height);
	virtual void init(GLsizei _width, GLsizei _height);

	void prepare_draw();
	void draw_scene();
// Implementation
public:
	virtual ~CTBServView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	struct DrawPrePost
	{
		CTBServView* m_pthis;

		DrawPrePost(CTBServView* _pthis)
			:m_pthis(_pthis)
		{
			//m_pthis.predraw();
		}
		~DrawPrePost()
		{
			m_pthis->postdraw();
		}
	};
	void move(double _dx);
	void movelr(double _dx);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGameStart();
	afx_msg void OnGamePause();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewInformation();
};//class CTBServView

#ifndef _DEBUG  // debug version in tbservView.cpp
inline CTBServDoc* CTBServView::GetDocument() const
   { return reinterpret_cast<CTBServDoc*>(m_pDocument); }
#endif

inline
void CTBServView::postdraw()
{
	SwapBuffers(wglGetCurrentDC());
}

