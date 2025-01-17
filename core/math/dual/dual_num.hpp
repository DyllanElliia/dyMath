/*
 * @Author: DyllanElliia
 * @Date: 2022-07-01 15:37:04
 * @LastEditors: DyllanElliia
 * @LastEditTime: 2022-07-19 16:09:34
 * @Description:
 */
#pragma once

#include "../matrix.hpp"
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
namespace dym {
template <typename Type = Real> class Dual {
private:
  using thisType = Dual<Type>;

public:
  Dual(const Type &a, const Type &b) { A = a, B = b; }
  Dual(const std::initializer_list<Type> &v) {
    _DYM_ASSERT_(v.size() != 2,
                 "Dual ERROR: please input {A,B} for a + b /epsilon!");
    A = *(v.begin()), B = *(v.begin() + 1);
  }
  Dual(const thisType &d) { A = d.A, B = d.B; }
  Dual(const thisType &&d) { A = d.A, B = d.B; }
  Dual(const Type &vul) { A = vul, B = Type(0); }
  Dual() {}
  ~Dual() {}

  inline Dual operator=(const Dual &d) {
    A = d.A, B = d.B;
    return *this;
  }
  inline Dual operator=(const Type &vul) {
    A = vul, B = vul;
    return *this;
  }

  void show() const {
    std::cout << "Dual: [A: " << A << ", B: " << B << +"]" << std::endl;
  }

  friend std::ostream &operator<<(std::ostream &output, const Dual &d) {
    output << "Dual: [A: " << d.A << ", B: " << d.B << +"]";
    return output;
  }

  _DYM_FORCE_INLINE_ auto conjugate() const {
    return thisType{A, B * Type(-1)};
  }
  _DYM_FORCE_INLINE_ auto inverse() const {
    return thisType{Type(1) / A, B * Type(-1) / (A * A)};
  }

  thisType operator+(const thisType &rhs) const {
    return {A + rhs.A, B + rhs.B};
  }
  thisType operator-(const thisType &rhs) const {
    return {A - rhs.A, B - rhs.B};
  }
  thisType operator*(const thisType &rhs) const {
    return {A * rhs.A, A * rhs.B + B * rhs.A};
  }
  thisType operator/(const thisType &rhs) const {
    return (*this) * rhs.inverse();
  }

  thisType &operator+=(const thisType &rhs) {
    A = A + rhs.A, B = B + rhs.B;
    return *this;
  }
  thisType &operator-=(const thisType &rhs) {
    A = A - rhs.A, B = B - rhs.B;
    return *this;
  }
  thisType &operator*=(const thisType &rhs) {
    A = A * rhs.A, B = A * rhs.B + B * rhs.A;
    return *this;
  }
  thisType &operator/=(const thisType &rhs) {
    *this = *this * rhs.inverse();
    return *this;
  }

  friend thisType operator+(const Type &lhs, const thisType &rhs) {
    return {lhs + rhs.A, rhs.B};
  }
  friend thisType operator-(const Type &lhs, const thisType &rhs) {
    return {lhs - rhs.A, -rhs.B};
  }
  friend thisType operator*(const Type &lhs, const thisType &rhs) {
    return {lhs * rhs.A, lhs * rhs.B};
  }
  friend thisType operator/(const Type &lhs, const thisType &rhs) {
    return lhs * rhs.inverse();
  }
  friend thisType operator+(const thisType &lhs, const Type &rhs) {
    return {lhs.A + rhs, lhs.B};
  }
  friend thisType operator-(const thisType &lhs, const Type &rhs) {
    return {lhs.A - rhs, lhs.B};
  }
  friend thisType operator*(const thisType &lhs, const Type &rhs) {
    return {lhs.A * rhs, lhs.B * rhs};
  }
  friend thisType operator/(const thisType &lhs, const Type &rhs) {
    return {lhs.A / rhs, lhs.B / rhs};
  }

  thisType &operator+=(const Type &rhs) {
    A = A + rhs, B = B;
    return *this;
  }
  thisType &operator-=(const Type &rhs) {
    A = A - rhs, B = B;
    return *this;
  }
  thisType &operator*=(const Type &rhs) {
    A = A * rhs, B = B * rhs;
    return *this;
  }
  thisType &operator/=(const Type &rhs) {
    A = A / rhs, B = B / rhs;
    return *this;
  }

public:
  Type A, B;
  Type &value = A, &grad = B;
};

template <typename Type, std::size_t m, std::size_t n>
_DYM_FORCE_INLINE_ Dual<Vector<Type, m>>
operator*(const Matrix<Type, m, n> &ma, const Dual<Vector<Type, n>> &ve) {
  return {
      Vector<Type, m>([&](Type &e, int i) { e = vector::dot(ma[i], ve.A); }),
      Vector<Type, m>([&](Type &e, int i) { e = vector::dot(ma[i], ve.B); })};
}

#define _dym_dual_oneArg_(funname, ...)                                        \
  template <typename Type_>                                                    \
  _DYM_FORCE_INLINE_ Dual<Type_> funname(const Dual<Type_> &d) {               \
    return __VA_ARGS__;                                                        \
  }

#define _dym_dual_twoArg_(funname, argTypeName, ...)                           \
  template <typename Type_>                                                    \
  _DYM_FORCE_INLINE_ Dual<Type_> funname(const Dual<Type_> &d, argTypeName) {  \
    return __VA_ARGS__;                                                        \
  }

_dym_dual_oneArg_(sqr, d *d);
_dym_dual_twoArg_(pow, const int &s, {pow(d.A, s), s *d.B *pow(d.A, s - 1)});
_dym_dual_oneArg_(sqrt, {sqrt(d.A), d.B / (2 * sqrt(d.A))});
_dym_dual_oneArg_(cos, {cos(d.A), -d.B *sin(d.A)});
_dym_dual_oneArg_(cosh, {cosh(d.A), d.B *sinh(d.A)});
_dym_dual_oneArg_(acos, {acos(d.A), -d.B / sqrt(1 - sqr(d.A))});
_dym_dual_oneArg_(acosh, {acosh(d.A), d.B / sqrt(sqr(d.A) - 1)});
_dym_dual_oneArg_(sin, {sin(d.A), d.B *cos(d.A)});
_dym_dual_oneArg_(sinh, {sinh(d.A), d.B *cosh(d.A)});
_dym_dual_oneArg_(asin, {asin(d.A), d.B / sqrt(1 - sqr(d.A))});
_dym_dual_oneArg_(asinh, {asinh(d.A), d.B / sqrt(sqr(d.A) + 1)});
_dym_dual_oneArg_(tan, {tan(d.A), d.B / sqr(cos(d.A))});
_dym_dual_oneArg_(tanh, {tanh(d.A), d.B / sqr(cosh(d.A))});
_dym_dual_oneArg_(atan, {atan(d.A), d.B / (1 + sqr(d.A))});
_dym_dual_oneArg_(atanh, {atanh(d.A), d.B / (1 - sqr(d.A))});
_dym_dual_oneArg_(exp, {exp(d.A), d.B *exp(d.A)});
_dym_dual_oneArg_(exp2, {exp2(d.A), d.B *exp2(d.A) * exp((Type_)2)});
_dym_dual_oneArg_(expm1, {expm1(d.A), d.B *exp(d.A)});
_dym_dual_oneArg_(log, {log(d.A), d.B / d.A});
_dym_dual_oneArg_(log2, {log2(d.A), d.B / (d.A * log((Type_)2))});
_dym_dual_oneArg_(log10, {log10(d.A), d.B / (d.A * log((Type_)10))});
_dym_dual_oneArg_(log1p, {log1p(d.A), d.B / (d.A + 1)});
} // namespace dym
