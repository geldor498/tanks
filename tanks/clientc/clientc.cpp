// clientc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "clientc.h"
#include <conio.h>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 1;
	}
	if(argc<4)
	{
		std::cout << "clientc.exe <team name> <tank name> <server name>\n";
		return 1;
	}
	try
	{
		CString sPipeName;
		srand((unsigned)time(NULL));
		sPipeName.Format(_T("TankBattleClient%04x"),rand());
		CTank tank(argv[1],argv[2],sPipeName);
		CFlagColor flag[3];
		flag[0] = RGB(0,0xff,0);
		flag[1] = RGB(0xff,0xff,0xff);
		flag[2] = RGB(0,0xff,0);
		if(!tank.connect(argv[3],g_szTankBattleConnectServer,flag)) return 1;

		std::cout << "press keys to send commands to server\nkeys: 'g' 'h' 'j' 'b' 'n' 'm' 'z' 'c' 's' 'x' 'i' 'w' 'e' 'r'\n";
		int key = _getch();
		for(;;(key = _getch()))
		{
			key = tolower(key);
			if(key=='q') break; // exit
			CRadarInfo ri;

			switch (key)
			{
			case 'g': 
				std::cout << "left track speed up\n";
				tank.lefttrack(0.1);
				tank.move();
				break;
			case 'h':
				std::cout << "both track speed up\n";
				tank.lefttrack(0.1);
				tank.righttrack(0.1);
				tank.move();
				break;
			case 'j':
				std::cout << "right track speed up\n";
				tank.righttrack(0.1);
				tank.move();
				break;
			case 'b':
				std::cout << "left track speed down\n";
				tank.lefttrack(-0.1);
				tank.move();
				break;
			case 'n':
				std::cout << "both track speed down\n";
				tank.lefttrack(-0.1);
				tank.righttrack(-0.1);
				tank.move();
				break;
			case 'm':
				std::cout << "right track speed down\n";
				tank.righttrack(-0.1);
				tank.move();
				break;
			case 'z':
				std::cout << "rotate turret clockwise\n";
				tank.rotateturret(1);
				tank.shot();
				break;
			case 'c':
				std::cout << "rotate turret anticlockwise\n";
				tank.rotateturret(-1);
				tank.shot();
				break;
			case 's':
				std::cout << "gun up\n";
				tank.upgun(1);
				tank.shot();
				break;
			case 'x':
				std::cout << "gun down\n";
				tank.upgun(-1);
				tank.shot();
				break;
			case 'i':
				std::cout << "get information\n";
				tank.get_radarinfo(ri);
				print_ri(ri);
				break;
			case 'w':
				{
				std::cout << "take artefact\n";
				CArtefactInfo artefact;
				tank.take_artefact(artefact);
				std::cout<<"artefact: "<<artefact.m_ID<<"  "<<artefact.m_fMass<<"  "<<artefact.m_type<<"\n";
				}break;
			case 'e':
				std::cout << "use artefact\n artefact id:";
				long uaid;
				std::cin>>uaid;
				tank.use_artefact(uaid);
				break;

			case 'r':
				std::cout << "put artefact\n artefact id:";
				long puid;
				std::cin>>puid;
				tank.put_artefact(puid);
				break;
			}
		}
	}
	catch(utils::IUtilitiesException EXC_ACCESS_TYPE _pexc)
	{
		ExceptionPtr<utils::IUtilitiesException> pexc(_pexc);
		CString msg = (LPCTSTR)FormatExceptionFull(pexc);
		std::string msgA = AnsiToOem(msg);
		std::cerr << "Exception :\n" << msgA.c_str() << "\n";
	}
	catch(std::exception& _exc)
	{
		std::string str = _exc.what();
		AnsiToOem(str);
		std::cerr << "Exception: " << str.c_str() << "\n";
	}
	return 0;
}
