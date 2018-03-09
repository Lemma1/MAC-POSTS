

#include "pre_routing.h"

/**************************************************************************
                            Pre_Routing
**************************************************************************/
//std::unordered_map<TInt, std::unordered_map<TInt, MNM_Pathset*>*>
MNM_Pre_Routing::MNM_Pre_Routing(Path_Table *path_table,MNM_OD_Factory *od_factory){
	//std::unordered_map<TInt, std::unordered_map<TInt, MNM_Pathset*>*> <O_id,<D_id,pathset>>
	//first generate, assign all demand to one path
	routing_table = new std::unordered_map<TInt,std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>>();
	m_path_table = path_table;
	TInt _dest_node_ID, _origin_node_ID;
	TInt _oid;
	TInt _did;
	MNM_Pathset* _pset;


	// for (auto _ops = _path_table -> begin();_ops != _path_table ->end(); _ops++){
	// 	_oid = _ops -> first;
	// 	for (auto _dps = _ops -> second -> begin(); _dps != _ops -> second -> end(); _dps++){
	// 		_did = _dps -> first;
	// 		_pset = _dps -> second;
	// 		if (_pset -> m_path_vec.size() ==0)
	// 			continue;
	// 		else{







	for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
	    _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
	    for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
	    	_origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
	    	//-----
	    	int size_demand = _o_it -> second -> m_max_assign_interval;
	    	std::unordered_map<MNM_Destination*, TFlt*>::const_iterator demand_it = _o_it -> second -> m_demand.find(_d_it->second);
	    	if(demand_it != _o_it -> second -> m_demand.end()){
	    		TFlt* demand = demand_it->second;
	    		// int size_demand = sizeof(demand)/sizeof(demand[0]);

	    		// std::cout<< "length of demand " <<size_demand <<std::endl;
	    		TFlt* demand_copy = new TFlt[size_demand];
	    		for(int demand_it=0;demand_it<size_demand;demand_it++){
	    			demand_copy[demand_it] = demand[demand_it];
	    		}

	    		if(routing_table->find(_origin_node_ID) == routing_table->end()){
	    			std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>> _destmap = std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>();
	    			std::unordered_map<TInt,TFlt*> _path_demand_map = std::unordered_map<TInt,TFlt*>();
	    			_path_demand_map.insert(std::make_pair(0,demand_copy));
	    			int _path_size = path_table -> at(_origin_node_ID) -> at(_dest_node_ID) ->m_path_vec.size();
	    			// std::cout<<"Path Set Size:" << _path_size <<std::endl;
	    			
	    			for (int _k = 1;_k < _path_size;_k++){
	    				TFlt* demand_copy = new TFlt[size_demand];
	    				for(int demand_it=0;demand_it<size_demand;demand_it++){
	    					demand_copy[demand_it] = TFlt(0.0);
	    				}
	    				_path_demand_map.insert(std::make_pair(_k,demand_copy));

	    			}
	    			_destmap.insert(std::make_pair(_dest_node_ID,_path_demand_map));
	    			routing_table->insert(std::make_pair(_origin_node_ID,_destmap));
	    		}else{
	    			std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>> _destmap = std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>();
	    			std::unordered_map<TInt,TFlt*> _path_demand_map = std::unordered_map<TInt,TFlt*>();
	    			_path_demand_map.insert(std::make_pair(0,demand_copy));
	    			int _path_size = path_table -> at(_origin_node_ID) -> at(_dest_node_ID) ->m_path_vec.size();
	    			// std::cout<<"Path Set Size:" << _path_size <<std::endl;
	    			
	    			for (int _k = 1;_k < _path_size;_k++){
	    				TFlt* demand_copy = new TFlt[size_demand];
	    				for(int demand_it=0;demand_it<size_demand;demand_it++){
	    					demand_copy[demand_it] = TFlt(0.0);
	    				}
	    				_path_demand_map.insert(std::make_pair(_k,demand_copy));

	    			}
	    			_destmap.insert(std::make_pair(_dest_node_ID,_path_demand_map));

	    			routing_table->find(_origin_node_ID)->second.insert(std::make_pair(_dest_node_ID,_path_demand_map));
	    		}
	    	}
	    }
    }
}

MNM_Pre_Routing::~MNM_Pre_Routing(){
	for (auto _rt_it = routing_table->begin();_rt_it!=routing_table->end();_rt_it++){
		for(auto _rt_dit = _rt_it->second.begin();_rt_dit!=_rt_it->second.end();_rt_dit++){
			for(auto _demand_it = _rt_dit->second.begin();_demand_it!=_rt_dit->second.end();_demand_it++){
				free(_demand_it->second);
			}
			free(&_rt_dit->second);
		}
		free(&_rt_it->second);
	}
	free(routing_table);
}



