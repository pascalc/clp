#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>

using namespace Gecode;

static int DELTAS[][2] = {  
    {-1, -1}, {0, -1}, {1, -1},
    {-1, 0},           {1, 0},
    {-1, 1},  {0, 1},  {1, 1} 
};

class StillLife : public Space {
public:
  int size;
  // Worlds
  BoolVarArray current;
  BoolVarArray next;

  // Helper functions
  BoolVarArgs neighbours(Matrix<BoolVarArray> current, int x, int y) {
    BoolVarArgs results;
    for(int i = 0; i < 8; i++) {
      int x_p = x + DELTAS[i][0];
      int y_p = y + DELTAS[i][1];

      if (x_p >= 0 && x_p < size &&
          y_p >= 0 && y_p < size) {
        results << current(x_p, y_p);
      }
    }
    return results;
  }

  // Constraints
  
  void rulesOfLifeConstraints(Matrix<BoolVarArray> current, Matrix<BoolVarArray> next, int x, int y) {
    BoolVarArgs neighbourList = neighbours(current, x, y);
    // Rule 1: sum(neighbours(x,y)) < 2 <-> next(x,y) = 0 (Boredom)
    linear(*this, neighbourList, IRT_GR, 2, Reify(next(x, y)));
    // Rule 2: sum(neighbours(x,y)) > 3 <-> next(x,y) = 0 (Overcrowding)
    linear(*this, neighbourList, IRT_LE, 3, Reify(next(x, y)));
    // Rule 3: sum(neighbours(x,y)) = 3 <-> next(x,y) = 1
    linear(*this, neighbourList, IRT_EQ, 3, Reify(next(x, y)));
    // Rule 4: sum(neighbours(x,y)) = 2 <-> next(x,y) = current(x,y)
    Reify r(BoolVar(*this, 0, 1));
    linear(*this, neighbourList, IRT_EQ, 2, r);
    rel(*this, current(x,y), IRT_EQ, next(x, y), r);
  }

  void stillLifeConstraint(Matrix<BoolVarArray> current, Matrix<BoolVarArray> next, int x, int y) {
    rel(*this, current(x,y), IRT_EQ, next(x, y)); 
  }

  // Setup
  StillLife(int n)
    : size(n), current(*this, n*n), next(*this, n*n) {

    Matrix<BoolVarArray> c(current, size, size);
    Matrix<BoolVarArray> nxt(next, size, size);

    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
        rulesOfLifeConstraints(c, nxt, x, y);
        stillLifeConstraint(c, nxt, x, y);
      }
    }

    branch(*this, current, INT_VAR_SIZE_MIN(), INT_VAL_MAX());
  }

  // Maximise density
  virtual void constrain(const Space& _b) {
    const StillLife& b = static_cast<const StillLife&>(_b);
    
    int currentDensity = 0;
    for (int i = 0; i < current.size(); i++) {
      currentDensity += current[i].val();
    }
    linear(*this, current, IRT_GR, currentDensity);
  }

  // Boilerplate

  StillLife(bool share, StillLife& s) : Space(share, s) {
    current.update(*this, share, s.current);
    next.update(*this, share, s.next);
    size = s.size;
  }

  virtual Space* copy(bool share) {
    return new StillLife(share,*this);
  }

  // Print solutions
  void print() const {
    Matrix<BoolVarArray> world(current, size, size);
    std::cout << "Still Life " << "("<< size << "x" << size << ")\n";
    std::cout << world << "\n";
  }
};

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);
  StillLife* life = new StillLife(n);

  DFS<StillLife> e(life);
  delete life;
  if (StillLife* s = e.next()) {
    s->print(); delete s;
  }
  return 0;
}
