#include "Snap.h"

#include "path.h"
#include "io.h"
#include "shortest_path.h"
#include "ults.h"

#include <string>

int main()
{
  std::string file_folder = "../../data/input_files_new_philly";
  MNM_ConfReader *conf_reader = new MNM_ConfReader(file_folder + "/config.conf", "DTA");
  PNEGraph graph = MNM_IO::build_graph(file_folder, conf_reader);
  // TInt dest_node_ID = graph -> GetRndNId();
  TInt dest_node_ID = 151632;
  TInt max_interval = 4000;

  std::unordered_map<TInt, TFlt*> cost_map = std::unordered_map<TInt, TFlt*>();
  TFlt *tmp;
  printf("Assign randomized weights\n");
  for (auto e = graph->BegEI(); e < graph->EndEI(); e++) {
    // printf("link ID is %d", e.GetId());
    tmp = new TFlt[max_interval];
    for (int i=0; i<max_interval; ++i){
      tmp[i] = MNM_Ults::rand_flt();
      // printf(",%lf", tmp[i]());
    }
    // printf("\n");
    cost_map[e.GetId()] = tmp;
  }

  if (MNM_Shortest_Path::is_FIFO(graph, cost_map, max_interval, 0.5)) {
    printf("The graph is FIFO\n");
  }
  else {
    printf("The graph is not FIFO\n");
  }

  MNM_TDSP_Tree *tdst_tree = new MNM_TDSP_Tree(dest_node_ID, graph, max_interval);
  printf("Init TDSP tree\n");
  tdst_tree -> initialize();
  printf("Update tree\n");
  tdst_tree -> update_tree(cost_map);
  TFlt tmp_tt;
  for (int i=0; i<max_interval; ++i){
    printf("get distance to dest\n");
    tmp_tt = tdst_tree -> get_distance_to_destination(101152, TFlt(i));
    printf("At time %d, minimum tt is %f\n", i, tmp_tt());
  }
  
  for (auto e = graph->BegEI(); e < graph->EndEI(); e++) {
    delete cost_map[e.GetId()];
  }
  return 0;
}