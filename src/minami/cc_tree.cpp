#include "dlink.h"

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
  if (m_record.find(path) == m_record.end()){
    m_record[path] = std::unordered_map<TInt, MNM_Cumulative_Curve*>();
  }
  if (m_record[path].find(departing_int) == m_record[path].end()){
    m_record[path][departing_int] = new MNM_Cumulative_Curve();
    m_record[path][departing_int] -> add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  }
  m_record[path][departing_int] -> add_increment(std::make_pair(timestamp, flow));
  return 0;
}