#pragma once

#include "messages.h"
#include "gameworld.h"

#include <pipe/server.h>
#include <pipe/client.h>
#include <utils/typelist.h>
#include <utils/security.h>

typedef TYPELIST_1(CHelloWorld) ConnectionMessages;

struct CConnectionServer 
	:public CServerPipeCommImpl<CConnectionServer,ConnectionMessages>
{
protected:
	CSecurityAttributes m_sa;
public:

	CConnectionServer(CGameWorld& _gameworld)
		:CServerPipeCommImpl(g_szTankBattleConnectServer)
		,m_gameworld(_gameworld)
		,m_id(0)
		,m_rand_seed_initialized(false)
	{
		CSid everyone = security::Sids::World();
		CSid owner = security::Sids::Self();

		CDacl dacl;
		dacl.AddDeniedAce(everyone,security::AccessRights_WriteDac);
		dacl.AddAllowedAce(everyone,security::AccessRights_GenericWrite|security::AccessRights_GenericRead);
		dacl.AddAllowedAce(owner,security::AccessRights_GenericAll);

		CSecurityDesc secrdesc;
		secrdesc.SetDacl(dacl);

		m_sa.Set(secrdesc);
		m_psa = &m_sa;
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
			client.save(&answer);
		}
		return true;
	}

	void on_error(const utils::IUtilitiesException& _exc,bool& _bCancel)
	{
		CString_ msg = FormatException(_exc);
	}

	void on_connected()
	{
		if(m_rand_seed_initialized) return;
		srand((unsigned)time(NULL));
		m_rand_seed_initialized = true;
	}
protected:
	long m_id;
	CGameWorld& m_gameworld;
	bool m_rand_seed_initialized;
};

PIPESERVRERTRAITS(CConnectionServer,ConnectionMessages,true,true);


