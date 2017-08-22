

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
	for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
	    _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
	    for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
	    	_origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
	    	//-----
	    	std::unordered_map<MNM_Destination*, TFlt*>::const_iterator demand_it = _o_it -> second -> m_demand.find(_d_it->second);
	    	if(demand_it != _o_it -> second -> m_demand.end()){
	    		TFlt* demand = demand_it->second;
	    		int size_demand = sizeof(demand)/sizeof(demand[0]);
	    		TFlt* demand_copy = new TFlt[size_demand];
	    		for(int demand_it=0;demand_it<size_demand;demand_it++){
	    			demand_copy[demand_it] = demand[demand_it];
	    		}

	    		if(routing_table->find(_origin_node_ID) == routing_table->end()){
	    			std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>> _destmap = std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>();
	    			std::unordered_map<TInt,TFlt*> _path_demand_map = std::unordered_map<TInt,TFlt*>();
	    			_path_demand_map.insert(std::make_pair(0,demand_copy));
	    			_destmap.insert(std::make_pair(_dest_node_ID,_path_demand_map));
	    			routing_table->insert(std::make_pair(_origin_node_ID,_destmap));
	    		}else{
	    			std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>> _destmap = std::unordered_map<TInt,std::unordered_map<TInt,TFlt*>>();
	    			std::unordered_map<TInt,TFlt*> _path_demand_map = std::unordered_map<TInt,TFlt*>();
	    			_path_demand_map.insert(std::make_pair(0,demand_copy));
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


int MNM_Pre_Routing::update_routing_table_MSA(MNM_PMC_Table pmc_table, float lambda){
	//TO DO
	MNM::routing_table_multiply(routing_table,1-lambda);
	return 0;
}


int MNM_Pre_Routing::update_routing_table_MSA1(MNM_PMC_Table pmc_table_lower,MNM_PMC_Table pmc_table_upper,float lambda){
	//TO DO
	MNM::routing_table_multiply(routing_table,1-lambda);
	return 0;
}

int MNM_Pre_Routing::update_routing_table_MSA2(MNM_PMC_Table pmc_table_lower,MNM_PMC_Table pmc_table_upper,float lambda){
	//TO DO
	MNM::routing_table_multiply(routing_table,1-lambda);
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