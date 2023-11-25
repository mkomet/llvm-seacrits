#include <cstdio>
#include <exception>
#include <string>
#include <unwind.h>

// namespace std {
// void terminate() noexcept { abort(); }
// }; // namespace std

class base_class {
public:
  base_class() = default;
  virtual ~base_class() = default;

  virtual void print() const { std::printf("base_class\n"); }
};

class derived_class : public base_class {
public:
  derived_class() = default;
  virtual ~derived_class() = default;

  virtual void print() const override { std::printf("derived_class\n"); }
};

int main() {
  try {
    std::string secret = "lolz";

    if (secret[0] == 'l')
      throw derived_class();
  } catch (const base_class &base) {
    std::printf("caught base_class\n");
    base.print();
  }

  return 0;
}
