#ifndef SIMPLE_HPP
#define SIMPLE_HPP

#include "conv1.hpp"
#include "core.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Class definition for 1D Convolution
template <typename FloatType>
class Conv1DRef : Conv1DBase<FloatType>
{
 public:
   // Constructor
   inline Conv1DRef(bool preserve_shape = false)
         : preserve_shape(preserve_shape)
   {
   }
   inline Conv1DRef(const ConstArrayView1D<FloatType>& init_kernel, bool preserve_shape = false)
         : preserve_shape(preserve_shape)
   {
      set_kernel(init_kernel);
   }

   // Set the convolution kernel (reverse it)
   inline void set_kernel(const ConstArrayView1D<FloatType>& new_kernel)
   {
      kernel = std::move(Array1D<FloatType>(new_kernel.size()));
      for ( std::size_t i = 0; i < kernel.size(); i++ )
      {
         kernel(i) = new_kernel(new_kernel.size() - 1 - i);
      }
   }

   // Compute the output size based on input size
   inline std::size_t output_size(std::size_t input_size) const
   {
      return input_size + (preserve_shape ? 0 : 1 - kernel.size());
   }

   // Perform the 1D convolution
   inline Array1D<FloatType> conv(const ConstArrayView1D<FloatType>& x) const
   {
      std::size_t input_size = x.size();
      Array1D<FloatType> y(output_size(input_size));

      std::size_t kernel_size = kernel.size();
      std::size_t raw_output_size = input_size - kernel_size + 1;
      std::size_t offset = preserve_shape ? (kernel_size - 1) / 2 : 0;

      conv1d_core<FloatType>(x, kernel, y.view(offset, raw_output_size + offset));
      return y;
   }

 private:
   Array1D<FloatType> kernel; // Reversed convolution kernel
   bool preserve_shape;       // Preserve shape of the input/output
};

#endif // SIMPLE_HPP
