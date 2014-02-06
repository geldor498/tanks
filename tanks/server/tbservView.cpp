// tbservView.cpp : implementation of the CTBServView class
//

#include "stdafx.h"
#include "servapp.h"

#include "tbservDoc.h"
#include "tbservView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTBServView

IMPLEMENT_DYNCREATE(CTBServView, CView)

BEGIN_MESSAGE_MAP(CTBServView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_GAME_START, &CTBServView::OnGameStart)
	ON_COMMAND(ID_GAME_PAUSE, &CTBServView::OnGamePause)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_INFORMATION, &CTBServView::OnViewInformation)
END_MESSAGE_MAP()

// CTBServView construction/destruction

CTBServView::CTBServView()
:
	m_hRC(NULL)
	,m_rotate(0)
	,m_r1(17)
	,m_fProjectiveAngle(45.0)
	,m_fAspect(1.0)
	,m_fNear(20)
	,m_fFar(3000)
	,m_width(1)
	,m_height(1)
	,m_shiftZ(-160)
	,m_shiftY(-30.0)
	,m_maxshiftY(-5)
{
}

CTBServView::~CTBServView()
{
}

BOOL CTBServView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTBServView drawing

void CTBServView::OnDraw(CDC* /*pDC*/)
{
	CTBServDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	draw();
	// TODO: add draw code for native data here
}


// CTBServView diagnostics

#ifdef _DEBUG
void CTBServView::AssertValid() const
{
	CView::AssertValid();
}

void CTBServView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTBServDoc* CTBServView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTBServDoc)));
	return (CTBServDoc*)m_pDocument;
}
#endif //_DEBUG


// CTBServView message handlers

int CTBServView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	HDC hdc = GetDC()->GetSafeHdc();
	CRect rc;
	GetClientRect(&rc);
	CreateContext(hdc,rc);
	m_redrawtimerid = SetTimer(REDRAWTIMER,100,NULL);

	m_infodlg.Create(m_infodlg.IDD);
	return 0;
}

void CTBServView::OnDestroy()
{
	m_infodlg.DestroyWindow();

	KillTimer(m_redrawtimerid);
	m_redrawtimerid = 0;

	wglMakeCurrent(NULL, NULL);
	if(NOT_NULL(m_hRC))
	{
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
	CView::OnDestroy();
}

void CTBServView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	wglMakeCurrent(NULL, NULL);
	if(m_hRC)
	{
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
	HDC hdc = GetDC()->GetSafeHdc();
	CRect rc;
	GetClientRect(&rc);
	CreateContext(hdc,rc);
	//draw();
}

// Set OpenGL pixel format for given DC
BOOL CTBServView::SetupPixelFormat(HDC _hdc)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
		1,                           // version number
		PFD_DRAW_TO_WINDOW|          // support window
		PFD_SUPPORT_OPENGL|          // support OpenGL
		PFD_DOUBLEBUFFER,            // double buffered
		PFD_TYPE_RGBA,               // RGBA type
		24,                          // 24-bit color depth
		0, 0, 0, 0, 0, 0,            // color bits ignored
		0,                           // no alpha buffer
		0,                           // shift bit ignored
		0,                           // no accumulation buffer
		0, 0, 0, 0,                  // accum bits ignored
		32,                          // 32-bit z-buffer
		0,                           // no stencil buffer
		0,                           // no auxiliary buffer
		PFD_MAIN_PLANE,              // main layer
		0,                           // reserved
		0, 0, 0                      // layer masks ignored
	};
	int pixelformat;

	if ((pixelformat = ChoosePixelFormat(_hdc, &pfd)) == 0)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	if (SetPixelFormat(_hdc, pixelformat, &pfd) == FALSE)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}

// Create rendering context given device context and control bounds
void CTBServView::CreateContext(HDC _hdc,const CRect& _rc)
{
	//VERIFY_EXIT(NOT_NULL(m_hRC));
	PIXELFORMATDESCRIPTOR pfd;
	if(!SetupPixelFormat(_hdc))
		return;

	int n = GetPixelFormat(_hdc);
	DescribePixelFormat(_hdc, n, sizeof(pfd), &pfd);
	m_hRC = wglCreateContext(_hdc);
	wglMakeCurrent(_hdc, m_hRC);

	resize(_rc.Width(),_rc.Height());
}

void CTBServView::GetOGLPos(int _x, int _y,double _zbase,CFPoint3D& _pt)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX = 0, winY = 0, winZ = 0;
	GLdouble posX = 0, posY = 0, posZ = 0;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (GLfloat)_x;
	winY = (GLfloat)viewport[3] - (GLfloat)_y;
	//glReadPixels( _x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
	//winZ = (GLfloat)_zbase;

	gluUnProject( winX, winY, 1.0f, modelview, projection, viewport, &posX, &posY, &posZ);

	_pt.x = posX*_zbase/m_fFar;
	_pt.y = posY*_zbase/m_fFar;
	_pt.z = 0;
}

void CTBServView::resize(GLsizei _width, GLsizei _height) 
{
	//CAutoLock<CCritSecLock> lock(m_critseclock);

	if (_height==0)		// Предотвращение деления на ноль, если окно слишком мало
		_height=1;

	glViewport(0, 0, _width, _height);
	// Сброс текущей области вывода и перспективных преобразований
	init(_width,_height);
}

