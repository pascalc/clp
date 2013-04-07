#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

class Donald : public Space {
protected:
  IntVarArray vars;
public:
  Donald(void) : vars(*this, 10, 0, 9) {
    IntVar d(vars[0]), o(vars[1]), n(vars[2]), a(vars[3]), l(vars[4]),
           g(vars[5]), e(vars[6]), r(vars[7]),
           b(vars[8]), t(vars[9]);
    rel(*this, d != 0);
    rel(*this, g != 0);
    rel(*this, r != 0);
    distinct(*this, vars);
    rel(*this,
     100000*d + 10000*o + 1000*n + 100*a + 10*l + d
   + 100000*g + 10000*e + 1000*r + 100*a + 10*l + d
  == 100000*r + 10000*o + 1000*b + 100*e + 10*r + t);
    branch(*this, vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }
  Donald(bool share, Donald& s) : Space(share, s) {
    vars.update(*this, share, s.vars);
  }
  virtual Space* copy(bool share) {
    return new Donald(share,*this);
  }
  void print(void) const {
    std::cout << vars << std::endl;
  }
};

int main(int argc, char* argv[]) {
  Donald* m = new Donald;
  DFS<Donald> e(m);
  delete m;
  while (Donald* s = e.next()) {
    s->print(); delete s;
  }
  return 0;
}
