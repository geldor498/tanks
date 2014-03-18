#if !defined(__FILEUTILS_H__9BFC9D6E_21F1_4666_8670_0FB639CD8C1B)
#define __FILEUTILS_H__9BFC9D6E_21F1_4666_8670_0FB639CD8C1B

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h" 

#include <atlconv.h>
#include <comdef.h>
#include <list>
#include <algorithm>
#include <shlobj.h>
#include <shtypes.h>

#include "utils.h"
#include "convert.h"
#include "translate.h"

/**\page Page_QuickStart_FileUtils Quick start: "File utils in you application"

All file function from utilities library incoming into FileUtils namespace.
Most functions are straightforward to do some simple function. Let examples them.

\code
void example()
{
	// remove set last slash of path
	static const LPCTSTR szPath1 = _T("c:\\temp");
	static const LPCTSTR szPath2 = _T("c:\\temp\\");
	CString_ s1 = szPath1,s2 = szPath2;
	FileUtils::SetLastSlash(s1);
	FileUtils::SetLastSlash(s2);
	ASSERT_(
		s1==_T("c:\\temp\\") 
		&& s2==_T("c:\\temp\\")
		);

	s1 = szPath1;
	s2 = szPath2;
	FileUtils::RemoveLastSlash(s1);
	FileUtils::RemoveLastSlash(s2);
	ASSERT_(
		s1==_T("c:\\temp") 
		&& s2==_T("c:\\temp")
		);

	// verify path functions
	static const LPCTSTR szPath3 = _T("c:\\temp");
	if(FileUtils::IsPathExist(szPath3))
	{
		std::cout << szPath3 << _T(" exist an it may be as a file so a directory") << std::endl;
	}
	if(FileUtils::IsFileExist(szPath3))
	{
		std::cout << szPath3 << _T(" the existing file") << std::endl;
	}
	if(FileUtils::IsDirectory(szPath3))
	{
		std::cout << szPath3 << _T(" the existing directory") << std::endl;
	}
	// splite path 
	CString_ sPath,sFileName,sExt;
	FileUtils::SplitePath(_T("c:\\temp\\f1.txt"),sPath,sFileName,sExt);
	ASSERT_(
		sPath == _T("c:\\temp\\")
		&& sFileName == _T("f1")
		&& sExt == _T(".txt")
		);

	// get path
	static const LPCTSTR szPath4 = _T("c:\\temp\\");
	FileUtils::GetPath(szPath4,sPath);
	ASSERT_(sPath==_T("c:\\temp\\"));
	static const LPCTSTR szPath5 = _T("c:\\temp");
	FileUtils::GetPath(szPath4,sPath);
	ASSERT_(sPath==_T("c:\\"));		// so, now you understand why you should use 
									// the function \ref FileUtils::SetLastSlash()

	// modify relative path. if path starts with "." or ".." then it is relative
	// path and it should be modified
	FileUtils::GetModulePath(NULL,sPath);
	ASSERT_(sPath==_T("c:\\temp\\")); // module was started in c:\temp
	static const LPCTSTR szPath5 = _T(".\\file1.txt");
	static const LPCTSTR szPath6 = _T("c:\\file1.txt");

	sPath = szPath5;
	FileUtils::ModifyRelativePath(NULL,sPath);
	ASSERT_(sPath==_T("c:\\temp\\.\\file1.txt"));
	sPath = szPath6;
	FileUtils::ModifyRelativePath(NULL,sPath);
	ASSERT_(sPath==_T("c:\\file1.txt"));

	// create temp file
	ASSERT_(FileUtils::IsFileExist(_T("c:\\temp\\file_A1")));
	FileUtils::GetTempFileName(_T("c:\\temp\\"),_T("file_"),2,sFileName);
	// sFileName will be m/\"c:\\temp\\file_\"[0-F][0-F]/
	// but not a "c:\\temp\\file_A1" because this file name already exists
	// if in there are all files from period "file_00".."file_FF" in c:\temp 
	// then function FileUtils::GetTempFileName() return FALSE because it is 
	// impossible to create temp file (all possible names already in use)
}

\endcode
*/


/**\defgroup FileUtils Utilities for work with files and directories
This group support base function to work with files and directories.
*/
//@{

