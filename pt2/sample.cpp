#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <functional>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>

struct Hobbit {
  std::string name;
  int hp, off, def;

  friend bool operator == (const Hobbit&, const Hobbit&) = default;
};

std::ostream& operator << (std::ostream& out, const Hobbit& h) {
  return out
    << "Hobbit{\"" << h.name << "\", "
    << ".hp=" << h.hp << ", "
    << ".off=" << h.off << ", "
    << ".def=" << h.def << "}";
}

template < typename T >
std::ostream& operator << (std::ostream& out, const std::optional<T>& x) {
  if (!x) return out << "EMPTY_OPTIONAL";
  return out << "Optional{" << *x << "}";
}

#endif


struct HobbitArmy {
  static constexpr bool CHECK_NEGATIVE_HP = false;

private:
  struct Node {
    Hobbit hobbit;
    Node *left = nullptr, *right = nullptr;
    int height = 1;

    Node(const Hobbit &hobbit) : hobbit(hobbit) {}
    const std::string& getName() const {return hobbit.name; }
  };

  Node *root = nullptr;
public:

  HobbitArmy() = default;
  ~HobbitArmy() {
    deleteTree(root);
  }
  HobbitArmy(const HobbitArmy&) = delete;
  HobbitArmy& operator=(const HobbitArmy&) = delete;

  bool add(const Hobbit& hobbit) {
    if (hobbit.hp <= 0) return false;
    bool flag = false;
    root = add_impl(root, hobbit, flag);
    return flag;
  }

  std::optional<Hobbit> erase(const std::string& hobbit_name) {
    std::optional<Hobbit> erased = std::nullopt;
    root = erase_impl(root, hobbit_name, erased);
    return erased;
  }

  std::optional<Hobbit> stats(const std::string& hobbit_name) const {
    const Node* node = find(root, hobbit_name);
    if (node) return node->hobbit;
    return std::nullopt;
  }

  bool enchant(
    const std::string& first,
    const std::string& last,
    int hp_diff,
    int off_diff,
    int def_diff
  ) {
    if (first > last) return true;
    enchant_impl(root, first, last, hp_diff, off_diff, def_diff);
    return true;
  }

  void for_each(auto&& fun) const {
    for_each_impl(root, fun);
  }

  private:
  static void for_each_impl(Node *node, auto& fun) {
    if (!node) return;
    for_each_impl(node->left, fun);
    fun(node->hobbit);
    for_each_impl(node->right, fun);
  }

  void deleteTree(Node *node) {
    if (node) {
      deleteTree(node->left);
      deleteTree(node->right);
      delete node;
    }
  }

  int getHeight(Node *n) const {return n ? n->height : 0 ;}
  void updateHeight(Node *n) {
    if (n) n->height = 1 + std::max(getHeight(n->left), getHeight(n->right));
  }
  int getBalance(Node *n) const {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
  }

  Node* rRotate(Node* y) {
    Node* x = y->left;
    Node *t2 = x->right;
    x->right = y;
    y-> left = t2;
    updateHeight(y);
    updateHeight(x);
    return x;
  }

  Node* lRotate(Node* x) {
    Node* y = x->right;
    Node *t2 = y->left;
    y->left = x;
    x->right = t2;
    updateHeight(x);
    updateHeight(y);
    return y;
  }

  Node* rebalance(Node *n) {
    updateHeight(n);
    int balance = getBalance(n);
    if (balance > 1) {
      if (getBalance(n->left) < 0) {
        n->left = lRotate(n->left);
      }
      return rRotate(n);
    }
    if (balance < -1) {
      if (getBalance(n->right) > 0) {
        n->right = rRotate(n->right);
      }
      return lRotate(n);
    }
    return n;
  }

  Node* findMin(Node* n) {
    if (n->left == nullptr)
      return n;
    return findMin(n->left);
  }

  Node* add_impl(Node* node, const Hobbit& hobbit, bool& success) {
    if (!node) {
      success = true;
      return new Node(hobbit);
    }
    if (hobbit.name < node->getName()) {
      node->left = add_impl(node->left, hobbit, success);
    } else if (hobbit.name > node->getName()) {
      node->right = add_impl(node->right, hobbit, success);
    } else {
      success = false;
      return node;
    }
    if (!success) return node;
    return rebalance(node);
  }

