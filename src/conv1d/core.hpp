#ifndef CONV1D_CORE_HPP
#define CONV1D_CORE_HPP

#include "myarray.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

// General implementation for arbitrary kernel size
template <typename FloatType>
void conv1d_general(ConstArrayView1D<FloatType> x, ConstArrayView1D<FloatType> k, ArrayView1D<FloatType> y)
{
   const auto kernel_size = k.size();
   const auto output_size = x.size() - kernel_size + 1;

   for ( size_t i = 0; i < output_size; ++i )
   {
      FloatType total = 0.0f;
      for ( size_t j = 0; j < kernel_size; ++j )
      {
         total += k(j) * x(i + j);
      }
      y(i) = total;
   }
}

// Optimized implementation for kernel sizes that are multiples of 4
template <typename FloatType, std::size_t kernel_block_size>
void conv1d_km(ConstArrayView1D<FloatType> x, ConstArrayView1D<FloatType> k, ArrayView1D<FloatType> y)
{
   assert(k.size() % kernel_block_size == 0 && "Kernel size must be a multiple of kSizeMul");
   const auto kernel_blocks = k.size() / kernel_block_size;
   const auto output_size = x.size() - k.size() + 1;

   for ( size_t i = 0; i < output_size; ++i )
   {
      FloatType total = 0.0f;
      for ( size_t j = 0; j < kernel_blocks * kernel_block_size; ++j )
      {
         total += k(j) * x(i + j);
      }
      y(i) = total;
   }
}

// A generic function for 1D convolution
template <typename FloatType>
void conv1d_core(ConstArrayView1D<FloatType> x, ConstArrayView1D<FloatType> k, ArrayView1D<FloatType> y)
{
   const auto kernel_size = k.size();
   const auto output_size = x.size() - kernel_size + 1;

   if ( y.size() != output_size )
   {
      throw std::runtime_error("Incorrect output shape for 1D convolution");
   }

   if ( kernel_size % 16 == 0 )
   {
      conv1d_km<FloatType, 16>(x, k, y);
   }
   else if ( kernel_size % 8 == 0 )
   {
      conv1d_km<FloatType, 8>(x, k, y);
   }
   else if ( kernel_size % 4 == 0 )
   {
      conv1d_km<FloatType, 4>(x, k, y);
   }
   else if ( kernel_size == 1 )
   {
      for ( size_t i = 0; i < output_size; ++i )
      {
         y(i) = x(i) * k(0);
      }
   }
   else
   {
      conv1d_general(x, k, y);
   }
}

#endif // CONV1D_CORE_HPP
