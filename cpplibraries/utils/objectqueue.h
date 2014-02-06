#if !defined(__OBJECTQUEUE_H__185B9DB4_E048_4E45_800A_EBC2153FB5CC_INCLUDED)
#define __OBJECTQUEUE_H__185B9DB4_E048_4E45_800A_EBC2153FB5CC_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "utils.h"
#include "serialize.h"

/**\brief queue over IArchive (FIFO)
You can put some object and that get object from queue.
*/
struct CObjectsQueue
{
	/**\brief constructor
	\param _parch -- \ref IArchive object that is used to build queue over
	*/
	CObjectsQueue(IArchive* _parch)
		:m_parchive(_parch)
	{
	}

	/**\brief function to get object from queue with specified compare object
	\return true if object was founded
	*/
	template<typename _MessageType,typename _CmpType>
		bool get(_MessageType& _msg,const _CmpType& _cmpdta)
	{
		VERIFY_EXIT1(NOT_NULL(m_parchive),false);
		CPtrShared<INode> proot = m_parchive->get_root();
		VERIFY_EXIT1(NOT_NULL(proot),false);
		GUID guid = _MessageType::get_guid();
		CString_ sGUID;
		common::convert(guid,sGUID);
		CPtrShared<INodeList> pnodes = proot->get_childlist(sGUID);
		if(!pnodes->size()) return false;
		long i = 0;
		for(i=0;i<(long)pnodes->size();i++)
		{
			_MessageType msg;
			const INode* pnode = (*pnodes)[i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			get_structlayout<_MessageType>().load(*pnode,msg);
			if(msg==_cmpdta)
			{
				get_structlayout<_MessageType>().load(*((*pnodes)[i]),_msg);
				pnodes->remove(i);
				return true;
			}
		}
		return true;
	}

	/**\brief function to get object from queue
	\return true if data was founded
	*/
	template<typename _MessageType>
		bool get(_MessageType& _msg)
	{
		VERIFY_EXIT1(NOT_NULL(m_parchive),false);
		CPtrShared<INode> proot = m_parchive->get_root();
		VERIFY_EXIT1(NOT_NULL(proot),false);
		GUID guid = _MessageType::get_guid();
		CString_ sGUID;
		common::convert(guid,sGUID);
		CPtrShared<INodeList> pnodes = proot->get_childlist(sGUID);
		if(!pnodes->size()) return false;
		const INode* pnode = (*pnodes)[0];
		VERIFY_EXIT1(NOT_NULL(pnode),false);
		get_structlayout<_MessageType>().load(*pnode,_msg);
		pnodes->remove(0);
		return true;
	}

	/**\brief function to add object to queue
	*/
	template<typename _MessageType>
		void put(const _MessageType& _msg)
	{
		VERIFY_EXIT(NOT_NULL(m_parchive));
		CPtrShared<INode> proot = m_parchive->get_root();
		VERIFY_EXIT(NOT_NULL(proot));
		GUID guid = _MessageType::get_guid();
		CString_ sGUID;
		common::convert(guid,sGUID);
		CPtrShared<INode> pnode = proot->add_child(sGUID);
		VERIFY_EXIT(NOT_NULL(pnode));
		get_structlayout<_MessageType>().save(*pnode,_msg);
	}

protected:
	IArchive* m_parchive;				///< \ref IArchive object
};//struct CObjectsQueue



#endif//#if !defined(__OBJECTQUEUE_H__185B9DB4_E048_4E45_800A_EBC2153FB5CC_INCLUDED)