#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class Queens : public Script {
public:
  /// Chessboard squares
  IntVarArray squares;
  int size;

  /// The actual problem
  Queens(const SizeOptions& opt)
    : squares(*this,opt.size()*opt.size(),0,1), size(opt.size()) {
    const int n = opt.size();
    const int nn = squares.size();

    // Matrix interface
    Matrix<IntVarArray> m(squares, n, n);

    // Sum of each right-diagonal <= 1
    // Sum of each left-diagonal <= 1

    // One queen in each row
    // One queen in each column
    for (int i = 0; i < n; i++) {
      count(*this, m.row(i), 1, IRT_EQ, 1);
      count(*this, m.col(i), 1, IRT_EQ, 1);
    }

    // n queens placed <-- Redundant due to row and column constraints!
    // count(*this, squares, 1, IRT_EQ, n);

    branch(*this, squares, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  /// Constructor for cloning \a s
  Queens(bool share, Queens& s) : Script(share,s) {
    squares.update(*this, share, s.squares);
    size = s.size;
  }

  /// Perform copying during cloning
  virtual Space*
  copy(bool share) {
    return new Queens(share,*this);
  }

  /// Print solution
  virtual void
  print(std::ostream& os) const {
    Matrix<IntVarArray> m(squares, size, size);
    os << "queens " << "("<< size << "x" << size << ")\n";
    os << m << "\n";
  }
};

/** \brief Main-function
 *  \relates Queens
 */
int
main(int argc, char* argv[]) {
  SizeOptions opt("Queens");
  opt.iterations(500);
  opt.size(100);

  opt.parse(argc,argv);
  Script::run<Queens,DFS,SizeOptions>(opt);
  return 0;
}

// STATISTICS: example-any
