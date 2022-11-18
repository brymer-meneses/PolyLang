
#include "PolyLang.hpp"

int main(const int argc, const char *argv[]) {

  PolyLang polyLang = PolyLang(argc, argv);
  polyLang.run();
  return 0;
}
