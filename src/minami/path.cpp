#include "path.h"

/**************************************************************************
                              Path
**************************************************************************/
MNM_Path::MNM_Path()
{
  m_link_vec = std::deque<TInt>();
  m_node_vec = std::deque<TInt>();
  m_buffer_length = 0;
  m_p = 0;
  buffer = NULL;
}


MNM_Path::~MNM_Path()
{
  m_link_vec.clear();
  m_node_vec.clear();
  if (buffer != NULL) free(buffer);
}


std::string MNM_Path::node_vec_to_string()
{
  std::string _s;
  for (TInt node_ID : m_node_vec){
    _s += std::to_string(node_ID) + " ";
  }
  _s.pop_back();
  _s += "\n";
  return _s;
}

int MNM_Path::allocate_buffer(TInt length){
  buffer = static_cast<TFlt*>(std::malloc(sizeof(TFlt) * length));
  return 0;
}

/**************************************************************************
                            Path Set
**************************************************************************/

MNM_Pathset::MNM_Pathset()
{
  m_path_vec = std::vector<MNM_Path*>();
}

bool MNM_Pathset::is_in(MNM_Path* path)
{
  for (MNM_Path *tmp_path : m_path_vec){
    if (*tmp_path == *path) return true;
  }
  return false;
}


int MNM_Pathset::normalize_p()
{
  TFlt _tot_p = TFlt(0);
  TFlt _min_p = TFlt(0);
  for (MNM_Path *_path : m_path_vec){
    // if (_path -> m_p < 0){
    //   printf("Negative probability, impossible!\n");
    //   exit(-1);
    // }
    if (_path -> m_p < _min_p){
      _min_p = _path -> m_p;
    }
  }
  for (MNM_Path *_path : m_path_vec){
    _tot_p += _path -> m_p - _min_p;
  }
  if (_tot_p == TFlt(0)){
    for (MNM_Path *_path : m_path_vec){
      _path -> m_p = TFlt(1)/TFlt(m_path_vec.size());
    }    
  }
  else{
    for (MNM_Path *_path : m_path_vec){
      _path -> m_p = (_path -> m_p - _min_p) / _tot_p;
    } 
  }
  return 0;
}


