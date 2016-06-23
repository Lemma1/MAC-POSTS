#include "marginal_cost.h"

MNM_Link_Tt::MNM_Link_Tt(MNM_Link_Factory *link_facotory, TFlt unit_time)
{
  m_link_factory = link_facotory;
  m_unit_time = unit_time;
  m_update_speed = 0.5;
  m_tracker = std::map<MNM_Veh*, Vehicle_Record*>();
  m_tt_map =std::map<MNM_Dlink*, TFlt>();
  intialize();
}

MNM_Link_Tt::~MNM_Link_Tt()
{
  for (auto _it : m_tracker){
    delete _it.second;
  }
  m_tracker.clear();
  m_tt_map.clear();
}

int MNM_Link_Tt::intialize()
{
  MNM_Dlink *_link;
  TFlt _fft;
  for (auto _link_it : m_link_factory -> m_link_map){
    _link = _link_it.second;
    _fft = _link -> m_length / _link -> m_ffs;
    m_tt_map.insert(std::pair<MNM_Dlink*, TFlt>(_link, _fft));
  }
  return 0;
}

int MNM_Link_Tt::update_tt(TInt timestamp)
{
  MNM_Dlink *_link;
  Vehicle_Record *_record;
  TFlt _duration;
  for (auto _link_it : m_link_factory -> m_link_map){
    _link = _link_it.second;
    for (MNM_Veh *_veh : _link -> m_incoming_array){
      if (m_tracker.find(_veh) == m_tracker.end()){
        register_vehicle(_veh, timestamp);
      }
      else {
        _record = m_tracker.find(_veh) -> second;
        _duration = timestamp - _record -> m_start_time;
        if (_veh -> m_finish_time > 0){
          update_indiviual_tt(_record -> m_link, _duration * m_unit_time);
          delete _record;
          m_tracker.erase(m_tracker.find(_veh));
        }
        else if (_record -> m_link != _veh -> get_current_link()){
          update_indiviual_tt(_record -> m_link, _duration * m_unit_time);
          change_link(_veh, _veh -> get_current_link(), timestamp);
        }
        else{
          printf("update_marginal_cost::Impossible case!\n");
          exit(-1);
        }
      } 
    }
  }
  return 0;
}

int MNM_Link_Tt::register_vehicle(MNM_Veh* veh, TInt current_time)
{
  Vehicle_Record *_record = new Vehicle_Record{veh -> get_current_link(), TFlt(current_time)};
  m_tracker.insert(std::pair<MNM_Veh*, Vehicle_Record*>(veh, _record));
  return 0;
}

int MNM_Link_Tt::change_link(MNM_Veh* veh, MNM_Dlink *link, TInt current_time)
{
  Vehicle_Record *_record = m_tracker.find(veh) -> second;
  _record -> m_link = link;
  _record -> m_start_time = TFlt(current_time);
  return 0;
}

int MNM_Link_Tt::update_indiviual_tt(MNM_Dlink *link, TFlt tt)
{
  auto _it = m_tt_map.find(link);
  _it -> second = _it -> second * (1 - m_update_speed) + m_update_speed * tt;
  return 0;
}

void MNM_Link_Tt::print_info()
{
  MNM_Dlink *_link;
  TFlt _tt;
  for (auto _link_it : m_tt_map){
    _link = _link_it.first;
    _tt = _link_it.second;
    printf("In link ID %d, the travel time is %.4f.\n", (int) _link -> m_link_ID, (float)_tt);
  }  
}




namespace MNM{

TFlt calculate_link_mc(MNM_Dlink *link, TFlt link_tt)
{
  TFlt _fft = MNM_Ults::divide(link -> m_length, link -> m_ffs);
  if (link_tt < _fft) {
    return _fft;
  }
  else{
    return _fft + link_tt;
  }
}

}//end namespace MNM