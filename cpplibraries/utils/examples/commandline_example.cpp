#include <utils/commandline.h>

// application help string
static const LPCTSTR g_szHelp =
	_T("Convertor from hex to binary format of file.\n")
	_T("Usage:\n")
	_T("convert.exe  -in input_file [b2h [-width line_length]] [-out out_filename] [-verbose]\n")
	_T("Where:\n")
	;

// functions to process data
void bin2hex(const CString& _srcfn,const CString& _destfn,long _width,bool _bverbose);
void hex2bin(const CString& _srcfn,const CString& _destfn,bool _bverbose);

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
	CCmdLineArg help(_T("help"),_T("this help message."),0,true);
	CCmdLineArg infile(_T("in"),_T("input file name."),1);
	CCmdLineArg b2h(_T("b2h"),_T("invers conversation (from bin to hex)."));
	CCmdLineArg outfile(_T("out"),_T("output file name."),1);
	CCmdLineArg verbose(_T("v"),_T("generate verbose output."));
	CCmdLineArg width(_T("w"),_T("width of string line in hex output file."),1);

	// set symonyms of command line params
	help
		.add_syn(_T("?"));	// help can be "-help" or "-?" or "/help" or "/?"
	infile
		.add_syn(_T("in_file"))		// input file can be "-in" or "-in_file"
		.add_syn(_T("input_file"));	// or "-input_file"
	outfile.add_syn(_T("out_file"))
		.add_syn(_T("output_file"));
	verbose
		.add_syn(_T("verbose"));
	width
		.add_syn(_T("width"));

	// create command line object
	CCommandLine cmdline(argc,argv);
	
	cmdline.set_help(g_szHelp);		// set application help string

	// add params to search in command line for
	cmdline
		.set_param(help)
		.set_param(infile)
		.set_param(outfile)
		.set_param(b2h)
		.set_param(verbose)
		.set_param(width);
		;

	// if
	if(argc <= 1				// no command line argumenst
		|| !cmdline.parse()		// or there was error in parsing
		|| help					// or there was help param in command line 
		|| !infile				// or there was no input file in command line
		)
	{
		cmdline.print_help(cout);	// then output command line help
		return 0;					// and return
	}

	// form input file name
	CString sInFile = infile[0];	// from infile param argument
	CString sOutFile;

	// form output file name
	if(outfile) sOutFile = outfile[0];	// from out file name param
	else sOutFile = sInFile + (b2h?_T(".hex"):_T(".bin"));	// or from input file

	// call function according to params
	if(b2h) // if in command line is specified to convert from binary to hex
	{
		long w = 80;
		if(width) w = _tcstol(width[0],NULL,10);
		bin2hex(sInFile,sOutFile,w,verbose);
	}
	else hex2bin(sInFile,sOutFile,verbose);

	return 0;
}
