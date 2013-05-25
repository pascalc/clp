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
  // Live cell coordinates
  IntVarArray live_x;
  IntVarArray live_y;

  // World
  IntVarArray world;
  int size;

  // Helper functions

  int live(Matrix<IntVarArray> w, int x, int y) {
    return w(x, y).val() == 1;
  }

  int dead(Matrix<IntVarArray> w, int x, int y) {
    return w(x, y).val() == 0;
  }

  IntVarArgs neighbours(Matrix<IntVarArray> w, int x, int y) {
    IntVarArgs results;
    for(int i = 0; i < 8; i++) {
      int x_p = x + DELTAS[i][0];
      int y_p = y + DELTAS[i][1];

      if (x_p >= 0 && x_p < size &&
          y_p >= 0 && y_p < size) {
        results << w(x_p, y_p);
      }
    }
    return results;
  }

  int neighbourSum(Matrix<IntVarArray> w, int x, int y) {
    IntVarArgs neighbourList = neighbours(w, x, y);
    int sum = 0;
    for(int i = 0; i < neighbourList.size(); i++) {
      sum += neighbourList[i].val();
    }
    return sum;
  }

  int nextState(Matrix<IntVarArray> w, int x, int y) {
    // Rule 1: Death from boredom
    if (live(w, x, y) && neighbourSum(w, x, y) < 2) {
      return 0;
    } 
    // Rule 2: Death by overcrowding
    else if (live(w, x, y) && neighbourSum(w, x, y) > 3) {
      return 0;
    }
    // Rule 3: Survival
    else if (live(w, x, y) && 
             (neighbourSum(w, x, y) == 2 || neighbourSum(w, x, y) == 3)) {
      return 1;
    }
    // Rule 4: Birth
    else if (dead(w, x, y) && neighbourSum(w, x, y) == 3) {
      return 1;
    }
  }

  int isStillLife(Matrix<IntVarArray> w) {
    int stationary = 0;
    for(int i = 0; i < live_x.size(); i++) {
      int x = live_x[i];
      int y = live_y[i];
      if (w(x, y) == nextState(w(x, y))) {
        stationary++;
      }
    }
    return stationary == live_x.size();
  }

  // Mark live cells in the World matrix for easier computation
  // of neighbour sums and liveness
  Matrix<IntVarArray> populate() {
    world = IntVarArray(*this, size*size, 0, 1);
    Matrix<IntVarArray> w(world, size, size);

    for(int i = 0; i < live_x.size(); i++) {
      int x = live_x[i].val();
      int y = live_y[i].val();

      rel(*this, w(x, y), IRT_EQ, 1);
    }

    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
        IntVar v = w(x, y);
        if (v.size() > 1) {
          rel(*this, v, IRT_EQ, 0);
        }
      }
    }

    return w;
  }

  StillLife(int n, int m)
    : live_x(*this, m, 0, n-1), live_y(*this, m, 0, n-1), world(*this, n*n, 0, 1), size(n) {

    Matrix<IntVarArray> w = populate();

    branch(*this, live_x, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, live_y, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  StillLife(bool share, StillLife& s) : Space(share, s) {
    live_x.update(*this, share, s.live_x);
    live_y.update(*this, share, s.live_y);
    // Compute the new world
    delete world;
    populate();
  }

  virtual Space* copy(bool share) {
    return new StillLife(share,*this);
  }

  /// Print solution
  void print() const {
    Matrix<IntVarArray> w(world, size, size);
    std::cout << "world " << "("<< size << "x" << size << ")\n";
    std::cout << w << "\n";
  }
};

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  StillLife* life = new StillLife(n, m);

  DFS<StillLife> e(life);
  delete life;
  if (StillLife* s = e.next()) {
    s->print(); 
    delete s;
  }
  
  return 0;
}
