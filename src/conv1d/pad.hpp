#ifndef PAD_HPP
#define PAD_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "conv1.hpp"
#include "core.hpp"

// Class definition for padded 1D convolution
template <typename FloatType>
class Conv1DPad : Conv1DBase<FloatType>
{
 public:
   // Constructor
   inline Conv1DPad(std::size_t pad_modulo, bool preserve_shape = false)
         : pad_modulo(pad_modulo), preserve_shape(preserve_shape), padding(0), kernel_size(0)
   {
   }
   inline Conv1DPad(const ConstArrayView1D<FloatType>& k, std::size_t pad_modulo,
         bool preserve_shape = false)
         : pad_modulo(pad_modulo), preserve_shape(preserve_shape)
   {
      set_kernel(k);
   }

   // Set the convolution kernel and compute padding
   inline void set_kernel(const ConstArrayView1D<FloatType>& k)
   {
      kernel_size = k.size();

      const std::size_t many = pad_modulo * (kernel_size / pad_modulo + 1);
      padding = (many - kernel_size) % pad_modulo;
      kernel = std::move(Array1D<FloatType>(kernel_size + padding));
      /*std::cout << "initial size = " << kernel_size << "; pad_modulo = " <<
         pad_modulo
           << "; final size = " << kernel_size + padding << std::endl;*/
      for ( std::size_t i = 0; i < kernel.size(); i++ )
      {
         if ( i < kernel_size )
         {
            kernel(i) = k(kernel_size - i - 1);
         }
         else
         {
            kernel(i) = 0;
         }
      }
   }

   // Compute the output size based on input size
   inline std::size_t output_size(std::size_t input_size) const
   {
      return input_size + (preserve_shape ? 0 : 1 - kernel_size);
   }

   // Perform the padded 1D convolution
   inline Array1D<FloatType> conv(const ConstArrayView1D<FloatType>& x) const
   {

      std::size_t input_size = x.size();
      Array1D<FloatType> y(output_size(input_size));

      std::size_t raw_output_size = input_size - kernel_size + 1;
      std::size_t offset = preserve_shape ? (kernel_size - 1) / 2 : 0;

      conv1d_core<FloatType>(x, kernel, y.view(offset, offset + raw_output_size - padding));

      if ( padding > 0 )
      {
         conv1d_core<FloatType>(x.view(raw_output_size - padding, input_size),
               kernel.const_view(0, kernel_size),
               y.view(offset + raw_output_size - padding, raw_output_size + offset));
      }
      return y;
   }

 private:
   Array1D<FloatType> kernel; // Convolution kernel, including padding
   std::size_t pad_modulo;    // Padding alignment constraint
   bool preserve_shape;       // Preserve shape of the input/output
   std::size_t padding;       // Computed padding size
   std::size_t kernel_size;   // Size of the actual kernel
};

#endif // PAD_HPP