int MNM_Pre_Routing::reassign_routing(TInt oid,TInt did, TInt pid, TInt interval, TFlt lambda){

	// TO DO: for period _int, in time _int, assign all demand to path pid
	// denote the new routing scheme as f_n, original as f_o
	// update routing table as f = (1-lambda)* f_o + lambda *f_n
	// the interval should already be divided by the unit length
	// std::cout << " Reassign.... at interval " << interval << std::endl;
	TFlt _total_demand = 0;
	for (auto _path_demand  = routing_table ->at(oid).at(did).begin();
			_path_demand != routing_table ->at(oid).at(did).end();_path_demand++){
		_total_demand += _path_demand -> second[interval];
	}
	// std::cout <<  "pre routing size: " << routing_table ->at(oid).at(did).size() <<std::endl;
	for (auto _path_demand  = routing_table ->at(oid).at(did).begin();
			_path_demand != routing_table ->at(oid).at(did).end();_path_demand++){
		if (_path_demand -> first == pid){
			_path_demand -> second[interval] = (1-lambda) * _path_demand -> second[interval] + lambda * _total_demand;
			// std::cout <<  _path_demand -> first << "path : " <<(1-lambda) * _path_demand -> second[interval] + lambda * _total_demand << std::endl;
		}else{
			// std::cout <<_path_demand -> first << "low path : " << (1-lambda) * _path_demand -> second[interval]<< std::endl;
			_path_demand -> second[interval] = (1-lambda) * _path_demand -> second[interval];
		}
	}
	// std::cout << "Within reassign:" << pid << std::endl;
	return 0;
}


std::string MNM_Pre_Routing::toString(){
	std::string result = "Pre routing table\n";
	MNM_Path * _path;
	for (auto _oit = routing_table ->begin();_oit!= routing_table->end(); _oit++){
		TInt _oid = _oit -> first;
		result += "From Origin " + std::to_string(_oid);
		for (auto _dit = _oit -> second.begin();_dit != _oit -> second.end();_dit ++){
			TInt _did = _dit -> first;
			result += "	to destination" + std::to_string(_did) +":\n";
			int _size = _dit->second.size();
			result += std::to_string(_size) + "paths\n";
			for (auto _pit = _dit->second.begin();_pit != _dit->second.end();_pit++){
				TInt _pid = _pit->first;
				MNM_Pathset* _path_set = m_path_table->at(_oid)->at(_did);
				_path  = _path_set -> m_path_vec[_pid];
				result += "Path " + std::to_string(_pid) + ":";
				for (size_t _l_it = 0;_l_it < _path->m_link_vec.size();_l_it++){
					result += std::to_string(_path->m_link_vec[_l_it]) + ",";
				}
				result += ": ";
				TFlt* _portion = _pit->second;
				for(int _kk = 0;_kk < 10;_kk++){
					result += std::to_string(_portion[_kk]) +",";
				}

				result += "\n";

			}
		}
	}
	return result;

}

int MNM_Pre_Routing::update_routing_table_MSA(MNM_PMC_Table pmc_table, float lambda){
	//TO DO, not only need to update the routing table, but also the demand of OD when no departure time choice
	// MNM::routing_table_multiply(routing_table,1-lambda);
	return 0;
}


int MNM_Pre_Routing::update_routing_table_MSA1(MNM_PMC_Table pmc_table_lower,MNM_PMC_Table pmc_table_upper,float lambda){
	//TO DO
	// MNM::routing_table_multiply(routing_table,1-lambda);
	return 0;
}

int MNM_Pre_Routing::update_routing_table_MSA2(MNM_PMC_Table pmc_table_lower,MNM_PMC_Table pmc_table_upper,float lambda){
	//TO DO
	// MNM::routing_table_multiply(routing_table,1-lambda);
	return 0;
}

int MNM_Pre_Routing::update_demand(MNM_OD_Factory *od_factory){
	// required in the case with departure time choice
	return 0;
}

int MNM_Pre_Routing::test_function(){
	return 0;
}





/**************************************************************************
                          help functions
**************************************************************************/
int routing_table_multiply(std::unordered_map<TInt,std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>> *routing_table,
		float lambda){
	for (auto _rt_it = routing_table->begin();_rt_it!=routing_table->end();_rt_it++){
		for(auto _rt_dit = _rt_it->second.begin();_rt_dit!=_rt_it->second.end();_rt_dit++){
			for(auto _demand_it = _rt_dit->second.begin();_demand_it!=_rt_dit->second.end();_demand_it++){
				TFlt* this_demand = _demand_it->second;
				int demand_size = sizeof(this_demand)/sizeof(this_demand[0]);
				for(int d_it=0;d_it<demand_size;d_it++){
					this_demand[d_it] *= lambda;
				}
			}
		}
	}
	return 0;
}