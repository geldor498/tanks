#if !defined(__COMMANDLINE_H__5E058224_5D06_4F0B_B649_FA114E65AE66)
#define __COMMANDLINE_H__5E058224_5D06_4F0B_B649_FA114E65AE66


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"


#include "utils.h"

/**\defgroup CommandLine Command line support
Decalares command line support.
See example of ussage of this classes: \ref ref_Example_ForCCommandLine "processing of command line arguments example"
*/

/**\page Page_QuickStart_Commandline Quick start: "Comand line support in you application"

Command line usage steps:
	-# Create \ref CCmdLineArg objects to specify params to search in command line for
	-# Tune \ref CCmdLineArg adding synonyms names (\ref CCmdLineArg::add_syn())
	   or setting parameter help (\ref CCmdLineArg::set_help())
	-# Create \ref CCommandLine object and transfer argumnets of main function to it 
	   (argumnets count and arguments array).
	-# Tune \ref CCommandLine object.
	-# Set params for object of type \ref CCommandLine to all params you need 
	   (variables of type \ref CCmdLineArg). 
	-# Call function \ref CCommandLine::parse()
	-# If function \ref CCommandLine::parse() return false then output 
	   application command line help \ref CCommandLine::print_help() and 
	   exit application.
	-# Test objects of type \ref CCmdLineArg if they were founded in command 
	   line while parsing it. If \ref CCmdLineArg::operator bool () const
	   return true then this param was founded in comand line and if 
	   this param has any arguments then and param`s argumenst are available 
	   now (with \ref CCmdLineArg::operator [](long _i) const ).
	-# Retry arguments with \ref CCmdLineArg::operator [](long _i) const .
	.

\code
// application help string
static const LPCTSTR g_szHelp =
	_T("Convertor from hex to binary format of file.\n")
	_T("Usage:\n")
	_T("convert.exe  -in input_file [b2h [-width line_length]] [-out out_filename] [-verbose]\n")
	_T("Where:\n")
	;

// functions to process data
void bin2hex(const CString_& _srcfn,const CString_& _destfn,long _width,bool _bverbose);
void hex2bin(const CString_& _srcfn,const CString_& _destfn,bool _bverbose);

// main function of console application
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}

	// create command line params objects to serach in command line
[1]	CCmdLineArg help(_T("help"),_T("this help message."),0,true);
[1]	CCmdLineArg infile(_T("in"),_T("input file name."),1);
[1]	CCmdLineArg b2h(_T("b2h"),_T("invers conversation (from bin to hex)."));
[1]	CCmdLineArg outfile(_T("out"),_T("output file name."),1);
[1]	CCmdLineArg verbose(_T("v"),_T("generate verbose output."));
[1]	CCmdLineArg width(_T("w"),_T("width of string line in hex output file."),1);

	// set symonyms of command line params
[2]	help
		.add_syn(_T("?"));	// help can be "-help" or "-?" or "/help" or "/?"
[2]	infile
		.add_syn(_T("in_file"))		// input file can be "-in" or "-in_file"
		.add_syn(_T("input_file"));	// or "-input_file"
[2]	outfile.add_syn(_T("out_file"))
		.add_syn(_T("output_file"));
[2]	verbose
		.add_syn(_T("verbose"));
[2]	width
		.add_syn(_T("width"));

	// create command line object
[3]	CCommandLine cmdline(argc,argv);
	
[4]	cmdline.set_help(g_szHelp);		// set application help string

	// add params to search in command line for
[5]	cmdline
		.set_param(help)
		.set_param(infile)
		.set_param(outfile)
		.set_param(b2h)
		.set_param(verbose)
		.set_param(width);
		;

	// if
	if(argc <= 1				// no command line argumenst
[6]		|| !cmdline.parse()		// or there was error in parsing
[8]		|| help					// or there was help param in command line 
[8]		|| !infile				// or there was no input file in command line
		)
	{
[7]		cmdline.print_help(cout);	// then output command line help
		return 0;					// and return
	}

	// form input file name
[9]	CString_ sInFile = infile[0];	// from infile param argument
	CString_ sOutFile;

	// form output file name
[8,9]	if(outfile) sOutFile = outfile[0];	// from out file name param
	else sOutFile = sInFile + (b2h?_T(".hex"):_T(".bin"));	// or from input file

	// call function according to params
[8]	if(b2h) // if in command line is specified to convert from binary to hex
	{
		long w = 80;
[9]		if(width) w = _tcstol(width[0],NULL,10);
		bin2hex(sInFile,sOutFile,w,verbose);
	}
	else hex2bin(sInFile,sOutFile,verbose);

	return 0;
}
\endcode

See also examples.
*/

