#include "factory.h"

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
  MNM_Veh *__veh = new MNM_Veh(m_num_veh + 1);
  m_veh_map.insert(std::pair<TInt, MNM_Veh*>(m_num_veh + 1, __veh));
  m_num_veh += 1;
  return __veh;
}