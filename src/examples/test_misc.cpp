#include <ctime>
#include <queue>
#include <cstdlib>

#include "Snap.h"

#include "shortest_path.h"
#include "io.h"
#include "ults.h"


int DijkstraBinaryHeapWithVisitedLabel(TInt destination_ID, 
                        PNEGraph graph, std::unordered_map<TInt, TFlt> &cost_map,
                        std::unordered_map<TInt, TInt> &output_map)
{
  std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost> m_Q \
  = std::priority_queue<MNM_Cost*, std::vector<MNM_Cost*>, LessThanByCost>();
  MNM_Cost *dest_cost = new MNM_Cost(destination_ID, TFlt(0));
	m_Q.push(dest_cost);

  std::unordered_map<TInt, TFlt> dist_to_dest = std::unordered_map<TInt, TFlt>();
  std::unordered_map<TInt, bool> visited = std::unordered_map<TInt, bool>();

  for (auto _node_it = graph->BegNI(); _node_it < graph->EndNI(); _node_it++){
    TInt _node_id = _node_it.GetId();
    if (_node_id != destination_ID){
      dist_to_dest.insert({_node_id, TFlt(std::numeric_limits<double>::max())});
      output_map.insert({_node_id, -1});
    }
    visited.insert({_node_id, false});
  }
  dist_to_dest[destination_ID] = TFlt(0);

  while (m_Q.size() != 0){
    MNM_Cost *_min_cost = m_Q.top();
    m_Q.pop();
    TInt _node_id = _min_cost->m_ID;
    auto _node_it = graph->GetNI(_node_id);
    TFlt _tmp_dist = dist_to_dest[_node_id]; 
    if (not visited[_node_id]){
      visited[_node_id] = true;
      for (int e = 0; e < _node_it.GetInDeg(); e++){
        TInt _in_node_id = _node_it.GetInNId(e);
        TInt _in_link_id = graph->GetEI(_in_node_id, _node_id).GetId();
        TFlt _alt = _tmp_dist + cost_map[_in_link_id];
        if (_alt < dist_to_dest[_in_node_id]){
          dist_to_dest[_in_node_id] = _alt;
          m_Q.push(new MNM_Cost(_in_node_id, _alt));
          output_map[_in_node_id] = _in_link_id;
        }
      }
    }
    delete _min_cost;
  }

  return 0;
}



int main(int argc, char const *argv[])
{
  printf("Start!\n");
  // std::string m_file_folder = "../../data/input_files_";
  std::string m_file_folder = "../../data/input_files_new_philly";
  // std::string m_file_folder = "../../data/input_files_PGH";
  // std::string m_file_folder = "../../data/input_files_SR41";
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
    cost_map.insert(std::pair<TInt, TFlt>(_it -> first, MNM_Ults::rand_flt()));
  }


  std::unordered_map<TInt, TInt> output_map = std::unordered_map<TInt, TInt>();
  std::unordered_map<TInt, TInt> output_map2 = std::unordered_map<TInt, TInt>();
  std::unordered_map<TInt, TInt> output_map3 = std::unordered_map<TInt, TInt>();
  MNM_Destination *_dest;
  TInt _dest_node_ID;
  clock_t startTime, endTime, clockTicksTaken;
  TFlt timeInSeconds;

/**************************************************************************
                          Test different algorithm
**************************************************************************/
  startTime = clock();
  // int tot_dest = 0;
  // int tot_count1 = 0;
  // int tot_count2 = 0;
  // int tot_count3 = 0;
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
  	// tot_dest++;
    _dest = _it -> second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    output_map.clear();
    DijkstraBinaryHeapWithVisitedLabel(_dest_node_ID, m_graph, cost_map, output_map);
    // output_map2.clear();
    // MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, cost_map, output_map2);
    // output_map3.clear();
    // MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, cost_map, output_map3);
    // printf("Testing for the %d-th Destination with ID: %d\n", tot_dest, _dest_node_ID); 

  //   int count1 = 0;
  //   int count2 = 0;
  //   int count3 = 0;
  //   for (auto _it = output_map.begin(); _it!=output_map.end(); _it++){
  //     TInt _node_id = _it->first;
  //     TInt _link_id = _it->second;
  //     if (output_map2[_node_id] != _link_id){
  //       count1++;
  //       // printf("node %d: [head to %d]; [head to %d]\n", _node_id, _link_id, output_map2[_node_id]);
  //     }
  //   }
  //   tot_count1 += count1;
  //   printf("dijkstra-withvisitlabel vs dijkstra: %d difference\n", count1); 
  //   printf("=====================================================================\n");
  //   for (auto _it = output_map.begin(); _it!=output_map.end(); _it++){
  //     TInt _node_id = _it->first;
  //     TInt _link_id = _it->second;
  //     if (output_map3[_node_id] != _link_id){
  //       count2++;
  //       // printf("node %d: [head to %d]; [head to %d]\n", _node_id, _link_id, output_map3[_node_id]);
  //     }
  //   }
  //   tot_count2 += count2;
  //   printf("dijkstra-withvisitlabel vs LC-FIFO: %d difference\n", count2); 
  //   printf("=====================================================================\n");
  //   for (auto _it = output_map2.begin(); _it!=output_map2.end(); _it++){
  //     TInt _node_id = _it->first;
  //     TInt _link_id = _it->second;
  //     if (output_map3[_node_id] != _link_id){
  //       count3++;
  //       // printf("node %d: [head to %d]; [head to %d]\n", _node_id, _link_id, output_map3[_node_id]);
  //     }
  //   }
  //   tot_count3 += count3;
  //   printf("dijkstra vs LC-FIFO: %d difference\n", count3);
  //   printf("=====================================================================\n\n\n"); 

  //   // break;
  }
  // printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  // printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  // printf("\n+                 Difference for all %d destinations:              +\n", tot_dest);
  // printf("\n+          dijkstra-withvisitlabel vs dijkstra: %d difference        +\n", tot_count1);
  // printf("\n+          dijkstra-withvisitlabel vs LC-FIFO: %d difference         +\n", tot_count2);
  // printf("\n+                dijkstra vs LC-FIFO: %d difference                  +\n", tot_count3);
  // printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  // printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for dijkstra-withvisitlabel is %f seconds.\n", timeInSeconds());


  startTime = clock();
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){
  	_dest = _it -> second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    output_map.clear();
    MNM_Shortest_Path::all_to_one_Dijkstra(_dest_node_ID, m_graph, cost_map, output_map);
  }
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for dijkstra is %f seconds.\n", timeInSeconds());


  startTime = clock();
  for (auto _it = m_od_factory -> m_destination_map.begin(); _it != m_od_factory -> m_destination_map.end(); _it++){      
  	_dest = _it -> second;
    _dest_node_ID = _dest -> m_dest_node -> m_node_ID;
    output_map.clear();
    MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, m_graph, cost_map, output_map);
  }
  endTime = clock();
  clockTicksTaken = endTime - startTime;
  timeInSeconds = TFlt(clockTicksTaken / (double) CLOCKS_PER_SEC);
  printf("The time using for FIFO is %f seconds.\n", timeInSeconds());


  return 0;
}