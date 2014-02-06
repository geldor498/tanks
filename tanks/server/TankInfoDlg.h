#pragma once

#include "gameworld.h"
#include "afxcmn.h"
#include <controls/ListHelper.h>
#include <controls/listvbase.h>

struct CTankInfoListData 
	:public CListCBase
	,public GridDataSource
{
	virtual void GetString(int _row,int _col,CString& _str);
	virtual void GetImage(int _row,int _col,int& _nImage);
	virtual void GetColumnHeader(int _col,CString& _str,int& _fmt);
	virtual int GetRows();
	virtual int GetCols();
	virtual void GetColumnInfo(int _rowi,int _coli,ColumnInfoTag& cit);
	virtual void GetParam(int _row,LPARAM& lParam);
	virtual void GetImage(int _row,int& nStateImg);
	virtual bool GetColors(int _row,int _col,COLORREF _clrs[ColorsSNS_MAX][ColorsBF_MAX]);
	virtual bool GetDrawMode(int _row,DrawModeEn& _drawmode);
	virtual void GetRowID(int _row,Utils::ItemID*& _id);
	virtual bool GetDescription(int _row,int _col,CString& _str,bool& _bshowalways);
	virtual void set_GrigProperties();
	virtual GridDataSource* get_GridDataSource();

	void populate(CGameWorld& _gw);
	const TanksInfoLst& get_tanksinfo() const {return m_data;}
protected:
	TanksInfoLst m_data;
};

struct CTeamInfoLI
{
	CString m_sTeamName;
	double m_fLifes;

	CTeamInfoLI(const CString& _sTeamName = _T(""),double _fLifes = 0)
		:m_sTeamName(_sTeamName)
		,m_fLifes(_fLifes)
	{
	}
};

typedef std::list<CTeamInfoLI> TeamInfoLst;

struct CTeamInfoLstData
	:public CListCBase
	,public GridDataSource
{
	virtual void GetString(int _row,int _col,CString& _str);
	virtual void GetImage(int _row,int _col,int& _nImage);
	virtual void GetColumnHeader(int _col,CString& _str,int& _fmt);
	virtual int GetRows();
	virtual int GetCols();
	virtual void GetColumnInfo(int _rowi,int _coli,ColumnInfoTag& cit);
	virtual void GetParam(int _row,LPARAM& lParam);
	virtual void GetImage(int _row,int& nStateImg);
	virtual bool GetColors(int _row,int _col,COLORREF _clrs[ColorsSNS_MAX][ColorsBF_MAX]);
	virtual bool GetDrawMode(int _row,DrawModeEn& _drawmode);
	virtual void GetRowID(int _row,Utils::ItemID*& _id);
	virtual bool GetDescription(int _row,int _col,CString& _str,bool& _bshowalways);
	void populate(const TanksInfoLst& _tanksinfo);
	virtual void set_GrigProperties();
	virtual GridDataSource* get_GridDataSource() {return this;};
protected:
	TeamInfoLst m_data;
};

// CTankInfoDlg dialog

class CTankInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CTankInfoDlg)

public:

	CTankInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTankInfoDlg();

// Dialog Data
	enum { IDD = IDD_TANKSINFODLG };

protected:
	CTankInfoListData m_tanksinfo;
	CTeamInfoLstData m_teamsinfo;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void populate(CGameWorld& _gw);
	afx_msg void OnBnClickedRefresh();
	CGameWorld* m_pgw;
};
