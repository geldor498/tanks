#pragma once

#include "messages.h"
#include "gameworld.h"

#include <pipe/server.h>
#include <pipe/client.h>
#include <utils/typelist.h>

typedef TYPELIST_1(CHelloWorld) ConnectionMessages;

struct CConnectionServer 
	:public CServerPipeCommImpl<CConnectionServer,ConnectionMessages>
{
	CConnectionServer(CGameWorld& _gameworld)
		:CServerPipeCommImpl(g_szTankBattleConnectServer)
		,m_gameworld(_gameworld)
		,m_id(0)
	{
		start();
	}

	bool process(CHelloWorld* _pHelloWorldMsg)
	{
		CHandShack answer;
		answer.m_sTankPipeID.Format(_T("%08x"),m_id++); 
		CClientPipeComm client;
		CString sComputerName;
		GetComputerName(sComputerName);
		if(client.open(
				sComputerName==_pHelloWorldMsg->m_sTankClientComputer
					?_T(".")
					:_pHelloWorldMsg->m_sTankClientComputer
				,_pHelloWorldMsg->m_sPipeName
				)
			)
		{
			m_gameworld.add_tank(
				_pHelloWorldMsg->m_sTankClientComputer,_pHelloWorldMsg->m_sPipeName,answer.m_sTankPipeID
				,_pHelloWorldMsg->m_sTeamName,_pHelloWorldMsg->m_sTankName
				,_pHelloWorldMsg->m_flag	
				);
			Sleep(10);
			client.save(&answer);
		}
		return true;
	}

	void on_error(const utils::IUtilitiesException& _exc,bool& _bCancel)
	{
		CString_ msg = FormatException(_exc);
	}
protected:
	long m_id;
	CGameWorld& m_gameworld;
};

PIPESERVRERTRAITS(CConnectionServer,ConnectionMessages,false,true);


