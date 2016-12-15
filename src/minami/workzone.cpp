#include "workzone.h"

#include <algorithm>

MNM_Workzone::MNM_Workzone(MNM_Node_Factory *node_factory, MNM_Link_Factory *link_factory, PNEGraph graph)
{
  m_node_factory = node_factory;
  m_link_factory = link_factory;
  m_graph = graph;
  m_disabled_link = std::vector<MNM_Dlink*>();
  m_workzone_list = std::vector<Link_Workzone>();
}

MNM_Workzone::~MNM_Workzone()
{
  m_disabled_link.clear();
  m_workzone_list.clear();
}

int MNM_Workzone::init_workzone()
{
  Link_Workzone _workzone;
  for (Link_Workzone _workzone : m_workzone_list){
    add_workzone_link(_workzone.m_link_ID);
  }
  return 0;
}


int MNM_Workzone::update_workzone(TInt timestamp)
{
  return 0;
}


int MNM_Workzone::add_workzone_link(TInt link_ID)
{
  MNM_Dlink *_link = m_link_factory -> get_link(link_ID);
  if (_link == NULL) {
    printf("No disabled link in the link factory\n");
    exit(-1);
  }
  MNM_Dnode *_from_node, *_to_node;
  _from_node = m_node_factory -> get_node(m_graph -> GetEI(link_ID).GetSrcNId());
  _to_node = m_node_factory -> get_node(m_graph -> GetEI(link_ID).GetDstNId());
  _from_node -> m_out_link_array.erase(std::remove(_from_node -> m_out_link_array.begin(), _from_node -> m_out_link_array.end(), _link));
  _to_node -> m_in_link_array.erase(std::remove(_to_node -> m_in_link_array.begin(), _to_node -> m_in_link_array.end(), _link));
  m_graph -> DelEdge(link_ID);
  m_link_factory -> delete_link(link_ID);
  m_disabled_link.push_back(_link);
  return 0;
}


int MNM_Workzone::delete_workzone_link(TInt link_ID)
{
  return 0;
}