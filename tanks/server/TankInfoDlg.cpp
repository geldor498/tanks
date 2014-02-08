// TankInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "servapp.h"
#include "TankInfoDlg.h"


// CTankInfoDlg dialog

IMPLEMENT_DYNAMIC(CTankInfoDlg, CDialog)

CTankInfoDlg::CTankInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTankInfoDlg::IDD, pParent)
	,m_pgw(NULL)
{
}

CTankInfoDlg::~CTankInfoDlg()
{
}

void CTankInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEAMSINFOLIST, m_teamsinfo);
	DDX_Control(pDX, IDC_TANKSINFOLST, m_tanksinfo);
	m_teamsinfo.InitCtrl();
	m_tanksinfo.InitCtrl();
}


BEGIN_MESSAGE_MAP(CTankInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_REFRESH, &CTankInfoDlg::OnBnClickedRefresh)
END_MESSAGE_MAP()


// CTankInfoDlg message handlers

void CTankInfoListData::populate(CGameWorld& _gw)
{
	_gw.get_tanksinfo(m_data);
}

void CTankInfoListData::GetString(int _row,int _col,CString& _str)
{
	VERIFY_EXIT(_row>=0 && _row<(int)m_data.size());
	TanksInfoLst::const_iterator it = m_data.begin();
	std::advance(it,_row);
	switch(_col)
	{
	case 0:
		_str = it->m_sTeamName;
		break;
	case 1:
		_str = it->m_sTankName;
		break;
	case 2:
		_str.Format(_T("%f"),it->m_fArmor);
		break;
	case 3:
		_str.Format(_T("%f"),it->m_fFuel);
		break;
	}
}

void CTankInfoListData::GetImage(int _row,int _col,int& _nImage)
{
	_nImage = 0;
}

void CTankInfoListData::GetColumnHeader(int _col,CString& _str,int& _fmt)
{
	static const LPCTSTR szHeaders[] = {_T("Team"),_T("Tank"),_T("Armor"),_T("Fuel")};
	static const int Fmt[] = {LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_CENTER,LVCFMT_CENTER};
	VERIFY_EXIT(_col>=0 && _col<_countof(szHeaders) && _col<_countof(Fmt));
	_str = szHeaders[_col];
	_fmt = Fmt[_col];
}

int CTankInfoListData::GetRows()
{
	return (int)m_data.size();
}

int CTankInfoListData::GetCols()
{
	return 4;
}

void CTankInfoListData::GetColumnInfo(int _rowi,int _coli,ColumnInfoTag& cit)
{
		cit.m_type = ColumnInfoTag::CT_TEXT;
}

void CTankInfoListData::GetParam(int _row,LPARAM& lParam)
{
	lParam = NULL;
}

void CTankInfoListData::GetImage(int _row,int& nStateImg)
{
	nStateImg = 0;
}

bool CTankInfoListData::GetColors(int _row,int _col,COLORREF _clrs[ColorsSNS_MAX][ColorsBF_MAX])
{
	return false;
}

bool CTankInfoListData::GetDrawMode(int _row,DrawModeEn& _drawmode)
{
	return false;
}

void CTankInfoListData::GetRowID(int _row,Utils::ItemID*& _id)
{
	_id = NULL;
}

bool CTankInfoListData::GetDescription(int _row,int _col,CString& _str,bool& _bshowalways)
{
	return false;
}

BOOL CTankInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTankInfoDlg::populate(CGameWorld& _gw)
{
	m_tanksinfo.populate(_gw);
	m_teamsinfo.populate(m_tanksinfo.get_tanksinfo());
	m_tanksinfo.refresh();
	m_teamsinfo.refresh();
	m_pgw = &_gw;
}

