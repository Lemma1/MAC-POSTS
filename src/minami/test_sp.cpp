#include "Snap.h"

#include "path.h"
#include "io.h"
#include "shortest_path.h"

#include <ctime>
#include <omp.h>

int main()
{
  printf("Start!\n");
  std::string m_file_folder = "../../data/input_files_2link";
  // std::string m_file_folder = "../../data/input_files_new_philly";
  MNM_ConfReader *m_config;
  MNM_Node_Factory *m_node_factory;
  MNM_Link_Factory *m_link_factory;
  MNM_OD_Factory *m_od_factory;
  PNEGraph m_graph;

  m_node_factory = new MNM_Node_Factory();
  m_link_factory = new MNM_Link_Factory();
  m_od_factory = new MNM_OD_Factory();
  printf("config\n");
  m_config = new MNM_ConfReader(m_file_folder + "/config.conf", "DTA");

  printf("Node\n");
  MNM_IO::build_node_factory(m_file_folder, m_config, m_node_factory);
  printf("link\n");
  MNM_IO::build_link_factory(m_file_folder, m_config, m_link_factory);
  printf("od\n");
  MNM_IO::build_od_factory(m_file_folder, m_config, m_od_factory, m_node_factory);
  m_graph = MNM_IO::build_graph(m_file_folder, m_config);

  std::unordered_map<TInt, TFlt> cost_map = std::unordered_map<TInt, TFlt>();
  for (auto _it = m_link_factory -> m_link_map.begin(); _it != m_link_factory -> m_link_map.end(); ++_it){
    cost_map.insert(std::pair<TInt, TFlt>(_it -> first, TFlt(100)));
  }


  std::unordered_map<TInt, TInt> output_map = std::unordered_map<TInt, TInt>();
  MNM_Destination *_dest;
  TInt _dest_node_ID;
  clock_t startTime, endTime, clockTicksTaken;
  TFlt timeInSeconds;

/**************************************************************************
                          Test different algorithm
**************************************************************************/

  startTime = clock();
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
  // #pragma omp task firstprivate(_it)
    // {
      _dest = _it -> second;
      _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
      output_map.clear();
      MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, cost_map, output_map);
    // } 
  }
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for dijkstra is %f seconds.\n", timeInSeconds());


  startTime = clock();
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
  // #pragma omp task firstprivate(_it)
    // {
      _dest = _it -> second;
      _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
      output_map.clear();
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, cost_map, output_map);
      // MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
    // } 
  }
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for FIFO is %f seconds.\n", timeInSeconds());

  // startTime = clock();
  // for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
  // // #pragma omp task firstprivate(_it)
  //   // {
  //     _dest = _it -> second;
  //     _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
  //     output_map.clear();
  //     printf("Current ID is:%d\n", _dest_node_ID());
  //     MNM_Shortest_Path::all_to_one_LIFO(_dest_node_ID, m_graph, cost_map, output_map);
  //     // MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
  //   // } 
  // }
  // endTime = clock();
  // clockTicksTaken = endTime - startTime;
  // timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  // printf("The time using for LIFO is %f seconds.\n", timeInSeconds());
  // MNM_Path *p = MNM::extract_path(1, 6, output_map, m_graph);


/**************************************************************************
                          Test multithread 
**************************************************************************/
  
  startTime = clock();

  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
      _dest = _it -> second;
      _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
      std::unordered_map<TInt, TInt> *_shortest_path_tree = new std::unordered_map<TInt, TInt>();
      // std::unordered_map<TInt, TInt> *_shortest_path_tree = new std::unordered_map<TInt, TInt>();
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, cost_map, *_shortest_path_tree);
  }
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for FIFO and single thread is %f seconds.\n", timeInSeconds());

  startTime = clock();
  std::vector<MNM_Destination*> elements; //my_element is whatever is in list
  for(auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); ++_it){
    elements.push_back(_it -> second);
  }
  printf("Finished building list, the size is %d\n", elements.size());
#pragma omp parallel for
  for(size_t i = 0; i < elements.size(); ++i){
    _dest = elements[i];
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    std::unordered_map<TInt, TInt> *_shortest_path_tree = new std::unordered_map<TInt, TInt>();
    MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, cost_map, *_shortest_path_tree);
    // MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, m_statistics -> m_record_interval_tt, *_shortest_path_tree);
  }

  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for FIFO and openMP is %f seconds.\n", timeInSeconds());
  
  return 0;
}