/**\example commandline_example.cpp
Example of commang line utilities use.
*/
//@{

/**\brief One command line argument.
Class to support one argument of command line
\sa CCommandLine
*/
struct CCmdLineArg
{
	/**\brief Constructor
	Create an command line argument 
	\param _sName name of the command line param.
		   This is the name which used in command line params (and argumnets of params) 
		   parse algorithm.
	\param _sHelp help string for this argument
	\param _cnt number of arguments for this command line param
	\param _bterm flag to terminate command line parse algorithm 
	       if this param was founded (true to terminate, false don`t stop parsing)
	*/
	CCmdLineArg(const CString_& _sName,const CString_& _sHelp,long _cnt = 0,bool _bterm = false,bool _bMulti=false)
		:m_bfinded(false)
		,m_bterm(_bterm)
		,m_sHelp(_sHelp)
		,m_cnt(_cnt)
		,m_bMulti(_bMulti)
	{
		reset();
		VERIFY_EXIT(!_sName.IsEmpty());
		m_names.push_back(_sName);
	}

	/**\brief add command line param synonym 
	Add synonym to search for in command line
	\param _synonym param name synonym
	\return reference to \ref CCmdLineArg object
	*/
	CCmdLineArg& add_syn(const CString_& _synonym)
	{
		VERIFY_EXIT1(!_synonym.IsEmpty(),*this);
		m_names.push_back(_synonym);
		return *this;
	}

	/**\brief class type conversation operator (to bool type)
	\return true if command line param was founded in command line after it parse
	*/
	operator bool () const {return m_bfinded;}
	/**\brief class operator ! 
	\return true if command line param not founded in command line after it parse
	*/
	bool operator ! () const {return !operator bool();}
	/**\brief get arguments list for this param founded in command line
	\return arguments list founded in command line after it parse
	*/

	bool search_more() const {return !(bool)*this || m_bMulti;}

	const StringLst& get_names() const {return m_names;}
	/**\brief get arguments count founded in command line
	\return arguments count founded in command line after it parse
	*/

	long get_argcount() const {return m_cnt;}
	long get_count() const {return m_args.size();}

	/**\brief sets founded arguments 
	\param _paramargs founded arguments strings list
	*/
	void set(const StringLst _paramargs)
	{
		m_args = _paramargs;
		m_bfinded = true;
	}
	/**\brief reset founded arguments and founded flag
	*/
	void reset()
	{
		m_args.clear();
		m_bfinded = false;
	}

	/**\brief class operator[] getter for founded arguments
	\param _i index of argument
	\return value of founded argument with index _i
	*/
	CString_ operator[] (long _i) const
	{
		if(_i<0 || _i>=(long)m_args.size() && _i<LONG_MAX) return CString_();
		StringLst::const_iterator it = m_args.begin();
		std::advance(it,_i);
		return *it;
	}

	/**\brief is this command line parameter is terminal 
	\return true if this command line param is terminal one
	*/
	bool is_term() const {return m_bterm;}

	/**\brief sets help string for this command line param
	\param _sHelp command line param help string
	*/
	void set_help(const CString_& _sHelp) {m_sHelp = _sHelp;}

	/**\brief gets help string for this param
	\return helpstring for this param
	*/
	const CString_& get_help() const {return m_sHelp;}
protected:
	bool m_bfinded;		///< if param founded in command line
	bool m_bterm;		///< if this param is terminal one 
						/**<(this mean that parsing is stopped if this param was founded in command line)*/
	StringLst m_names;	///< list of parameter names (name declared in constructor + synonyms)
	StringLst m_args;	///< list of arguments after parameter was founded 
	CString_ m_sHelp;	///< help string
	long m_cnt;			///< maximum count of parameter arguments
	bool m_bMulti;
};//struct CCmdLineArg

