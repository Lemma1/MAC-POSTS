#include "dlink.h"

int main()
{
  MNM_Cumulative_Curve c = MNM_Cumulative_Curve();
  c.add_record(std::pair<TFlt, TFlt>(TFlt(0), TFlt(0)));
  for(int i= 1; i<100; ++i){
    // c.add_record(std::pair<TFlt, TFlt>(TFlt(i * 5), TFlt(i)));
    c.add_increment(std::pair<TFlt, TFlt>(TFlt(i * 5), TFlt(1)));
  }

  TFlt _time;
  for(int i=0; i< 102; ++i){
    _time = TFlt(i * 5) - 1;
    printf("For time %lf, the value is %lf\n", _time(), c.get_result(_time)());
  }

  return 0;
}