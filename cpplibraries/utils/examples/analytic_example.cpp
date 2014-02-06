#include <utils/analytics.h>

bool analytic_sample(StringLst& _names)
{
	const long ann_type = 6053;
	AnalyticLst ann_6053;
	
	//load analytic
	if(!analytic::find(ann_type,ann_6053))	// optimize for PriorSql creation
	{
		PriorSqlSwitch sql;
		sql.CreateConnect();		// create sql connection
		if(!analytic::load(ann_type,sql,ann_6053)) return false; // loading error
	}
	AnalyticLst ann_6053_filtered;
	// filter analytic
	analytic::filter(
		ann_6053					// source analytic list
		,ann_6053_filtered			// destination (filtered) analytic list
		,std::bind2nd(				// functor to filter with
			analytic::op_cmp_relation_anncode(6054)		// 6054 -- relation type
			,1											// 1 -- relation code to filter
			)
		);
	// return analytic names
	_names.clear();					// clear list
	AnalyticLst::const_iterator 
		it = ann_6053_filtered.begin()
		,ite = ann_6053_filtered.end()
		;
	for(;it!=ite;++it)
	{
		_names.push_back(it->m_name);	// add name to list
	}
	return true; // ok
}