/**\brief Class of command line.
Class that represents command line and support for command line parsing
\sa CCmdLineArg
*/
struct CCommandLine
{
protected:
	/**\brief define list of \ref CCmdLineArg pointers to hold command line params and its arguments*/
	typedef std::vector<CCmdLineArg*> ParamsLst;
	/**\brief enumeration for \ref CCommandLine inner work*/
	enum CmdEn{
		cmd_CycleItem	///<repeat flag
		,cmd_NextArg	///<search next param flag
	};
public:
	/**\brief constructor
	\param _argc -- _argc parameter of WinMain|main function
	\param _argv -- _argv parameter of WinMain|main function
	*/
	CCommandLine(int _argc, TCHAR* _argv[])
		:m_argc(_argc),m_argv(_argv),m_pos(1)
	{
	}

	/**\brief set command line param to search for in command line
	\param _par -- command line parameter
	\return \ref CCommandLine reference
	*/
	CCommandLine& set_param (CCmdLineArg& _par)
	{
		m_params.push_back(&_par);
		return *this;
	}

	/**\brief parse command line function
	\return true if command line was successfuly parsed
	*/
	bool parse()
	{
		if(m_argc-1<min_params_count()) return false;
		while(*this)
		{
			CmdEn cmd = cmd_CycleItem;
			ParamsLst::iterator 
				it = m_params.begin()
				,ite = m_params.end()
				;
			for(;it!=ite && cmd==cmd_CycleItem;++it)
			{
				CCmdLineArg& par = **it;
				if(par.search_more())
				{
					if(*this >> par)
					{
						if(par.is_term()) return true;
						cmd = cmd_NextArg;
					}
				}
			}
			if(cmd_NextArg==cmd) continue;
			return false;// unknown item
		}
		return true;
	}

	long min_params_count() const
	{
		ParamsLst::const_iterator 
			it = m_params.begin()
			,ite = m_params.end()
			;
		long nArgCnt = 0;
		for(;it!=ite;++it)
		{
			CCmdLineArg& par = **it;
			nArgCnt += par.get_argcount();
		}
		it = m_params.begin();
		for(;it!=ite;++it)
		{
			CCmdLineArg& par = **it;
			if(par.is_term()) 
			{
				nArgCnt = min(nArgCnt,1);
				break;
			}
		}
		return nArgCnt;
	}
	/**\brief print help into stream
	\tparam _Stream tream type
	\param _stream stream object to output help for command line
	*/
	template<typename _Stream>
	void print_help(_Stream& _stream) const 
	{
		_stream << (LPCTSTR)m_help;
		ParamsLst::const_iterator 
			it = m_params.begin()
			,ite = m_params.end()
			;
		for(;it!=ite;++it)
		{
			_stream << _T("\t");
			StringLst::const_iterator 
				nameit = (*it)->get_names().begin()
				,nameite = (*it)->get_names().end()
				;
			for(;nameit!=nameite;nameit++)
			{
				if(nameit!=(*it)->get_names().begin())
					_stream << _T(" | ");
				_stream << (LPCTSTR)*nameit;
			}
			_stream << _T(" -- ");
			_stream << (LPCTSTR)(*it)->get_help() << endl;
		}
	}

	/**\brief set help string for this application command line
	\param _sHelp help string for this application
	*/
	void set_help(const CString_& _sHelp)
	{
		m_help = _sHelp;
	}

protected:
	/**\brief return true if string eqaul to any string in string list
	Function used to find params in command line by the lists of there names
	\param _s1 -- string to compare (command line string)
	\param _lst -- list of string to compare with first param
	\return true if _s1 equal any valu from string list (_lst)
	*/
	bool equal(const CString_& _s1,const StringLst& _lst)
	{
		StringLst::const_iterator 
			it = _lst.begin()
			,ite = _lst.end()
			;
		for(;it!=ite;++it)
		{
			if(!_s1.CompareNoCase(*it)) return true;
		}
		return false;
	}

