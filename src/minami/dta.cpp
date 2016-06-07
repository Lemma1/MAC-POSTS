#include "dta.h"

MNM_Dta::MNM_Dta(std::string file_folder)
{
  m_file_folder = file_folder;
  initialize();
}

int MNM_Dta::initialize()
{
  m_veh_factory = new MNM_Veh_Factory();
  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
  m_od_factory = new MNM_OD_Factory();
  m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");
  m_unit_time = m_config -> get_int("unit_time");
  m_flow_scalar = m_config -> get_int("flow_scalar");


  return 0;
}


int MNM_Dta::build_from_files()
{
  MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  // std::cout << test_dta -> m_node_factory -> m_node_map.size() << "\n";
  MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  // std::cout << test_dta -> m_link_factory -> m_link_map.size() << "\n";
  MNM_IO::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  // std::cout << test_dta -> m_od_factory -> m_origin_map.size() << "\n";
  // std::cout << test_dta -> m_od_factory -> m_destination_map.size() << "\n";
  m_graph = MNM_IO::build_graph(m_file_folder, m_config);
  MNM_IO::build_demand(m_file_folder, m_config, m_od_factory);
  return 0;  
}

int MNM_Dta::hook_up_linkANDnode()
{
  TInt __node_ID;
  MNM_Dnode *__node;
  MNM_Dlink *__link;
  // traverse the nodes
  for (TNGraph::TNodeI __node_it = m_graph->BegNI(); __node_it < m_graph->EndNI(); __node_it++) {
    printf("node id %d with out-degree %d and in-degree %d\n",
      __node_it.GetId(), __node_it.GetOutDeg(), __node_it.GetInDeg());
    __node_ID = __node_it.GetId();
    __node = m_node_factory -> get_node(__node_ID);
    for (int e = 0; e < __node_it.GetOutDeg(); ++e) {
      printf("Out: edge (%d %d)\n", __node_it.GetId(), __node_it.GetOutNId(e));
      __link = m_link_factory -> get_link(__node_it.GetOutNId(e));
      __node -> add_out_link(__link);
    }
    for (int e = 0; e < __node_it.GetInDeg(); ++e) {
      printf("In: edge (%d %d)\n", __node_it.GetInNId(e), __node_it.GetId());
      __link = m_link_factory -> get_link(__node_it.GetInNId(e));
      __node -> add_in_link(__link);
    }   
  }
  return 0;
}


/*
 *  perform a series of checks to ensure we can run the DTA model
 */
bool MNM_Dta::is_ok()
{
  bool __flag = true;
  bool __temp_flag = true;
  //Checks the graph data structure for internal consistency.
  //For each node in the graph check that its neighbors are also nodes in the graph.
  printf("Checking......Graph consistent!\n");
  __temp_flag = m_graph -> IsOk(); 
  __flag = __flag && __temp_flag;
  if (__temp_flag)  printf("Passed!\n");

  //check node
  printf("Checking......Node consistent!\n");
  __temp_flag = (m_graph -> GetNodes() == m_config -> get_int("num_of_node"))
                && (m_graph -> GetNodes() == TInt(m_node_factory -> m_node_map.size()));
  __flag = __flag && __temp_flag;
  if (__temp_flag)  printf("Passed!\n");

  //check link
  printf("Checking......Link consistent!\n");
  __temp_flag = (m_graph -> GetEdges() == m_config -> get_int("num_of_link"))
                && (m_graph -> GetEdges() == TInt(m_link_factory -> m_link_map.size()));
  __flag = __flag && __temp_flag;
  if (__temp_flag)  printf("Passed!\n");  

  //check OD node
  printf("Checking......OD consistent!\n");
  TInt __node_ID;
  __temp_flag = (TInt(m_od_factory -> m_origin_map.size()) == m_config -> get_int("num_of_O"))
                && (TInt(m_od_factory -> m_destination_map.size()) == m_config -> get_int("num_of_D"));
  std::map<TInt, MNM_Origin*>::iterator __origin_map_it;
  for (__origin_map_it = m_od_factory->m_origin_map.begin();
       __origin_map_it != m_od_factory->m_origin_map.end(); __origin_map_it++){
    __node_ID = __origin_map_it -> second -> m_origin_node -> m_node_ID;
    __temp_flag = __temp_flag && ((m_graph -> GetNI(__node_ID)).GetId() == __node_ID)
                  && (m_graph -> GetNI(__node_ID).GetOutDeg() == 1)
                  && (m_graph -> GetNI(__node_ID).GetInDeg() == 0);
  }
  std::map<TInt, MNM_Destination*>::iterator __dest_map_it;
  for (__dest_map_it = m_od_factory->m_destination_map.begin();
       __dest_map_it != m_od_factory->m_destination_map.end(); __dest_map_it++){
    __node_ID = __dest_map_it -> second -> m_dest_node -> m_node_ID;
    __temp_flag = __temp_flag && ((m_graph -> GetNI(__node_ID)).GetId() == __node_ID)
                  && (m_graph -> GetNI(__node_ID).GetOutDeg() == 0)
                  && (m_graph -> GetNI(__node_ID).GetInDeg() == 1);
  }  
  __flag = __flag && __temp_flag;
  if (__temp_flag)  printf("Passed!\n");  
  return __flag;
}


int MNM_Dta::run(bool verbose)
{
  TInt __cur_int = 0;

  // step 1: Origin release vehicle
  // step 2: route the vehicle
  // step 3: move vehicles through node
  // step 4: move vehicles through link
  // step 5: Destination receive vehicle

  return 0;
}
