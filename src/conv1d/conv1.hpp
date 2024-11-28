#ifndef CONV1D_H
#define CONV1D_H

#include "conv_base.hpp"
#include "myarray.hpp"
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

// Base abstract class for 1D convolution
template <typename FloatType>
class Conv1DBase : ConvBase
{
 public:
   virtual ~Conv1DBase() = default;

   // Pure virtual functions (abstract methods)
   virtual void set_kernel(const ConstArrayView1D<FloatType>& k) = 0;
   virtual std::size_t output_size(std::size_t input_size) const = 0;
   virtual Array1D<FloatType> conv(const ConstArrayView1D<FloatType>& input) const = 0;
};

#endif // CONV1D_H