	/**\brief operator to initialize founded command line param with arguments.
	Operator to read strings from command line and initialize command line 
	parameter.
	\param _par -- command line parameter to initialize
	\return \ref CCommandLine reference 
	*/
	bool operator >> (CCmdLineArg& _par)
	{
		_par.reset();
		if(m_pos>=m_argc) return false;
		CString_ s1 = m_argv[m_pos];
		const CString_ s2(_T("\\/:-"));
		s1 = (s2.Find(s1.GetAt(0))>=0)?s1.Mid(1):s1;
		if(!equal(s1,_par.get_names())) return false;
		m_pos++;
		long cnt = _par.get_argcount();
		StringLst lst;
		if(!cnt)
		{
			_par.set(lst);
			return true;
		}
		if(m_pos+cnt>m_argc) return false;
		long i=0;
		for(i=0;i<cnt && m_pos<m_argc;i++) lst.push_back(m_argv[m_pos++]);
		_par.set(lst);
		return true;
	}

	/**\brief state operator.
	Class operator to get is current state is valid for more operation from 
	command line.
	\return true if current position (\ref CCommandLine::m_pos) less then 
	        argument count (member variable \ref CCommandLine::m_argc).
	*/
	operator bool () const {return m_pos<m_argc;}

	/**\brief list of params to use in command line parsing*/
	ParamsLst m_params;
protected:
	CString_ m_help; ///< help string for this command line for an application
	int m_pos; ///< current position in command line for parsing
	int m_argc; ///< count of arguments
	TCHAR** m_argv; ///< arguments array
};//struct CCommandLine


/**\class CCommandLine
\anchor ref_Example_ForCCommandLine
Example of usage \ref CCommandLine and \ref CCmdLineArg
\code
  

	static const LPCTSTR g_szHelp =
		_T("Convertor from hex to binary format of file.\n")
		_T("Usage:\n")
		_T("convert.exe  -in input_file [b2h [-width line_length]] [-out out_filename] [-verbose]\n")
		_T("Where:\n")
		;

	void bin2hex(const CString_& _srcfn,const CString_& _destfn,long _width,bool _bverbose);
	void hex2bin(const CString_& _srcfn,const CString_& _destfn,bool _bverbose);

	int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			cerr << _T("Fatal Error: MFC initialization failed") << endl;
			return 1;
		}
		CCmdLineArg help(_T("help"),_T("this help message."),0,true);
		CCmdLineArg infile(_T("in"),_T("input file name."),1);
		CCmdLineArg b2h(_T("b2h"),_T("invers conversation (from bin to hex)."));
		CCmdLineArg outfile(_T("out"),_T("output file name."),1);
		CCmdLineArg verbose(_T("v"),_T("generate verbose output."));
		CCmdLineArg width(_T("w"),_T("width of string line in hex output file."),1);

		//set symonyms
		help
			.add_syn(_T("?"));
		infile
			.add_syn(_T("in_file"))
			.add_syn(_T("input_file"));
		outfile.add_syn(_T("out_file"))
			.add_syn(_T("output_file"));
		verbose
			.add_syn(_T("verbose"));
		width
			.add_syn(_T("width"));

		CCommandLine cmdline(argc,argv);
		
		cmdline.set_help(g_szHelp);
		cmdline
			.set_param(help)
			.set_param(infile)
			.set_param(outfile)
			.set_param(b2h)
			.set_param(verbose)
			.set_param(width);
			;

		if(argc <= 1 || !cmdline.parse() || help || !infile)
		{
			cmdline.print_help(cout);
			return 0;
		}

		CString_ sInFile = infile[0];
		CString_ sOutFile;
		if(outfile) sOutFile = outfile[0];
		else sOutFile = sInFile + (b2h?_T(".hex"):_T(".bin"));

		if(b2h) 
		{
			long w = 80;
			if(width) w = _tcstol(width[0],NULL,10);
			bin2hex(sInFile,sOutFile,w,verbose);
		}
		else hex2bin(sInFile,sOutFile,verbose);

		return 0;
	}
\endcode
*/

//@}

#endif //#if !defined(__COMMANDLINE_H__5E058224_5D06_4F0B_B649_FA114E65AE66)