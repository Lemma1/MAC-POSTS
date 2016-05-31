#include "factory.h"

/**************************************************************************
                          Vehicle node
**************************************************************************/
MNM_Veh_Factory::MNM_Veh_Factory()
{
  m_veh_map = std::map<TInt, MNM_Veh*>();
  m_num_veh = TInt(0);
}

MNM_Veh_Factory::~MNM_Veh_Factory()
{

}

MNM_Veh *MNM_Veh_Factory::make_veh(TInt timestamp)
{
  MNM_Veh *__veh = new MNM_Veh(m_num_veh + 1, timestamp);
  m_veh_map.insert(std::pair<TInt, MNM_Veh*>(m_num_veh + 1, __veh));
  m_num_veh += 1;
  return __veh;
}


/**************************************************************************
                          Node factory
**************************************************************************/

MNM_Node_Factory::MNM_Node_Factory()
{
  m_node_map = std::map<TInt, MNM_Dnode*>();
}

MNM_Dnode *MNM_Node_Factory::make_node(TInt ID, TFlt flow_scalar, DNode_type node_type)
{
  MNM_Dnode *__node;
  switch (node_type){
    case MNM_TYPE_FWJ:
      __node = new MNM_Dnode_FWJ(ID, flow_scalar);
      break;
    case MNM_TYPE_ORIGIN:
      __node = new MNM_DMOND(ID, flow_scalar);
      break;
    case MNM_TYPE_DEST:
      __node = new MNM_DMDND(ID, flow_scalar);
      break;
    default:
      printf("Wrong node type.\n");
      exit(-1);
  }
  m_node_map.insert(std::pair<TInt, MNM_Dnode*>(ID, __node));
  return __node;
}



/**************************************************************************
                          Link factory
**************************************************************************/
