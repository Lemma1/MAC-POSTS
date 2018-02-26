#include "routing.h"

/**************************************************************************
                          Pre determined Routing
**************************************************************************/

MNM_Routing_Predetermined::MNM_Routing_Predetermined(PNEGraph &graph,
              MNM_OD_Factory *od_factory, MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory
              ,Path_Table *p_table, MNM_Pre_Routing *pre_routing, TInt max_int)
  : MNM_Routing::MNM_Routing(graph, od_factory, node_factory, link_factory){
  	m_tracker = std::unordered_map<MNM_Veh*, std::deque<TInt>*>();
  	m_path_table = p_table;
  	m_pre_routing = pre_routing;
    m_total_assign_inter = max_int;

  }

int MNM_Routing_Predetermined::init_routing(Path_Table *path_table)
{
  // if (path_table == NULL){
  //   printf("Path table need to be set in Fixed routing.\n");
  //   exit(-1);
  // }
  // set_path_table(path_table);
  return 0;
}

MNM_Routing_Predetermined::~MNM_Routing_Predetermined()
{
  for (auto _map_it : m_tracker){
    _map_it.second -> clear();
    delete _map_it.second;
  }
  m_tracker.clear();
}

int MNM_Routing_Predetermined::update_routing(TInt timestamp){
  // question: the releasing frequency of origin is not every assignment interval?
  TInt _release_freq = m_od_factory -> m_origin_map.begin() -> second -> m_frequency;
  MNM_Origin *_origin;
  MNM_DMOND *_origin_node;
  MNM_Destination *_destination;
  TInt _node_ID, _next_link_ID;
  MNM_Dlink *_next_link;
  MNM_Veh *_veh;
  MNM_Path *_route_path;
  TInt _ass_int = timestamp/_release_freq;
  if (timestamp % _release_freq ==0  ){
    
    // need to register vehicles to m_tracker
    for (auto _origin_it = m_od_factory->m_origin_map.begin(); _origin_it != m_od_factory->m_origin_map.end(); _origin_it++){
      
      _origin = _origin_it -> second;
      _origin_node = _origin -> m_origin_node;
      _node_ID = _origin_node -> m_node_ID;

      // here assume that the order of dest in veh deq is the same as in the mdemand of origin
      // this is ensured by the release function of Origin
      auto _demand_it = _origin -> m_demand.begin();
      _destination = _demand_it -> first;
      TFlt _thisdemand = _demand_it ->second[_ass_int];
      int _id_path = 0;
      TFlt _remain_demand = m_pre_routing -> routing_table -> find(_origin -> m_origin_node -> m_node_ID)->
        second.find(_destination -> m_dest_node -> m_node_ID)->second.find(_id_path) -> second[_ass_int];
      
      for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
        _veh = *_veh_it;
        if(_veh -> get_destination() != _destination &&_remain_demand<=0 ){
          _destination = _veh -> get_destination();
          _remain_demand = m_pre_routing -> routing_table -> find(_origin -> m_origin_node -> m_node_ID)->
        second.find(_destination -> m_dest_node -> m_node_ID)->second.find(_id_path) -> second[_ass_int];
          _thisdemand = _origin -> m_demand.find(_destination) -> second[_ass_int];

          _id_path = 0;
        }else if(_remain_demand<=0 && _thisdemand > 0){
          _id_path ++;
          std::cout << _id_path << "," <<_origin -> m_origin_node -> m_node_ID << ","
           << _destination -> m_dest_node -> m_node_ID << "," << std::endl;
          _remain_demand = m_pre_routing -> routing_table -> find(_origin -> m_origin_node -> m_node_ID)->
        second.find(_destination -> m_dest_node -> m_node_ID)->second.find(_id_path) -> second[_ass_int];
        }else if(_remain_demand >0 && _thisdemand <0){
          std::cout<< "somthing wrong with the demand " <<std::endl;
          exit(1);
        }
        _thisdemand--;
        _remain_demand--;
        _route_path = m_path_table -> find(_veh -> get_origin() -> m_origin_node  -> m_node_ID) -> second
                        -> find(_veh -> get_destination() -> m_dest_node  -> m_node_ID) -> second 
                        -> m_path_vec[_id_path];
        std::deque<TInt> *_link_queue = new std::deque<TInt>();
        std::copy(_route_path -> m_link_vec.begin(), _route_path -> m_link_vec.end(), std::back_inserter(*_link_queue));
        m_tracker.insert(std::pair<MNM_Veh*, std::deque<TInt>*>(_veh, _link_queue));
      }  
      for (auto _veh_it = _origin_node -> m_in_veh_queue.begin(); _veh_it!=_origin_node -> m_in_veh_queue.end(); _veh_it++){
        _veh = *_veh_it;
        _next_link_ID = m_tracker.find(_veh) -> second -> front();
        _next_link = m_link_factory -> get_link(_next_link_ID);
        _veh -> set_next_link(_next_link);
        m_tracker.find(_veh) -> second -> pop_front();
        // std::cout << "vehicle " << _veh->m_veh_ID<<" next link: " << _next_link ->m_link_ID <<std::endl;
      }
    }

  }

  // step 1: register vehilces in the Origin nodes to m_tracker, update their next link





  // step 2: update the next link of all vehicles in the last cell of links
  MNM_Destination *_veh_dest;
  MNM_Dlink *_link;
  for (auto _link_it = m_link_factory -> m_link_map.begin(); _link_it != m_link_factory -> m_link_map.end(); _link_it ++){
    _link = _link_it -> second;
    _node_ID = _link -> m_to_node -> m_node_ID;
    // printf("2.1\n");
    for (auto _veh_it = _link -> m_finished_array.begin(); _veh_it!=_link -> m_finished_array.end(); _veh_it++){
      _veh = *_veh_it;
      _veh_dest = _veh -> get_destination();
      // printf("2.2\n");
      if (_veh_dest -> m_dest_node -> m_node_ID == _node_ID){
        if (m_tracker.find(_veh) -> second -> size() != 0){
          printf("Something wrong in fixed routing!\n");
          exit(-1);
        }
        _veh -> set_next_link(NULL);
        // m_tracker.erase(m_tracker.find(_veh));
      }
      else{
        // printf("2.3\n");
        if (m_tracker.find(_veh) == m_tracker.end()){
          printf("Vehicle not registered in link, impossible!\n");
          exit(-1);
        }
        if(_veh -> get_current_link() == _veh -> get_next_link()){
          _next_link_ID = m_tracker.find(_veh) -> second -> front();
          if (_next_link_ID == -1){
            printf("Something wrong in routing, wrong next link 2\n");
            printf("The node is %d, the vehicle should head to %d\n", (int)_node_ID, (int)_veh_dest -> m_dest_node -> m_node_ID);
            exit(-1);
          }
          _next_link = m_link_factory -> get_link(_next_link_ID);
          _veh -> set_next_link(_next_link);
          m_tracker.find(_veh) -> second -> pop_front();
        }
      }
    }
  }




	return 0;
}


