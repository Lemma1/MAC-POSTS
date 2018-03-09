/*cppimport

*/
#include <pybind11/pybind11.h>
// #include "../snap-core/Snap.h"

int test()
{
  TInt a = 3;
  printf("Integer is %d\n", a);
  return 0;
}

PYBIND11_PLUGIN(MNMAPI) {
    pybind11::module m("MNMAPI", "auto-compiled c++ extension");
    m.def("test", &(test));
    return m.ptr();
}