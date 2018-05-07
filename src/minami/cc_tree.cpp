#include "dlink.h"
#include "path.h"

MNM_Tree_Cumulative_Curve::MNM_Tree_Cumulative_Curve()
{
  m_record = std::unordered_map<MNM_Path*, std::unordered_map<TInt, MNM_Cumulative_Curve*>>();
}


MNM_Tree_Cumulative_Curve::~MNM_Tree_Cumulative_Curve()
{
  for (auto outer_it : m_record){
    for (auto inner_it : outer_it.second){
      delete inner_it.second;
    }
  }
  for (auto outer_it : m_record){
    outer_it.second.clear();
  }
  m_record.clear();

}

int MNM_Tree_Cumulative_Curve::add_flow(TFlt timestamp, TFlt flow, MNM_Path* path, TInt departing_int)
{
  // printf("CC tree, adding flow, time %d, flow %f, path id %d, depart time %d \n", (int) timestamp(), (float) flow(), (int) path -> m_path_ID, (int) departing_int);
  if (m_record.find(path) == m_record.end()){
    m_record[path] = std::unordered_map<TInt, MNM_Cumulative_Curve*>();
  }
  if (m_record[path].find(departing_int) == m_record[path].end()){
    // printf("Create new cc with path id %d, depart time %d\n", path -> m_path_ID, departing_int);
    m_record[path][departing_int] = new MNM_Cumulative_Curve();
    m_record[path][departing_int] -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  }
  // printf("CC tree, adding flow, time %d, flow %f, path id %d, depart time %d \n", (int) timestamp(), (float) flow(), (int) path -> m_path_ID, (int) departing_int);
  m_record[path][departing_int] -> add_increment(std::make_pair(timestamp, flow));
  return 0;
}


int MNM_Tree_Cumulative_Curve::print_out()
{
  for (auto outer_it : m_record){
    printf("For path id %d\n", outer_it.first -> m_path_ID());
    for (auto inner_it : outer_it.second){
      printf("For deparint time %d\n", inner_it.first());
      std::cout << inner_it.second -> to_string() << std::endl;
    }
  }
  return 0;
}