void CTBServView::init(GLsizei _width, GLsizei _height)
{
	//CAutoLock<CCritSecLock> lock(m_critseclock);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Очистка экрана в черный цвет
	glClearDepth(1.0);		// Разрешить очистку буфера глубины
	glDepthFunc(GL_LESS);	// Тип теста глубины
	glEnable(GL_DEPTH_TEST);// разрешить тест глубины
	glShadeModel(GL_SMOOTH);// разрешить плавное цветовое сглаживание
	glMatrixMode(GL_PROJECTION);// Выбор матрицы проекции
	glLoadIdentity();		// Сброс матрицы проекции
	m_width = _width;
	m_height = _height;
	m_fAspect = (double)m_width/m_height;
	gluPerspective((GLfloat)m_fProjectiveAngle,(GLfloat)m_fAspect,(GLfloat)m_fNear,(GLfloat)m_fFar);
	// Вычислить соотношение геометрических размеров для окна
	glMatrixMode(GL_MODELVIEW);// Выбор матрицы просмотра модели
	m_gameview.initialize();
}

void CTBServView::draw()
{
	DrawPrePost __dpp(this);

	prepare_draw();
	draw_scene();
}

void CTBServView::prepare_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Очистка экрана и буфера глубины
	glLoadIdentity();				// Сброс просмотра
	glRotated(m_r1,1,0,0);
	glRotated(m_rotate,0,-1,0);
	glTranslatef(0.0f,(GLfloat)m_shiftY,(GLfloat)m_shiftZ);			// Сдвиг влево и вглубь экрана
	glTranslated(m_shift.x,m_shift.y,m_shift.z);
}

void CTBServView::draw_scene()
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));

	glPushMatrix();
	double scale = singleton<CGameConsts>::get().scale();
	m_gameview.draw_ground();
	m_gameview.draw_tanks(m_rotate);
	m_gameview.draw_artefacts();
	m_gameview.draw_exploits();
	m_gameview.draw_shells();
	glPopMatrix();
}


void CTBServView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	switch(nChar)
	{
	case VK_LEFT: 
		if(GetAsyncKeyState(VK_SHIFT)==0)
		{
			m_rotate += 5;
			minmaxr(m_rotate,0.0,360.0);
		}
		else
		{
			movelr(-5);
		}
		break;
	case VK_RIGHT:
		if(GetAsyncKeyState(VK_SHIFT)==0)
		{
			m_rotate -= 5;
			minmaxr(m_rotate,0.0,360.0);
		}
		else
		{
			movelr(5);
		}
		break;
	case VK_UP:
		if(GetAsyncKeyState(VK_SHIFT)!=0)
		{
			m_shiftY -= fabs(m_shiftY/m_maxshiftY)*0.5; 
		}
		else
		{
			move(10);
		}
		break;
	case VK_DOWN:
		if(GetAsyncKeyState(VK_SHIFT)!=0)
		{
			if(m_shiftY<m_maxshiftY)
			{
				m_shiftY += fabs(m_shiftY/m_maxshiftY)*0.5; 
			}
		}
		else
		{
			move(-10);
		}
		break;
	case VK_HOME:
		m_r1 -= 1;
		break;
	case VK_END:
		m_r1 += 1;
		break;
	}
	draw();
}

void CTBServView::move(double _dx)
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));
	double z = m_shift.z + cos(m_rotate*M_PI/180)*_dx;
	double x = m_shift.x + sin(m_rotate*M_PI/180)*_dx;
	double w = (double)pdoc->m_map.width();
	double h = (double)pdoc->m_map.height();
	double scale = singleton<CGameConsts>::get().scale();
	if(x<-w*0.5*scale || x>w*0.5*scale
		|| z+m_shiftZ<-h*0.5*scale || z+m_shiftZ>h*0.5*scale) 
		return;
	m_shift.x = x;
	m_shift.z = z;
}

void CTBServView::movelr(double _dx)
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));
	double z = m_shift.z + cos((m_rotate-90)*M_PI/180)*_dx;
	double x = m_shift.x + sin((m_rotate-90)*M_PI/180)*_dx;
	double w = (double)pdoc->m_map.width();
	double h = (double)pdoc->m_map.height();
	double scale = singleton<CGameConsts>::get().scale();
	if(x<-w*0.5*scale || x>w*0.5*scale
		|| z+m_shiftZ<-h*0.5*scale || z+m_shiftZ>h*0.5*scale) 
		return;
	m_shift.x = x;
	m_shift.z = z;
}

void CTBServView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));

	switch(lHint)
	{
	case DocUpdate_Link:
		m_gameview.link(&pdoc->m_gameworld,&pdoc->m_map);
		break;
	case DocUpdate_Redraw:
		draw();
		break;
	}
}


void CTBServView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==m_redrawtimerid)
	{
		draw();
		return;
	}
	CView::OnTimer(nIDEvent);
}

void CTBServView::OnGameStart()
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));
	pdoc->start_game();
}

void CTBServView::OnGamePause()
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));
	pdoc->pause_game();
}

BOOL CTBServView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
	//return CView::OnEraseBkgnd(pDC);
}

void CTBServView::OnViewInformation()
{
	CTBServDoc* pdoc = GetDocument();
	VERIFY_EXIT(NOT_NULL(pdoc));
	m_infodlg.populate(pdoc->m_gameworld);
	m_infodlg.ShowWindow(SW_SHOW);
}