/**\brief Namespace of utility functions for files and directories.*/
namespace FileUtils
{
	/**\brief remove last slash symbol in path (function argument _path).
	This function helpfull when we need to process some operation with path as directory.
	\param[in,out] _path -- path to remove last slash symbol from.
	*/
	inline
	void RemoveLastSlash(CString_& _path)
	{
		if(_path.GetLength()>0 && isslash(_path.GetAt(_path.GetLength()-1)))
			_path = _path.Left(_path.GetLength()-1);
	}

	/**\brief set last slash symbol in path
	Most win API function work with directories only they are ending with slash symbol.
	\param[in,out] _path -- path to set last slash symbol.
	*/
	inline
	void SetLastSlash(CString_& _path)
	{
		if(!_path.GetLength() || isslash(_path.GetAt(_path.GetLength()-1))) return;
		_path += _T('\\');
	}

	/**\brief verify existing of a some path
	\param[in] _path -- path to verify
	\return TRUE if specified path exist
	*/
	inline
	BOOL IsPathExist(const CString_& _path)
	{
		CString_ path = _path;
		bool bNetPath = startsfrom(path,0,_T("\\\\"));
		if(bNetPath) 
		{
			SetLastSlash(path);
			path += _T("*");
		}
		else
		{
			RemoveLastSlash(path);
		}
		WIN32_FIND_DATA fd;
		ZeroObj(fd);
		HANDLE hfind = FindFirstFile(path,&fd);
		if(EQL(INVALID_HANDLE_VALUE,hfind)) 
		{
			if(!bNetPath) return FALSE;

			// try to find file (path is a path to some file)
			path = _path;
			RemoveLastSlash(path);
			ZeroObj(fd);
			hfind = ::FindFirstFile(path,&fd);
			if(EQL(INVALID_HANDLE_VALUE,hfind)) return FALSE;
			bNetPath = false;
		}
		BOOL bret = TRUE;
		while(bNetPath)
		{
			if(!lstrcmp(fd.cFileName,_T("."))
				|| !lstrcmp(fd.cFileName,_T(".."))
				)
			{
				bret = NEQL(0, fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				break;
			}
			if(!FindNextFile(hfind,&fd))
			{
				//if(::GetLastError()==ERROR_NO_MORE_FILES){}
				bret = FALSE;
				break;
			}
		}
		FindClose(hfind);
		return bret;
	}

	/**\brief verify existing of some file
	\param[in] _path -- path to file to verify it existing
	\return TRUE if file exist
	*/
	inline
	BOOL IsFileExist(const CString_& _path)
	{
		CString_ path = _path;
		WIN32_FIND_DATA fd;
		ZeroObj(fd);
		HANDLE hfind = FindFirstFile(path,&fd);
		if(EQL((INVALID_HANDLE_VALUE),hfind)) return FALSE;
		BOOL bret = EQL(0,fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		FindClose(hfind);
		return bret;
	}

	/**\brief verify is specified path is a directory
	\param[in] _path -- path to verify
	\return TRUE if specified path is a directory
	*/
	inline
	BOOL IsDirectory(const CString_& _path)
	{
		CString_ path = _path;
		bool bNetPath = startsfrom(path,0,_T("\\\\"));
		if(bNetPath) 
		{
			SetLastSlash(path);
			path += _T("*");
		}
		else
		{
			RemoveLastSlash(path);
		}
		WIN32_FIND_DATA fd;
		ZeroObj(fd);
		HANDLE hfind = ::FindFirstFile(path,&fd);
		if(EQL(INVALID_HANDLE_VALUE,hfind)) return FALSE;
		BOOL bret = TRUE;
		while(bNetPath)
		{
			if(!lstrcmp(fd.cFileName,_T("."))
				|| !lstrcmp(fd.cFileName,_T(".."))
				)
			{
				bret = NEQL(0, fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				break;
			}
			if(!FindNextFile(hfind,&fd))
			{
				//if(::GetLastError()==ERROR_NO_MORE_FILES){}
				bret = FALSE;
				break;
			}
		}
		FindClose(hfind);
		return bret;
	}

	/**\brief splite same path name to path, filename and file extention
	\param[in] _fullfname -- full path name to splite
	\param[out] _path -- path from full path name (_fullfname)
	\param[out] _fname -- file name from full path name (_fullfname)
	\param[out] _ext -- file extension from full path name (_fullfname)
	*/
	inline
	void SplitePath(const CString_& _fullfname,CString_& _path,CString_& _fname,CString_& _ext)
	{
		_ext.Empty();
		int i1 = _fullfname.ReverseFind(_T('.'));
		int i2 = _fullfname.ReverseFind(_T('\\'));
		int i3 = _fullfname.ReverseFind(_T('/'));
		i2 = max(i2,i3);
		_path = _fullfname.Left(i2+1);
		_fname = _fullfname.Mid(i2+1);
		if(i1<=i2) return;
		if(!_fname.Compare(_T(".."))
			|| !_fname.Compare(_T("."))
			|| !_fname.Compare(_T("..."))
			) return;
		i1 = _fname.ReverseFind(_T('.'));
		if(i1>=0) {_ext= _fname.Mid(i1);_fname=_fname.Left(i1);}
	}

	/**\brief returns the path part from a full file name
	\param[in] _fullfname -- full file name to get from
	\param[out] _path -- path from full file name
	*/
	inline
	void GetPath(const CString_& _fullfname,CString_& _path)
	{
		CString_ fname,ext;
		SplitePath(_fullfname,_path,fname,ext);
	}

	/**\brief function to return file name (and extension) from full file path
	\param[in] _sFullPath -- full path to get file name from
	\param[out] _sName -- result file name 
	*/
	inline 
	void GetName(const CString_& _sFullPath,CString_& _sName)
	{
		CString_ path,fname,ext;
		SplitePath(_sFullPath,path,fname,ext);
		_sName = fname + ext;
	}

	/**\brief create directory.
	It works correctly if more then on directory don`t exist. This function 
	creates directory recursive in that case.
	\param dir -- directory to create
	*/
	inline
	BOOL CreateDirectory(const CString_& dir)
	{
		BOOL noerr = TRUE;
		int pos,lpos,pos1;
		lpos = dir.Find(_T(':'));
		if(lpos>=0 && isslash(dir.GetAt(lpos+1))) lpos++;
		for(;lpos<dir.GetLength();lpos=pos)
		{
			pos = dir.Mid(lpos+1).Find(_T('\\')); if(pos>=0) pos+=lpos+1;
			pos1 = dir.Mid(lpos+1).Find(_T('/')); if(pos1>=0) pos1+=lpos+1;
			if(pos1>=0 && pos1<pos) pos = pos1;
			CString_ dir1;
			if(pos==-1) pos = dir.GetLength();
			dir1 = dir.Left(pos);
			CString_ dirname = dir.Mid(lpos+1,pos-lpos-1);
			if(EQL(dirname.GetLength(),0)
				|| !dirname.Compare(_T("."))
				|| !dirname.Compare(_T(".."))
				|| !dirname.Compare(_T("..."))
				|| dir1.IsEmpty()
				)continue;


			bool bcreatedir = !IsDirectory(dir1);
			if(bcreatedir) noerr &= ::CreateDirectory(dir1,NULL);
		}
		return noerr;
	}

	/**\brief callback function used in function of selecting some directory
	\param _hwnd -- window handle
	\param _umsg -- message 
	\param _lparam -- LPARAM of message
	\param _lpdata -- data for message
	\return always 0
	\sa FileUtils::DialogChooseDirectory
	*/
	inline
	int CALLBACK BrowseForFolderCallbackProc(HWND _hwnd,UINT _umsg,LPARAM _lparam,LPARAM _lpdata)
	{
		switch(_umsg)
		{
		case BFFM_INITIALIZED:
			{	
				::SendMessage(_hwnd,BFFM_SETSELECTION,TRUE,_lpdata);
				//::SendMessage(_hwnd,BFFM_ENABLEOK,0,TRUE);
				break;
			}
		case BFFM_SELCHANGED:
			{
				CString_ path;
				if(SHGetPathFromIDList((LPCITEMIDLIST)_lparam,path.GetBuffer(MAX_PATH+1)))
				{
					path.ReleaseBuffer();
					if(!IsDirectory(path)) path = TR(_T("Path is not directory"));
				}
				else path.ReleaseBuffer();
				::SendMessage(_hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPCTSTR)path);
				::SendMessage(_hwnd,BFFM_ENABLEOK,0,TRUE);
				break;
			}
#if defined(BFFM_VALIDATEFAILED)
		case BFFM_VALIDATEFAILED:
			{
				::SendMessage(_hwnd,BFFM_ENABLEOK,0,TRUE);
				break;
			}
#endif
		}
		return 0;
	}

	/**\brief This function choose a directory.
	\param[in,out] _dir -- directory to start choose from and choosed directory.
	\param[in] _title -- title for directory choose dialog
	\param[in] _hwndParent -- handle for parent window
	\return TRUE if user choose directory
	*/
/*	inline
	BOOL DialogChooseDirectory(CString_& _dir,LPCTSTR _title,HWND _hwndParent)
	{
		//static TCHAR path[MAX_PATH];
		CString_ dir = _dir;
		BROWSEINFO bi;
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = _hwndParent;
		bi.pidlRoot = NULL;
		//::GetCurrentDirectory(MAX_PATH,path);
		bi.pszDisplayName = dir.GetBuffer(1+max(MAX_PATH,dir.GetLength())); //(LPTSTR)(LPCTSTR)_dir;
		bi.lParam = (LPARAM)bi.pszDisplayName;
		bi.lpfn = BrowseForFolderCallbackProc;
		bi.lpszTitle = _title;
		bi.ulFlags = 
#if defined BIF_NEWDIALOGSTYLE
			BIF_NEWDIALOGSTYLE|
#endif
#if defined(BIF_EDITBOX)
			BIF_EDITBOX|
#endif
#if defined(BIF_VALIDATE)
			BIF_VALIDATE|
#endif
			BIF_STATUSTEXT|
			BIF_BROWSEFORCOMPUTER ;
		LPITEMIDLIST lpItemidls = SHBrowseForFolder(&bi);
		dir.ReleaseBuffer();
		if(lpItemidls==NULL) return FALSE;

		CString_ dirpath;
		IMallocPtr pmalloc;
		SHGetMalloc(&pmalloc);
		IShellFolder* pshellfolder;
		SHGetDesktopFolder(&pshellfolder);
		VERIFY_EXIT1(pshellfolder!=NULL,FALSE);
		STRRET pathname;
		pshellfolder->GetDisplayNameOf(lpItemidls,SHGDN_NORMAL|SHGDN_FORPARSING,&pathname);
		switch(pathname.uType)
		{
		case STRRET_CSTR: dirpath = pathname.cStr; break;
		case STRRET_OFFSET: dirpath = (LPCTSTR)((LPBYTE)lpItemidls + pathname.uOffset); break;
		case STRRET_WSTR: 
			dirpath = (LPCTSTR)_bstr_t(pathname.pOleStr); 
			pmalloc->Free(pathname.pOleStr);
			break;
		}
		pshellfolder->Release();
		_dir = dirpath;
		pmalloc->Free(lpItemidls);
		return TRUE;
	}
*/
	/**\brief Verify access rights for file.
	\param _fname -- file name to verify access 
	\param _dwShareMode -- share mode to verify
	\return TRUE if to file access allowed
	*/
	inline
	BOOL CanAccess(const CString_& _fname,DWORD _dwShareMode)
	{
		HANDLE hfile = ::CreateFile(_fname,_dwShareMode,0,NULL
			,OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL,NULL
			);
		if(INVALID_HANDLE_VALUE==hfile) 
			if(GetLastError()==ERROR_SHARING_VIOLATION) return FALSE;
			else return FALSE;
		CloseHandle(hfile);
		return TRUE;
	}

	/**\brief Create random hex string of specified length
	\param[in] _hlen -- required length of a string
	\param[out] _str -- result string
	*/
	inline
	void randHexString(UINT _hlen,CString_& _str)
	{
		UINT i;
		_str.Empty();
		for(i=0;i<_hlen;i++)
		{
			TCHAR h = rand()&0xf;
			if(h>=0xA) h += _T('A')-10;
			else h += _T('0');
			_str += h;
		}
	}

	/**\brief Return next hex string, by adding the 1 to hex number specified by hex string.
	\param[in] _hlen -- length of hex string
	\param[in,out] _str -- hex string input and modified
	*/
	inline
	void nextHexString(UINT _hlen,CString_& _str)
	{
		UINT i;
		for(i=0;i<_hlen;i++)
		{
			TCHAR h = _str.GetAt(i);
			if(_T('9')==h) h=_T('A');
			else if(_T('F')==h) h=_T('0');
			else h++;
			_str.SetAt(i,h);
			if(h!=_T('0')) break;
		}
	}

	/**\brief search files and fill string list with them.
	\param[in] _path -- path to search in
	\param[in] _mask -- mask to search with
	\param[out] _files -- list of founded files
	\param[in] _attrmask -- file attributes mask used in search
	\param[in] _attrval -- file attributes value used in search (used with param _attrmask)
	\return TRUE if there was no error in search, FALSE if there was any arrors. In last case
	parameter _files may contain some files that was founded before error occurred.
	*/
	inline
	BOOL CollectFiles(IN const CString_& _path
		,IN const CString_& _mask
		,OUT std::list<CString_>& _files
		,IN DWORD _attrmask = FILE_ATTRIBUTE_DIRECTORY
		,IN DWORD _attrval = 0
		)
	{
		WIN32_FIND_DATA fd;
		CString_ path(_path);
		SetLastSlash(path);
		CString_ find = path;
		find += _mask;
		_files.clear();
		HANDLE hff = ::FindFirstFile(find,&fd);
		if(INVALID_HANDLE_VALUE==hff) return FALSE;
		for(;hff;)
		{
			if(!(
					(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0
					&& (
						!lstrcmp(fd.cFileName,_T("..")) 
						|| !lstrcmp(fd.cFileName,_T("."))
						)
					)
				) 
			{
				if((fd.dwFileAttributes & _attrmask)==_attrval)	
				{
					CString_ fname = path;
					fname += fd.cFileName;
					_files.push_back(fname);
				}
			}
			if(!::FindNextFile(hff,&fd))
			{
				if(::GetLastError()==ERROR_NO_MORE_FILES) break;
				DWORD dwErr = ::GetLastError();
				_files.clear();
				::FindClose(hff);
				::SetLastError(dwErr);
				return FALSE;
			}
		}
		::FindClose(hff);
		return TRUE;
	}

	/**\brief Returns temp file name.
	This function create random file name from path, file prefix and required random
	size length.
	\param[in] _path -- path where should create new temp file (create next unique file name)
	\param[in] _prefix -- prefix to start file name from for created file name
	\param[in] _hlen -- required length of random part for new file name
	\param[out] _filename -- next unique file name
	\return TRUE if file name created, FALSE if impossible to create next unique file name.
	*/
	inline
	BOOL GetTempFileName(const CString_& _path,const CString_& _prefix,UINT _hlen,CString_& _filename)
	{
		CString_ path,name,ext,fpath;
		fpath = _path;
		SetLastSlash(fpath);
		fpath += _prefix;
		SplitePath(fpath,path,name,ext);
		if(!IsPathExist(path)) return FALSE;
		SetLastSlash(path);
		fpath = path;
		StringLst files;
		CollectFiles(path,_T("*.*"),files);
		CString_ hexstr,hstr;
		randHexString(_hlen,hexstr);
		hstr = hexstr;
		for(;;)
		{
			CString_ fname = name + hexstr + ext;
			CString_ fullname = path;
			SetLastSlash(fullname);
			fullname += fname;
			std::list<CString_>::iterator it = std::find_if(files.begin(),files.end(),CmpStrNoCase(fullname));
			if(it==files.end()) 
			{
				_filename = path;
				SetLastSlash(_filename);
				_filename += fname;
				return TRUE;
			}
			nextHexString(_hlen,hexstr);
			if(!hexstr.Compare(hstr)) break;
		}
		return FALSE;
	}

	/**\brief Function return path for the module
	\param[in] _hmodule -- handle of the module
	\param[out] _path -- returned module path
	*/
	inline 
	void GetModuleFullPath(HMODULE _hmodule,CString_& _path)
	{
		CString_ path;
		long length = MAX_PATH;
		while(!::GetModuleFileName(_hmodule,path.GetBuffer(length+1),length))
		{
			if(GetLastError()!=ERROR_INSUFFICIENT_BUFFER || length>1024*1024)
			{
				_path.Empty();
				return;
			}
			length *= 2;
			path.ReleaseBuffer();
		}
		
		path.ReleaseBuffer();
		_path = path;
	}

	inline 
	BOOL GetTempPath(CString_& _sPath)
	{
		CString_ path;
		long length = MAX_PATH;
		long reqlen = ::GetTempPath(length,path.GetBuffer(length));
		if(reqlen>length)
		{
			length = reqlen+1;
			path.ReleaseBuffer(length);
			reqlen = ::GetTempPath(length,path.GetBuffer(length));
		}
		if(reqlen==0) return FALSE;
		path.ReleaseBuffer(-1);
		_sPath = path;
		return TRUE;
	}

	/**\brief Return module name.
	Function returns module name from module path \ref FileUtils::GetModuleFullPath .
	\param[in] _hmodule -- handle to module to get name for.
	\param[out] _name -- returned module name.
	\sa FileUtils::GetModuleFullPath 
	*/
	inline 
	void GetModuleName(HMODULE _hmodule,CString_& _name)
	{
		CString_ fullpath,path,fname,ext;
		GetModuleFullPath(_hmodule,fullpath);
		SplitePath(fullpath,path,fname,ext);
		_name = fname + ext;
	}

	/**\brief Return module path (without module name).
	Function return module path from full module path \ref FileUtils::GetModuleFullPath.
	\param[int] _hmodule -- module handle to get path for.
	\param[out] _path -- returned path of the module.
	\sa FileUtils::GetModuleFullPath 
	*/
	inline
	void GetModulePath(HMODULE _hmodule,CString_& _path)
	{
		CString_ fullpath,fname,ext;
		GetModuleFullPath(_hmodule,fullpath);
		SplitePath(fullpath,_path,fname,ext);
	}
	
	/**\brief Count files in some directory using some files mask and file attrbutes.
	\param[in] _path -- path to files to count
	\param[in] _mask -- file mask of files to count
	\param[in] _attrmask -- attributes mask of files to count
	\param[in] _attrval -- attributes value for files to count. used with argument _attrmask
	\return files count
	*/
	inline
	long CountFiles(const CString_& _path
		,const CString_& _mask
		,DWORD _attrmask = FILE_ATTRIBUTE_DIRECTORY
		,DWORD _attrval = 0
		)
	{
		long cnt = 0;
		WIN32_FIND_DATA fd;
		CString_ path(_path);
		SetLastSlash(path);
		path += _mask;
		HANDLE hff = ::FindFirstFile(path,&fd);
		if(INVALID_HANDLE_VALUE==hff) return -1;
		for(;hff;)
		{
			if(!(
					(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0
					&& (
						!lstrcmp(fd.cFileName,_T("..")) 
						|| !lstrcmp(fd.cFileName,_T("."))
						)
					)
				) 
			{
				if((fd.dwFileAttributes & _attrmask)==_attrval)	cnt++;
			}
			if(!::FindNextFile(hff,&fd))
			{
				if(::GetLastError()==ERROR_NO_MORE_FILES) break;
				DWORD dwErr = ::GetLastError();
				::FindClose(hff);
				::SetLastError(dwErr);
				return -1;
			}
		}
		::FindClose(hff);
		return cnt;
	}

	
	/**\brief covert relative part (starting with '..' or '.') to path
	          starting from module path.
	\param[in] _hmodule -- module to use as path base
	\param[in,out] _path -- path to modify
	*/
	inline
	void ModifyRelativePath(HMODULE _hmodule,CString_& _path)
	{
		CString_ modulepath;
		CString_ path0,fname0,ext0;
		SplitePath(_path,path0,fname0,ext0);
		int si1 = path0.Find(_T('/'));
		int si2 = path0.Find(_T('\\'));
		if(si2>=0 && (si2<si1 || si1<0)) si1 = si2;
		//if(si2>=0 && si2<si1) si1 = si2;
		CString_ dir0;
		if(si1>=0) dir0 = path0.Mid(0,si1);
		else dir0 = path0;
		if(dir0.Compare(_T(".")) && dir0.Compare(_T(".."))) return; //do nothing. not a relative path
		GetModulePath(_hmodule,modulepath);
		if(path0.GetLength()>0 && (path0.GetAt(0)==_T('\\') || path0.GetAt(0)==_T('/')))
		{
			_path = modulepath + _path;
		}
		else
		{
			SetLastSlash(modulepath);
			_path = modulepath + _path;
		}
		//convert2absolute(_path);
	}
	
	/**\brief load file data to string
	\param[in] _sFileName -- file name to load data from
	\param[out] _text -- loaded file content
	\return false if there is any error, true if success
	*/
	inline
	bool load_file(const CString_& _sFileName,CString_& _text)
	{
		HANDLE hfile = ::CreateFile(
			_sFileName
			,GENERIC_READ
			,FILE_SHARE_READ
			,NULL
			,OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(INVALID_HANDLE_VALUE==hfile) return false;
		DWORD dwSizeHi = 0;
		DWORD dwSize = ::GetFileSize(hfile,&dwSizeHi);
		if(dwSizeHi!=0)
		{
			CloseHandle(hfile);
			return false;
		}
		DWORD dwReaded = 0;
		std::vector<BYTE> buf;
		buf.resize(dwSize);
		if(!ReadFile(hfile,&buf[0],dwSize,&dwReaded,NULL)
			|| dwReaded!=dwSize)
		{
			CloseHandle(hfile);
			return false;
		}
		CloseHandle(hfile);
		string_converter<char,TCHAR> strT((char*)&buf[0],dwSize);
		_text = (LPCTSTR)strT;
		return true;
	}

	/**\brief simple save data to file.
	\param[in] _sFileName -- file name.
	\param[in] _text -- text to save to file.
	\return true if success else false.
	*/
	inline 
	bool save_file(const CString_& _sFileName,const CString_& _text)
	{
		HANDLE hfile = ::CreateFile(
			_sFileName
			,GENERIC_WRITE
			,FILE_SHARE_READ
			,NULL
			,CREATE_ALWAYS
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(INVALID_HANDLE_VALUE==hfile) return false;
		DWORD dwWritten = 0;
		if(!::WriteFile(hfile,(LPVOID)(LPCTSTR)_text,_text.GetLength(),&dwWritten,NULL)
			|| dwWritten!=_text.GetLength()
			)
		{
			::CloseHandle(hfile);
			return false;
		}
		::CloseHandle(hfile);
		return true;
	}

	/**\brief class to specify file in current user temp directory
	*/
	struct CTempFile
	{
		/**\brief constructor
		\param _szFileName -- file name
		*/
		CTempFile(LPCTSTR _szFileName = NULL)
			:m_hFile(INVALID_HANDLE_VALUE)
		{
			if(NOT_NULL(_szFileName)) open(_szFileName);
		}

		/**\brief destructor
		*/
		~CTempFile()
		{
			if(NEQL(m_hFile,INVALID_HANDLE_VALUE) && NEQL(m_hFile,NULL))
			{
				::CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
			}
		}

		/**\bief function to open temp file.
		It open file name at current user temp path with specified name
		\param _szFileName -- file name to open
		*/
		void open(LPCTSTR _szFileName)
		{
			VERIFY_EXIT(NOT_NULL(_szFileName));
			VERIFY_EXIT(EQL(m_hFile,INVALID_HANDLE_VALUE) || NEQL(m_hFile,NULL));
			CString_ sTempPath;
			if(!GetTempPath(sTempPath)) sTempPath = _T("c:\\temp");
			SetLastSlash(sTempPath);
			CreateDirectory(sTempPath);
			sTempPath += _szFileName;

			m_hFile = ::CreateFile(
				sTempPath
				,GENERIC_WRITE
				,FILE_SHARE_READ
				,NULL
				,OPEN_ALWAYS
				,FILE_ATTRIBUTE_NORMAL
				,NULL
				);
			if(INVALID_HANDLE_VALUE==m_hFile) return;
			SetFilePointer(m_hFile,0,NULL,FILE_END);
		}

		/**\brief function to write string into the temp file
		\param _sText -- string to write 
		\param _bExpandNewLine -- flag to replace '\n' into '\r\n'
		*/
		void write(const CString_& _sText,bool _bExpandNewLine = true)
		{
			TRACE_(_T("logging : %s\n"),(LPCTSTR)_sText);
			if(IS_NULL(m_hFile) || EQL(m_hFile,INVALID_HANDLE_VALUE)) return;
			CString_ sText = _sText;
			if(_bExpandNewLine)	replace(sText,_T("\n"),_T("\r\n"));
			DWORD dwWritten = 0;
			WriteFile(m_hFile,(LPVOID)(LPCTSTR)sText,sText.GetLength(),&dwWritten,NULL);
		}

		template<typename _Type>
			CTempFile& operator << (const _Type& _dta)
		{
			CString_ str;
			common::convert(_dta,str);
			write(str);
			return *this;
		}
	protected:
		HANDLE m_hFile;			///< file handle
	};//struct CTempFile

};//namespace FileUtils

//@}

#endif