namespace MNM{

MNM_Path *extract_path(TInt origin_ID, TInt dest_ID, std::map<TInt, TInt> &output_map, PNEGraph &graph)
{
  TInt _current_node_ID = origin_ID;
  TInt _current_link_ID = -1;
  MNM_Path *_path = new MNM_Path();
  while(_current_node_ID != dest_ID){
    _current_link_ID = output_map.find(_current_node_ID) -> second;
    _path -> m_node_vec.push_back(_current_node_ID);
    _path -> m_link_vec.push_back(_current_link_ID);
    _current_node_ID = graph -> GetEI(_current_link_ID).GetDstNId();
  }
  _path -> m_node_vec.push_back(_current_node_ID);
  return _path;
}


Path_Table *build_pathset(PNEGraph &graph, MNM_OD_Factory *od_factory, MNM_Link_Factory *link_factory)
{
  /* setting */
  size_t MaxIter = 3;
  TFlt Mid_Scale = 3;
  TFlt Heavy_Scale = 6;

  /* initialize data structure */
  Path_Table *_path_table = new Path_Table();
  for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
    std::map<TInt, MNM_Pathset*> *_new_map = new std::map<TInt, MNM_Pathset*>();
    _path_table -> insert(std::pair<TInt, std::map<TInt, MNM_Pathset*>*>(_o_it -> second -> m_origin_node -> m_node_ID, _new_map));
    for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
      MNM_Pathset *_pathset = new MNM_Pathset();
      _new_map -> insert(std::pair<TInt, MNM_Pathset*>(_d_it -> second -> m_dest_node -> m_node_ID, _pathset));
    }
  }

  std::map<TInt, TInt> _mid_shortest_path_tree = std::map<TInt, TInt>();
  std::map<TInt, TInt> _heavy_shortest_path_tree = std::map<TInt, TInt>();
  std::map<TInt, TFlt> _heavy_cost_map = std::map<TInt, TFlt>();
  std::map<TInt, TFlt> _mid_cost_map = std::map<TInt, TFlt>();

  TInt _dest_node_ID, _origin_node_ID;
  std::map<TInt, TFlt> _free_cost_map =  std::map<TInt, TFlt>();
  std::map<TInt, TInt> _free_shortest_path_tree;
  MNM_Path *_path;
  for (auto _link_it = link_factory -> m_link_map.begin(); _link_it!= link_factory -> m_link_map.end(); _link_it++){
    _free_cost_map.insert(std::pair<TInt, TFlt>(_link_it -> first, _link_it -> second -> get_link_tt()));
  }
  for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
    _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
    MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, graph, _free_cost_map, _free_shortest_path_tree);
    for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
      _origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
      _path = MNM::extract_path(_origin_node_ID, _dest_node_ID, _free_shortest_path_tree, graph);
      _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec.push_back(_path);
    }
  }

  _mid_cost_map.insert(_free_cost_map.begin(), _free_cost_map.end());
  _heavy_cost_map.insert(_free_cost_map.begin(), _free_cost_map.end());

  MNM_Dlink *_link;
  size_t _CurIter = 0;
  while (_CurIter < MaxIter){
    printf("Current interval %d\n", (int) _CurIter);
    for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
      _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, graph, _free_cost_map, _free_shortest_path_tree);
      for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
        _origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
        for (auto &_path : _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec){
          for (auto &_link_ID : _path -> m_link_vec){
            _link = link_factory -> get_link(_link_ID);
            _mid_cost_map.find(_link -> m_link_ID) -> second = _link -> get_link_tt() * Mid_Scale;
            _heavy_cost_map.find(_link -> m_link_ID) -> second = _link -> get_link_tt() * Heavy_Scale;
          }
        }
      }
    }
    MNM_Path *_path_mid, *_path_heavy;
    for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
      _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, graph, _mid_cost_map, _mid_shortest_path_tree);
      MNM_Shortest_Path::all_to_one_FIFO(_dest_node_ID, graph, _heavy_cost_map, _heavy_shortest_path_tree);
      for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
        _origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
        _path_mid = MNM::extract_path(_origin_node_ID, _dest_node_ID, _mid_shortest_path_tree, graph);
        _path_heavy = MNM::extract_path(_origin_node_ID, _dest_node_ID, _heavy_shortest_path_tree, graph);
        if (! _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> is_in(_path_mid)){
          _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec.push_back(_path_mid);
        }
        if (! _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> is_in(_path_heavy)){
          _path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec.push_back(_path_heavy);
        }
      }
    }
    _CurIter += 1;
  }
  return _path_table;
}


int save_path_table(Path_Table *path_table, MNM_OD_Factory *od_factory)
{
  std::string _file_name = "path_table";
  std::ofstream _path_table_file;
  _path_table_file.open(_file_name, std::ofstream::out);     
  if (!_path_table_file.is_open()){
    printf("Error happens when open _path_table_file\n");
    exit(-1);
  }
  TInt _dest_node_ID, _origin_node_ID;
  for (auto _d_it = od_factory -> m_destination_map.begin(); _d_it != od_factory -> m_destination_map.end(); _d_it++){
    _dest_node_ID = _d_it -> second -> m_dest_node -> m_node_ID;
    for (auto _o_it = od_factory -> m_origin_map.begin(); _o_it != od_factory -> m_origin_map.end(); _o_it++){
      _origin_node_ID = _o_it -> second -> m_origin_node -> m_node_ID;
      for (auto &_path : path_table -> find(_origin_node_ID) -> second -> find(_dest_node_ID) -> second -> m_path_vec){
        _path_table_file << _path -> node_vec_to_string();
      }
    }
  }
  _path_table_file.close();
  return 0;
}

int allocate_path_table_buffer(Path_Table *path_table, TInt num)
{
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        _path -> allocate_buffer(num);
      }
    }
  }  
  return 0;
}

int normalize_path_table_p(Path_Table *path_table)
{
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      _it_it.second -> normalize_p();
    }
  }    
  return 0;
}

int copy_p_to_buffer(Path_Table *path_table, TInt col)
{
  for(auto _it : *path_table){
    for (auto _it_it : *(_it.second)){
      for (MNM_Path* _path : _it_it.second -> m_path_vec){
        _path -> buffer[col] = _path -> m_p;
      }
    }
  }  
  return 0;
}

}//end namespace MNM

