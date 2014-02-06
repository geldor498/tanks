#if !defined(__IDHELPER_H__)
#define __IDHELPER_H__
#pragma once

#include <set>

namespace Utils
{

class ItemIDAccessor;

interface IItemIDHolder
{
	virtual void clear_ID() = 0;
};

#define INVALID_ID_NUM ((DWORD)(-1))

class ItemID
{
protected:
	DWORD id;
	IItemIDHolder* pholder;
public:
	ItemID():id(INVALID_ID_NUM),pholder(NULL){}
	~ItemID() {if(NOT_NULL(pholder))pholder->clear_ID();}
	void invalidate() 
	{
		if(NOT_NULL(pholder))pholder->clear_ID();
		pholder = NULL;
		id = INVALID_ID_NUM;
	}
	operator bool() const {return NEQL(id,INVALID_ID_NUM);}
	friend bool operator==(const ItemID& _id1,const ItemID& _id2);
	friend bool operator<(const ItemID& _id1,const ItemID& _id2);
	friend class ItemIDAccessor;
	void trace() {TRACE("id=%d",id);} 
};

inline
bool operator==(const ItemID& _id1,const ItemID& _id2)
{
	return EQL(_id1.id,_id2.id);
}

inline
bool operator<(const ItemID& _id1,const ItemID& _id2)
{
	return _id1.id<_id2.id;
}

//class IDWithDataAccessor;
template<typename _IDData>
class IDHelper;

template<typename _IDData>
struct IDWithData
: public IItemIDHolder
{
protected:
	ItemID* m_pid;
	DWORD cnt;
public:
	_IDData data;
	IDWithData():m_pid(NULL),cnt(0){};
	IDWithData(Utils::ItemID* _pid,int _cnt=0):m_pid(_pid),cnt(_cnt){ASSERT(NOT_NULL(_pid));}
	bool operator<(const IDWithData<_IDData>& _d)
	{
		return !*this || ((bool)_d && *m_pid<*_d.m_pid);
	}
	bool operator==(const IDWithData<_IDData>& _d)
	{
		return (bool)*this && (bool)_d && EQL(*m_pid,*_d.m_pid);
	}
	operator bool() const {return NOT_NULL(m_pid) && (bool)*m_pid;}
	IDWithData<_IDData>& operator++() {cnt++;return *this;}
//	void set_counter(int _cnt) {cnt = _cnt;}
//	DWORD get_counter() const {return cnt;}
//	Utils::ItemID* get_ID() {return m_pid;}
	IItemIDHolder* getHolder() {return this;}
	bool operator==(const ItemID& _id) const {return (bool)*this && *m_pid==_id;}
protected:
	virtual void clear_ID() {m_pid = NULL;}

	friend class IDHelper<_IDData>;
};


class ItemIDAccessor
{
protected:
	Utils::ItemID& id;
public:
	ItemIDAccessor(Utils::ItemID& _id):id(_id){};
	DWORD get() const {return id.id;}
	void set(DWORD _id) {id.id=_id;}
	void setIDHolder(Utils::IItemIDHolder* _pholder) {id.pholder=_pholder;}
	ItemIDAccessor& operator++() {id.id++;return *this;}
	static ItemIDAccessor get(Utils::ItemID& _id) {ItemIDAccessor _(_id);return _;}
};

template<typename _IDData>
class IDHelper
{
protected:
	typedef std::set<IDWithData<_IDData> > IDLst;
	IDLst m_idlst;
	IDWithData<_IDData> m_invalidid;
	DWORD m_cnt;
public:
	IDHelper() :m_cnt(0){}
	~IDHelper() {clear();}

	IDWithData<_IDData>* reset_ID(Utils::ItemID* _id)
	{
		if(IS_NULL(_id)) return &m_invalidid;
		if((bool)*_id)
		{
			IDLst::iterator it;
			it = m_idlst.find(IDWithData<_IDData>(_id,m_cnt));
			if(NEQL(it,m_idlst.end())) 
			{
				IDWithData<_IDData>& id = *it;
				id.cnt=m_cnt;
				return &*it;
			}
			ASSERT(NEQL(it,m_idlst.end()));
		}
		Utils::ItemID id1,id0;
		ItemIDAccessor::get(id0).set(MAKELONG(MAKEWORD(rand()&0xff,rand()&0xff)
			,MAKEWORD(rand()&0xff,rand()&0xff)));
		if(!id0) ++ItemIDAccessor::get(id0);
		for(id1=id0;;)
		{
			if(NEQL(m_idlst.find(IDWithData<_IDData>(&id1)),m_idlst.end()))
			{
				++ItemIDAccessor::get(id1);
				if(!id1) ++ItemIDAccessor::get(id1);
				if(NEQL(id1,id0)) continue;
				return &m_invalidid;
			}
			*_id = id1;
			std::pair<IDLst::iterator,bool> res = m_idlst.insert(IDWithData<_IDData>(_id,m_cnt));
			ASSERT(res.second);
			ItemIDAccessor::get(*_id).setIDHolder(res.first->getHolder());
			return &*res.first;
		}
		//ASSUME0();
	}

	IDWithData<_IDData>* get(const Utils::ItemID* const _id)
	{
		if(IS_NULL(_id)) return NULL;
		if(!(bool)*_id) return NULL;
		IDLst::iterator it;
		IDWithData<_IDData> key((Utils::ItemID*)_id,m_cnt);
		it = m_idlst.find(key);
		if(NEQL(it,m_idlst.end())) return &*it;
		return NULL;
	}

	void erase(IDWithData<_IDData>* _pIDRef)
	{
		if(IS_NULL(_pIDRef)) return;
		IDLst::iterator it;
		it = m_idlst.find(*_pIDRef);
		ASSERT(NEQL(it,m_idlst.end()));
		if(EQL(it,m_idlst.end())) return;
		m_idlst.erase(it);
	}

	void eraseold(DWORD _diff=1)
	{
		m_cnt++;
		IDLst::iterator it;
		ASSERT(_diff<(UINT_MAX>>1));
		it = m_idlst.begin();
		for(;it!=m_idlst.end();)
		{
			IDWithData<_IDData>& id = *it;
			if((bool)id)
			{
				if(id.cnt-m_cnt>_diff) {++it;continue;}
				ItemIDAccessor::get(*id.m_pid).set(INVALID_ID_NUM);
				ItemIDAccessor::get(*id.m_pid).setIDHolder(NULL);
			}
			it = m_idlst.erase(it);
		}
	}

	void clear()
	{
		m_cnt=0;
		IDLst::iterator it,ite;
		it = m_idlst.begin();
		ite = m_idlst.end();
		for(;it!=ite;++it)
		{
			Utils::IDWithData<_IDData>& id = *it;
			if(!(bool)id) continue;
			ItemIDAccessor::get(*id.m_pid).set(INVALID_ID_NUM);
			ItemIDAccessor::get(*id.m_pid).setIDHolder(NULL);
		}
		m_idlst.clear();
	}
	void traceIDList(){}
	void eraseinvalid()
	{
		IDLst::iterator it,ite;
		it = m_idlst.begin();
		for(;it!=m_idlst.end();)
		{
			Utils::IDWithData<_IDData>& id = *it;
			if((bool)id) {++it;continue;};
			it= m_idlst.erase(it);
		}
	}

};

}

#endif //#if !defined(__IDHELPER_H__)