void CTeamInfoLstData::populate(const TanksInfoLst& _tanksinfo)
{
	m_data.clear();
	TanksInfoLst::const_iterator
		tiit = _tanksinfo.begin()
		,tiite = _tanksinfo.end()
		;
	for(;tiit!=tiite;++tiit)
	{
		CString sTeamName = tiit->m_sTeamName;
		TeamInfoLst::iterator
			it = m_data.begin()
			,ite = m_data.end()
			;
		for(;it!=ite && it->m_sTeamName!=sTeamName;++it){}
		if(it!=ite)
		{
			it->m_fLifes += tiit->m_fArmor;
		}
		else
		{
			m_data.push_back(CTeamInfoLI(sTeamName,tiit->m_fArmor));
		}
	}
}

void CTeamInfoLstData::GetString(int _row,int _col,CString& _str)
{
	VERIFY_EXIT(_row>=0 && _row<(int)m_data.size());
	TeamInfoLst::const_iterator it = m_data.begin();
	std::advance(it,_row);
	switch(_col)
	{
	case 0:
		_str = it->m_sTeamName;
		break;
	case 1:
		_str.Format(_T("%f"),it->m_fLifes);
		break;
	}
}

void CTeamInfoLstData::GetImage(int _row,int _col,int& _nImage)
{
	_nImage = 0;
}

void CTeamInfoLstData::GetColumnHeader(int _col,CString& _str,int& _fmt)
{
	static const LPCTSTR szHeaders[] = {_T("Team"),_T("Lifes")};
	static const int Fmt[] = {LVCFMT_LEFT,LVCFMT_CENTER};
	VERIFY_EXIT(_col>=0 && _col<_countof(szHeaders) && _col<_countof(Fmt));
	_str = szHeaders[_col];
	_fmt = Fmt[_col];
}

int CTeamInfoLstData::GetRows()
{
	return (int)m_data.size();
}

int CTeamInfoLstData::GetCols()
{
	return 2;
}

void CTeamInfoLstData::GetColumnInfo(int _rowi,int _coli,ColumnInfoTag& cit)
{
	cit.m_type = ColumnInfoTag::CT_TEXT;
}

void CTeamInfoLstData::GetParam(int _row,LPARAM& lParam)
{
	lParam = NULL;
}
void CTeamInfoLstData::GetImage(int _row,int& nStateImg)
{
	nStateImg = 0;
}
bool CTeamInfoLstData::GetColors(int _row,int _col,COLORREF _clrs[ColorsSNS_MAX][ColorsBF_MAX])
{
	return false;
}
bool CTeamInfoLstData::GetDrawMode(int _row,DrawModeEn& _drawmode)
{
	return false;
}
void CTeamInfoLstData::GetRowID(int _row,Utils::ItemID*& _id)
{
	_id = NULL;
}
bool CTeamInfoLstData::GetDescription(int _row,int _col,CString& _str,bool& _bshowalways)
{
	return false;
}

void CTankInfoListData::set_GrigProperties()
{
	m_listhelper.set_SortedMode(false);
	m_listhelper.enable_CellEdit(false);
	m_listhelper.enable_ShowFocusAsSelection(false);
	m_listhelper.enable_ShowNoItemMessage(true);
	m_listhelper.set_NoItemMessage(_T("No tanks connected"));
	m_listhelper.set_AddNewLineMode(false);
	m_listhelper.set_CellSelectMode();
	//m_listhelper.set_DrawMode(LDM_DELPHIGRIG);
}

GridDataSource* CTankInfoListData::get_GridDataSource()
{
	return this;
}

void CTeamInfoLstData::set_GrigProperties()
{
	m_listhelper.set_SortedMode(false);
	m_listhelper.enable_CellEdit(false);
	m_listhelper.enable_ShowFocusAsSelection(false);
	m_listhelper.enable_ShowNoItemMessage(true);
	m_listhelper.set_NoItemMessage(_T("No tanks connected"));
	m_listhelper.set_AddNewLineMode(false);
	m_listhelper.set_CellSelectMode();
	//m_listhelper.set_DrawMode(LDM_DELPHIGRIG);
}

void CTankInfoDlg::OnBnClickedRefresh()
{
	VERIFY_EXIT(NOT_NULL(m_pgw));
	populate(*m_pgw);
}
