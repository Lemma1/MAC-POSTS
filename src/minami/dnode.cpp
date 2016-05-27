#include "dnode.h"


MNM_Dnode::MNM_Dnode(TInt ID)
{
  m_node_ID = ID;
  m_in_link_array = std::vector<MNM_Dlink*>();
  m_out_link_array = std::vector<MNM_Dlink*>();
}

MNM_Dnode::~MNM_Dnode()
{

}

MNM_DMOND::MNM_DMOND(TInt ID, MNM_Origin* origin)
  : MNM_Dnode::MNM_Dnode(TInt ID)
{
  m_origin = origin;
}

int MNM_DMOND::evolve(TInt timestamp)
{

}