  Node* erase_impl(Node* node, const std::string& name, std::optional<Hobbit>& erased) {
    if (!node) return nullptr;
    if (name < node->getName()) {
      node->left = erase_impl(node->left, name, erased);
    }
    else if (name > node->getName()) {
      node->right = erase_impl(node->right, name, erased);
    }  else {
      if (!erased.has_value()) {
        erased = node->hobbit;
      }
      if (!node->left) {
        Node* temp = node->right;
        delete node;
        return temp;
      } if (!node->right) {
        Node* temp = node->left;
        delete node;
        return temp;
      }
      Node* successor = findMin(node->right);
      node->hobbit = successor->hobbit;
      std::optional<Hobbit> dummy;
      node->right = erase_impl(node->right, successor->getName(), dummy);
    }
    return rebalance(node);
  }

  const Node* find(const Node *node, const std::string &name) const {
    if (!node) return nullptr;
    if (name < node->getName())
      return find(node->left, name);
    if (name > node->getName())
      return find(node->right, name);
    return node;
  }

  void enchant_impl(Node* node, const std::string& first, const std::string& last, int hp, int off, int def) {
    if (!node) return;
    const std::string& name = node->getName();
    if (name >= first && name <= last) {
      node->hobbit.hp += hp;
      node->hobbit.off += off;
      node->hobbit.def += def;
    }
    if (name > first) {
      enchant_impl(node->left, first, last, hp, off, def);
    }
    if (name < last) {
      enchant_impl(node->right, first, last, hp, off, def);
    }
  }
};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
  std::string ret = "\"";
  for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
  return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

 
////////////////// End of dark magic ////////////////////////


void check_army(const HobbitArmy& A, const std::vector<Hobbit>& ref, int& ok, int& fail) {
  size_t i = 0;

  A.for_each([&](const Hobbit& h) {
    CHECK(i < ref.size(), true);
    CHECK(h, ref[i]);
    i++;
  });

  CHECK(i, ref.size());
}

void test1(int& ok, int& fail) {
  HobbitArmy A;
  check_army(A, {}, ok, fail);

  CHECK(A.add({"Frodo", 100, 10, 3}), true);
  CHECK(A.add({"Frodo", 200, 10, 3}), false);
  CHECK(A.erase("Frodo"), std::optional(Hobbit("Frodo", 100, 10, 3)));
  CHECK(A.add({"Frodo", 200, 10, 3}), true);

  CHECK(A.add({"Sam", 80, 10, 4}), true);
  CHECK(A.add({"Pippin", 60, 12, 2}), true);
  CHECK(A.add({"Merry", 60, 15, -3}), true);
  CHECK(A.add({"Smeagol", 0, 100, 100}), false);

  if constexpr(HobbitArmy::CHECK_NEGATIVE_HP)
    CHECK(A.add({"Smeagol", -100, 100, 100}), false);

  CHECK(A.add({"Smeagol", 200, 100, 100}), true);

  CHECK(A.enchant("Frodo", "Frodo", 10, 1, 1), true);
  CHECK(A.enchant("Sam", "Frodo", -1000, 1, 1), true); // empty range
  CHECK(A.enchant("Bilbo", "Bungo", 1000, 0, 0), true); // empty range
  
  if constexpr(HobbitArmy::CHECK_NEGATIVE_HP)
    CHECK(A.enchant("Frodo", "Sam", -60, 1, 1), false);

  CHECK(A.enchant("Frodo", "Sam", 1, 0, 0), true);
  CHECK(A.enchant("Frodo", "Sam", -60, 1, 1), true);

  CHECK(A.stats("Gandalf"), std::optional<Hobbit>{});
  CHECK(A.stats("Frodo"), std::optional(Hobbit("Frodo", 151, 12, 5)));
  CHECK(A.stats("Merry"), std::optional(Hobbit("Merry", 1, 16, -2)));

  check_army(A, {
    {"Frodo", 151, 12, 5},
    {"Merry", 1, 16, -2},
    {"Pippin", 1, 13, 3},
    {"Sam", 21, 11, 5},
    {"Smeagol", 200, 100, 100},
  }, ok, fail);
}

int main() {
  int ok = 0, fail = 0;
  test1(ok, fail);

  if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif


