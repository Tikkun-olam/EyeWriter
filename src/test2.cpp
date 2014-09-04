extern "C"{
  #include <xdo.h>
}
//extern "C" xdo_window_search
#include <iostream>
#include <string.h>

using namespace std;

int main() {
  xdo_t *xdo = xdo_new(NULL);
  xdo_keysequence(xdo, CURRENTWINDOW, "A", 0);
  return  0;
}