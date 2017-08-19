#ifndef EMISSION_H
#define EMISSION_H

#include "dlink.h"

#include <vector>

class MNM_Cumulative_Emission
{
public:
  MNM_Cumulative_Emission(TFlt unit_time, TInt freq);
  ~MNM_Cumulative_Emission();
  std::vector<MNM_Dlink*> m_link_vector;
  int register_link(MNM_Dlink *link);
  // v should be in mile/hour
  TFlt calculate_fuel_rate(TFlt v);
  TFlt calculate_fuel_rate_deprecated(TFlt v);
  TFlt calculate_CO2_rate(TFlt v);
  TFlt calculate_CO2_rate_deprecated(TFlt v);
  TFlt calculate_HC_rate(TFlt v);
  TFlt calculate_CO_rate(TFlt v);
  TFlt calculate_NOX_rate(TFlt v);

  int update();
  int output();

  TFlt m_fuel;
  TFlt m_CO2;
  TFlt m_HC;
  TFlt m_CO;
  TFlt m_NOX;
  TFlt m_unit_time;
  TInt m_freq;
  TInt m_counter;
  TFlt m_VMT;
};


#endif