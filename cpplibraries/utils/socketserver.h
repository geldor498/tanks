#pragma once 

#include "typelist.h"
#include "serialize.h"
#include "socketserializer.h"
#include "memserializer.h"
#include "thread.h"

#include <boost\asio.hpp>

using boost::asio::ip::tcp;

namespace Private
{

template<class _TOwner,class _TMessagesTypeLst>
struct ProcessMessageT
{
	typedef typename _TMessagesTypeLst::Item Item;
	typedef typename _TMessagesTypeLst::Tail Tail;

	static void process_message(_TOwner& _obj,IArchive& _p)
	{
		CPtrShared<INode> pnode = _p.get_root();
		VERIFY_EXIT(NOT_NULL(pnode));
		pnode = pnode->get_child(get_structname<Item>());
		if(NOT_NULL(pnode))
		{
			Item item;
			load(_p,get_structname<Item>(),item,get_structlayout<Item>());
			_obj.process_message(item);
			return;
		}
		ProcessMessageT<_TOwner,Tail>::process_message(_obj,_p);
	}
};

template<class _TOwner>
struct ProcessMessageT<_TOwner,NullType>
{
	static void process_message(_TOwner& _obj,IArchive& _p)
	{
		CPtrShared<INode> pnode = _p.get_root();
		VERIFY_EXIT(NOT_NULL(pnode));
		CPtrShared<INodeList> pnodes = pnode->get_allchildlist();
		CString_ name;
		if(NOT_NULL(pnodes)) 
		{
			pnode = (*pnodes)[0];
			if(NOT_NULL(pnode))
				name = pnode->get_name();
		}
		_obj.unknown_message(name);
	}
};//

};//namespace Private

template<class _TOwner,class _TMessagesTypeLst>
struct CSocketMessages
{
	void read(tcp::socket& _socket)
	{
		CSocketLoader loader(_socket,static_cast<_TOwner*>(this));
		CMemoryArchive arch;
		loader.load(arch);

		Private::ProcessMessageT<_TOwner,_TMessagesTypeLst>::process_message(static_cast<_TOwner&>(*this),arch);
	}

	template<typename _Type>
	void process_message(const _Type&)
	{
	}

	void unknown_message(const CString_& _name)
	{
	}
};//template<> struct CSocketMessagesServer 

template<class _TOwner,class _TMessagesTypeLst>
class CSocketMessagesServer
:
	public CSocketMessages<_TOwner,_TMessagesTypeLst>
	,public CWorkedThreadImpl<CSocketMessagesServer<_TOwner,_TMessagesTypeLst> >
	//,public boost::enable_shared_from_this<CSocketMessagesServer<_TOwner,_TMessagesTypeLst> >
{
	typedef CSocketMessages<_TOwner,_TMessagesTypeLst> SocketMessages;
protected:
	boost::asio::io_service& m_io_service;
	tcp::endpoint m_endpoint;
	tcp::acceptor m_acceptor;
	tcp::socket* m_psocket;
	bool m_bdestruct;
	bool m_breconnect;
	CEvent m_acceppted;
	boost::system::error_code m_socketerr;
public:
	CSocketMessagesServer(
		boost::asio::io_service& _io_service
		,short _port
		)
		:m_io_service(_io_service)
		,m_endpoint(boost::asio::ip::tcp::v4(),_port)
		,m_acceptor(m_io_service,m_endpoint)
		,m_psocket(NULL)
		,m_bdestruct(false)
		,m_breconnect(false)
		,m_acceppted()
	{
		m_acceppted.ResetEvent();
		start();
		start_accept();
	}

	~CSocketMessagesServer()
	{
		m_bdestruct = true;
		m_io_service.stop();
		close();
		predestruct();
		if(NOT_NULL(m_psocket))
		{
			delete m_psocket;
			m_psocket = NULL;
		}
	}

	void close()
	{
		if(NOT_NULL(m_psocket))
		{
			m_psocket->close();
		}
	}

	tcp::socket& get_socket()
	{
		return *m_psocket;
	}

	void reconnect()
	{
		m_breconnect = true;
	}

	void start_accept()
	{
		if(IS_NULL(m_psocket)) 
		{
			m_psocket = new tcp::socket(m_io_service);
			m_acceptor.async_accept(
				*m_psocket
				,boost::bind(&CSocketMessagesServer::handle_accept,this,boost::asio::placeholders::error)
				);
		}
	}

	void on_server_error(utils::IUtilitiesException& _exc,bool& _bexit)
	{
		// rewrite this function to get error information from server
	}

protected:
	void handle_accept(const boost::system::error_code& _err)
	{
		m_socketerr = _err;
		m_acceppted.SetEvent();
	}

public:

	DWORD thread_main()
	{
		for(;;)
		{
			HANDLE h[] = {(HANDLE)m_acceppted,get_stophandle()};
			DWORD dwWaitRes = WaitForMultipleObjects(_countof(h),h,FALSE,INFINITE);
			if(dwWaitRes!=WAIT_OBJECT_0) return 0;

			try
			{
				if(m_socketerr) throw m_socketerr;

				read(*m_psocket);
				if(m_breconnect) 
				{
					delete m_psocket;
					m_psocket = NULL;
					m_breconnect = false;
				}
				start_accept();
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _pexc)
			{
				ExceptionPtr<utils::IUtilitiesException> pexc(_pexc);
				if(!m_bdestruct)
				{
					bool bexit = false;
					static_cast<_TOwner&>(*this).on_server_error(*pexc,bexit);
					if(bexit) return 0;
				}
			}
			catch(std::exception& _exc)
			{
				// TODO: add error handling 
				if(!m_bdestruct && !is_signaled(get_stophandle()))
				{
					string_converter<char,TCHAR> msgT(_exc.what());
					utils::CErrorTag __et(cnamedvalue(_T("what"),msgT));
					utils::CUtilitiesExceptionBase exc;
					exc.initialize_exception();

					bool bexit = false;
					static_cast<_TOwner&>(*this).on_server_error(exc,bexit);
					if(bexit) return 0;
				}
			}
			catch(boost::system::error_code& _err)
			{
				if(!m_bdestruct)
				{
					string_converter<char,TCHAR> msgT(_err.message().c_str());
					string_converter<char,TCHAR> catnameT(_err.category().name());
					string_converter<char,TCHAR> catmsgT(_err.category().message(0).c_str());
					utils::CErrorTag __et(
						cnamedvalue(_T("message"),msgT)
						,cnamedvalue(_T("category.name"),catnameT)
						,cnamedvalue(_T("category.message.0"),catmsgT)
						);
					utils::CUtilitiesExceptionBase exc;
					exc.initialize_exception();

					bool bexit = false;
					static_cast<_TOwner&>(*this).on_server_error(exc,bexit);
					if(bexit) return 0;
				}
			}
		}
	}
};//template<> class CSocketMessagesServer

