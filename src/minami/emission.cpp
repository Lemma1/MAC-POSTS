#include "emission.h"

#include "ults.h"

#include <math.h> 

MNM_Cumulative_Emission::MNM_Cumulative_Emission(TFlt unit_time, TInt freq)
{
  m_unit_time = unit_time;
  m_freq = freq;
  m_fuel = TFlt(0);
  m_CO2 = TFlt(0);
  m_HC = TFlt(0);
  m_CO = TFlt(0);
  m_NOX = TFlt(0);
  m_counter = 0;
  m_link_vector = std::vector<MNM_Dlink*>();
}

MNM_Cumulative_Emission::~MNM_Cumulative_Emission()
{
  m_link_vector.clear();
}


int MNM_Cumulative_Emission::register_link(MNM_Dlink *link){
  m_link_vector.push_back(link);
  return 0;
}

TFlt MNM_Cumulative_Emission:: calculate_fuel_rate(TFlt v)
{
  TFlt _fule_eco = -1.47718733159777 * 1e-13 * pow(v, 10) 
                  + 6.8247176456893 * 1e-11 * pow(v, 9)
                  - 1.38867659079602 * 1e-8 * pow(v, 8) 
                  + 1.63502795565324 * 1e-6 * pow(v, 7)
                  - 0.000123021415999858 * pow(v,6) + 0.00616148672549657 * pow(v,5)
                  - 0.207343000174098 * pow(v,4) + 4.61456181374348 * pow(v,3)
                  - 64.856033832593 * pow(v,2) + 519.768517199759 * v - 1780.82307640254;
  // printf("fuel rate is %lf\n", 1.0/_fule_eco());
  return MNM_Ults::max(TFlt(1)/_fule_eco, TFlt(0));
}

TFlt MNM_Cumulative_Emission::calculate_CO2_rate(TFlt v)
{
  TFlt _fuel_rate = calculate_fuel_rate(v);
  // printf("CO2 rate is %lf\n", _fuel_rate * TFlt(8875)());
  return MNM_Ults::max(_fuel_rate * TFlt(8875), TFlt(0));
}

TFlt MNM_Cumulative_Emission::calculate_HC_rate(TFlt v)
{
  TFlt _HC_rate = 1.61479076909784 * 1e-13 * pow(v,8) - 1.27884474982285 * 1e-10 * pow(v,7)
                + 2.92924270300974 * 1e-8 * pow(v,6) - 3.23670086149171 * 1e-6 * pow(v,5)
                + 0.000201135990745703 * pow(v,4) - 0.00737871178398462 * pow(v,3)
                + 0.15792241257931 * pow(v,2) - 1.82687242201925 * v + 9.84559996919605;
  // printf("HC rate is %lf\n", _HC_rate());
  return MNM_Ults::max(_HC_rate, TFlt(0));
}

TFlt MNM_Cumulative_Emission::calculate_CO_rate(TFlt v)
{
  TFlt _CO_rate = -1.08317411174986 * 1e-12 * pow(v,8) + 2.53340626614398 * 1e-10 * pow(v,7)
            -2.12944112670644 * 1e-8* pow(v,6) + 5.97070024385679 * 1e-7 * pow(v,5)
            +1.79281854904105 * 1e-5 * pow(v,4) - 0.00170366500109581 * pow(v,3)
            +0.047711166912908 * pow(v,2) - 0.615061016205463 * v + 4.12900319568868;
  // printf("CO rate is %lf\n", _CO_rate());
  return MNM_Ults::max(_CO_rate, TFlt(0));
}

TFlt MNM_Cumulative_Emission::calculate_NOX_rate(TFlt v)
{
  TFlt _NOX_rate = -6.52009367269462 * 1e-13 * pow(v,8) + 1.25335312366681 * 1e-10 * pow(v,7)
        -4.67202313364846 * 1e-9 * pow(v,6) - 6.63892272105462 * 1e-7 * pow(v,5)
        +8.01942113220463 * 1e-5 * pow(v,4) - 0.00374632777368871 * pow(v,3)
        +0.0895029037098895 * pow(v,2) - 1.07265851515536 * v + 6.06514023873933;
  // printf("NOX rate is %lf\n", _NOX_rate());
  return MNM_Ults::max(_NOX_rate, TFlt(0));
}

int MNM_Cumulative_Emission::update()
{
  m_counter += 1;
  // printf("ce counter is now %d\n", m_counter());
  TFlt _v;
  TFlt _v_converted;
  for (MNM_Dlink *link : m_link_vector){
    _v = link -> m_length / link -> get_link_tt(); // m/s
    _v_converted = _v * TFlt(3600) / TFlt(1600); // mile / hour
    m_fuel += calculate_fuel_rate(_v_converted) 
              * (_v * m_unit_time / TFlt(1600))  
              * link -> get_link_flow();
    m_CO2 += calculate_CO2_rate(_v_converted) 
              * (_v * m_unit_time / TFlt(1600))  
              * link -> get_link_flow();
    m_HC += calculate_HC_rate(_v_converted) 
              * (_v * m_unit_time / TFlt(1600))  
              * link -> get_link_flow();
    m_CO += calculate_CO_rate(_v_converted) 
              * (_v * m_unit_time / TFlt(1600))  
              * link -> get_link_flow();
    m_NOX += calculate_NOX_rate(_v_converted) 
              * (_v * m_unit_time / TFlt(1600))  
              * link -> get_link_flow();              
  }
  // printf("m_counter is %d and freq is %d\n", m_counter(), m_freq());
  if (m_counter == m_freq){
    // printf("CE update: output\n");
    output();
    m_fuel = TFlt(0);
    m_CO2 = TFlt(0);
    m_HC = TFlt(0);
    m_CO = TFlt(0);
    m_NOX = TFlt(0);
    m_counter = 0;
  }
  return 0;
}

int MNM_Cumulative_Emission::output()
{
  printf("The emission stats are: ");
  printf("%lf, %lf, %lf, %lf, %lf\n", m_fuel(), m_CO2(), m_HC(), m_CO(), m_NOX());
  return 0;
}




