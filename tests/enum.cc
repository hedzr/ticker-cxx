//
// Created by Hedzr Yeh on 2023/6/30.
//

#include <iostream>

//#include "ticker_cxx/ticker-def.hh"

namespace {

  namespace A {
    auto a1 = 1.0f;
    namespace B {
      auto a2 = 2;
    } // namespace B
  } // namespace A

  namespace A::B {
    struct C {
      bool a3;
    };
  } // namespace A::B

  void test_namespace_a() {
    A::B::C const c{true};
    std::cout << "A::B::C::a3: " << std::boolalpha << c.a3 << '\n';
    std::cout << "A::B::a2: " << A::B::a2 << '\n';
    std::cout << "A::a1: " << A::a1 << '\n';
  }

  namespace {
    enum class Suit {
      spades,
      hearts,
      diamonds,
      clubs
    };

    template<typename T>
    struct safer {
      T *&_ref;

      safer(T *&t) : _ref(t) {}

      ~safer() {
        if (_ref) delete _ref;
      }

      void reset() { _ref = nullptr; }
    };

    void run1() {
      // alignas(T) unsigned char buf[sizeof(T)];
      // T* tptr = new(buf) T;
      // tptr->~T();

      struct alignas(4) Vec1 {
        char x;
        int i;
      };

#pragma pack(push, 1)
      struct Vec2 {
        char x;
        int i;
      };
#pragma pack(pop)

      struct alignas(128) Vec3 {
        float x, y, z;
      };

      auto *ptr1 = new Vec1;
      safer const _1(ptr1);
      auto *ptr2 = new Vec1;
      safer const _2(ptr2);

      auto *ptr4 = new Vec3[1];

      auto *ptr3 = new Vec1;
      safer const _3(ptr3);

      auto *ptr5 = new Vec3[1];
      auto *ptr6 = new Vec3[3];

      std::cout << "ptr1: " << ptr1 << ", .i = " << &ptr1->i << '\n';
      std::cout << "ptr2: " << ptr2 << ", .i = " << &ptr2->i << '\n';
      std::cout << "ptr3: " << ptr3 << ", .i = " << &ptr3->i << '\n';

      std::cout << "ptr4: " << ptr4 << ", .y = " << &ptr4->y << '\n';
      std::cout << "ptr5: " << ptr5 << ", .y = " << &ptr5->y << '\n';
      std::cout << "ptr6: " << ptr6 << ", .y = " << &ptr6->y << '\n';
      delete[] ptr4;
      delete[] ptr5;
      delete[] ptr6;

      auto *ptr7 = new Vec2;
      safer const _7(ptr7);
      auto *ptr8 = new Vec2;
      safer const _8(ptr8);
      std::cout << "ptr7: " << ptr7 << ", .i = " << &ptr7->i << '\n';
      std::cout << "ptr8: " << ptr8 << ", .i = " << &ptr8->i << '\n';

      // auto ptr2 = (Vec3*)::operator new [](sizeof(Vec3)*10+overhead, std::align_val_t(alignof(Vec3)));
      // safer _2(ptr2);
      // std::cout << "ptr2: " << ptr2 << '\n';
    }
  } // namespace

} // namespace

int main() {
  std::cout << "Hello, World!" << '\n';
  run1();
  test_namespace_a();
}