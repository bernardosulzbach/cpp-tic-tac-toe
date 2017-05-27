#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

#define debug(x) cout << #x " is " << x << endl

// Generic code

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/**
 * Prints the values separated by spaces.
 */
template <typename It, typename T = typename iterator_traits<It>::value_type>
void print(It first, It last) {
  copy(first, last, ostream_iterator<T>(cout, " "));
  cout << '\n';
}

/**
 * Prints the values separated by newlines.
 */
template <typename It, typename T = typename iterator_traits<It>::value_type>
void print_lines(It first, It last) {
  copy(first, last, ostream_iterator<T>(cout, "\n"));
}

/**
 * Ignores everything on std::cin until the next '\n'.
 */
void cin_ignore_until_newline(void) {
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

template <typename K, typename V>
struct PairHash {
  size_t operator()(const pair<K, V> &p) const {
    auto h1 = hash<K>{}(p.first);
    auto h2 = hash<V>{}(p.second);
    return h1 ^ (h2 << 1);
  }
};

class TicTacToe {
  typedef uint8_t PlayerType;
  typedef uint32_t BoardType;

  const static PlayerType NONE = 0;
  const static PlayerType X = 1;
  const static PlayerType O = 2;

  BoardType board;

  uint32_t get_shift(size_t s) const { return 2 * (8 - s); }

 public:
  TicTacToe() : board(0) {}

  TicTacToe(string s) {
    for (int i = 0; i < 9; i++) {
      PlayerType v = player(s[i]);
      set(i, v);
    }
  }

  char symbol(PlayerType p) const {
    if (p == O) {
      return 'O';
    } else if (p == X) {
      return 'X';
    }
    return '_';
  }

  PlayerType player(char c) const {
    if (c == 'O') {
      return O;
    } else if (c == 'X') {
      return X;
    }
    return NONE;
  }

  PlayerType winner() const {
    for (int i = 0; i < 3; i++) {
      int h_base = 3 * i;
      if (get(h_base)) {
        if (get(h_base) == get(h_base + 1) &&
            get(h_base + 1) == get(h_base + 2)) {
          return get(h_base);
        }
      }
      int v_base = i;
      if (get(v_base)) {
        if (get(v_base) == get(v_base + 3) &&
            get(v_base + 3) == get(v_base + 6)) {
          return get(v_base);
        }
      }
    }
    if (get(0)) {
      if (get(0) == get(4) && get(4) == get(8)) {
        return get(0);
      }
    }
    if (get(2)) {
      if (get(2) == get(4) && get(4) == get(6)) {
        return get(2);
      }
    }
    return 0;
  }

  PlayerType get_player_to_move() const {
    if (enumerate_possibilities().size() % 2 == 1) {
      return X;
    } else {
      return O;
    }
  }

  bool winnable() {
    const PlayerType to_move = get_player_to_move();
    bool won = false;
    for (size_t move : enumerate_possibilities()) {
      set(move, to_move);
      if (winner() == to_move) {
        won = true;
      }
      unset(move);
    }
    return won;
  }

  // Returns in [0, 1], the higher the better for the player to move.
  double evaluate() {
    // Can win with the next move?
    if (winnable()) {
      return 1.0;
    }
    const PlayerType to_move = get_player_to_move();
    double best = 0.0;
    for (size_t move : enumerate_possibilities()) {
      set(move, to_move);
      double evaluation = 1.0 - evaluate();
      best = max(best, evaluation);
      unset(move);
    }
    // Decay the value of deeper games as these will take longer.
    const double decay = 0.9;
    return decay * best;
  }

  // Enumerates all empty positions in the board.
  vector<size_t> enumerate_possibilities() const {
    vector<size_t> possibilities;
    possibilities.reserve(9);
    for (size_t i = 0; i < 9; i++) {
      if (get(i) == NONE) {
        possibilities.push_back(i);
      }
    }
    return possibilities;
  }

  // Makes the best possible play for the player to move.
  size_t get_best_play() {
    // Worst case: your opponent will win.
    double best_so_far = 1.0;
    size_t best_move = enumerate_possibilities()[0];
    for (size_t move : enumerate_possibilities()) {
      set(move, get_player_to_move());
      double evaluation = evaluate();
      if (evaluation < best_so_far) {
        best_so_far = evaluation;
        best_move = move;
      }
      unset(move);
    }
    return best_move;
  }

  PlayerType get(size_t i) const { return (board >> get_shift(i)) & 3; }

  void set(size_t i, PlayerType v) {
    // Zero the position.
    unset(i);
    // Set to the player code.
    BoardType value = v & 3;
    board |= value << get_shift(i);
  }

  void unset(size_t i) { board &= ~(3 << get_shift(i)); }
};

ostream &operator<<(ostream &os, const TicTacToe &game) {
  for (int i = 0; i < 9; i++) {
    os << game.symbol(game.get(i));
    if (i % 3 == 2) {
      os << '\n';
    }
  }
  return os;
}

istream &operator>>(istream &is, TicTacToe &game) {
  for (int i = 0; i < 9; i++) {
    char value;
    is >> value;
    game.set(i, game.player(value));
  }
  return is;
}

typedef std::chrono::microseconds microseconds;
using chrono::duration_cast;

int main(void) {
  ios_base::sync_with_stdio(false);
  TicTacToe game;
  size_t r;
  size_t c;
  cout << game;
  while (!game.winnable()) {
    cin >> r >> c;
    game.set(3 * r + c, game.player('X'));
    cout << "After you:" << '\n' << game;
    const auto before = std::chrono::high_resolution_clock::now();
    size_t best = game.get_best_play();
    r = best / 3;
    c = best % 3;
    game.set(3 * r + c, game.player('O'));
    const auto after = std::chrono::high_resolution_clock::now();
    const auto delta = after - before;
    cout << "Took " << duration_cast<microseconds>(delta).count() << " μs.\n";
    cout << "After the algorithm:" << '\n' << game;
  }
  return 0;
}
