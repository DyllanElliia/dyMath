/*
 * @Author: DyllanElliia
 * @Date: 2021-10-06 17:00:50
 * @LastEditors: DyllanElliia
 * @LastEditTime: 2022-02-09 16:50:29
 * @Description:
 */

#pragma once

// #include "./picture.hpp"
#include "src/tensor.hpp"
#include "src/vector.hpp"
#include "src/matrix.hpp"
#include "src/realALG.hpp"
#include "src/vecALG.hpp"
#include "src/matALG.hpp"

namespace dym {
template <class InputType>
Tensor<InputType> abs(Tensor<InputType> in) {
  in.for_each_i([](InputType &e) { e = dym::max(e, (InputType)0); });
  return in;
}

template <typename ValueType>
Tensor<ValueType> cross(const Tensor<ValueType> &A,
                        const Tensor<ValueType> &B) {
  auto As = A.shape(), Bs = B.shape();
  try {
    if (As.rank != 2 || Bs.rank != 2)
      throw "\033[1;31mDyMath cross() error: Tensor must is matrix!\033[0m]";
  } catch (const char *str) {
    std::cerr << str << '\n';
    return Tensor<ValueType>(0, dym::gi(0));
  }

  Tensor<ValueType> result(0, dym::gi(As[0], Bs[1]));
  const int &flen = As[1] == Bs[0] ? As[1] : 0, &bs1 = Bs[1];
  result.for_each_i([&](ValueType &e, int i, int j) {
    for (int ii = 0; ii < flen; ++ii) e += A[i * flen + ii] * B[ii * bs1 + j];
  });
  return result;
}
};  // namespace dym