#pragma once

#include <cassert>
#include <exception>
#include <iostream>

template <typename Numeric>
class ArrayView1D;
template <typename Numeric>
class ConstArrayView1D;

template <typename Numeric>
class Array1D
{
   Numeric* data;
   std::size_t data_size;

 public:
   inline Array1D()
         : data(nullptr), data_size(0)
   {
   }

   inline Array1D(std::size_t size)
         : data_size(size)
   {
      data = new Numeric[data_size];
   }

   inline Array1D(const Array1D<Numeric>& other)
         : data_size(other.data_size)
   {
      data = new Numeric[data_size];
      for ( std::size_t i = 0; i < data_size; i++ )
      {
         data[i] = other.data[i];
      }
   }

   inline Array1D(Array1D<Numeric>&& other)
         : data(other.data), data_size(other.data_size)
   {
      other.data = nullptr;
#ifndef NDEBUG
      std::cout << "Move constructor called with size " << data_size << std::endl;
#endif
   }

   inline Array1D(const ConstArrayView1D<Numeric>& view)
         : data_size(view.length)
   {
      std::cout << "Array1D" << " c-tor from ConstArrayView" << std::endl;
      data = new Numeric[data_size];
      for ( std::size_t i = 0; i < data_size; i++ )
      {
         data[i] = view.data[view.offset + i];
      }
   }

   inline ~Array1D()
   {
#ifndef NDEBUG
      std::cout << "Destructor called with size " << data_size << std::endl;
#endif
      if ( data )
         delete[] data;
   }

   inline std::size_t size() const
   {
      return data_size;
   }

   inline Numeric& operator()(std::size_t idx)
#ifdef NDEBUG
         noexcept
#endif
   {
#ifndef NDEBUG
      if ( idx < 0 || idx >= size() )
      {
         throw std::runtime_error("Out of bounds");
      }
#endif
      // std::cout << "Access operator at index " << idx << std::endl;
      return data[idx];
   }

   inline const Numeric& operator()(std::size_t idx) const
#ifdef NDEBUG
         noexcept
#endif
   {
#ifndef NDEBUG
      if ( idx < 0 || idx >= size() )
      {
         throw std::runtime_error("Out of bounds");
      }
#endif
      // std::cout << "Const access operator at index " << idx << std::endl;
      return data[idx];
   }

   inline Array1D<Numeric>& operator=(const Array1D<Numeric>& other)
   {
#ifndef NDEBUG
      std::cout << "copy assignment " << describe() << " -> " << other.describe() << std::endl;
#endif
      if ( data )
      {
         delete[] data;
         data = nullptr;
      }
      if ( !other.data )
         return *this;
      data_size = other.data_size;
      data = new Numeric[data_size];
      for ( std::size_t i = 0; i < data_size; i++ )
      {
         data[i] = other.data[i];
      }
      return *this;
   }

   inline Array1D<Numeric>& operator=(Array1D<Numeric>&& other)
   {
#ifndef NDEBUG
      std::cout << "move assignment " << describe() << " -> " << other.describe() << std::endl;
#endif
      if ( data )
      {
         delete[] data;
         data = nullptr;
      }
      if ( !other.data )
         return *this;
      data_size = other.data_size;
      data = other.data;
      other.data = nullptr;
      return *this;
   }

   std::string describe() const
   {
      if ( !data )
         return "<unallocated>";
      return "{" + std::to_string(0) + ".." + std::to_string(size() - 1) + "}";
   }

   inline ArrayView1D<Numeric> view(std::size_t start, std::size_t end)
   {
      return {*this, start, end - start};
   }
   inline ConstArrayView1D<Numeric> const_view(std::size_t start, std::size_t end) const
   {
      return {*this, start, end - start};
   }

   friend class ArrayView1D<Numeric>;
   friend class ConstArrayView1D<Numeric>;
};

template <typename Numeric>
class ArrayView1D
{
   Numeric* data;
   std::size_t offset, length;

 public:
   // Array c-tors

   ArrayView1D(Array1D<Numeric>& array)
         : data(array.data), offset(0), length(array.data_size)
   {
#ifndef NDEBUG
      std::cout << "ArrayView1D" << " c-tor from array" << std::endl;
#endif
   }
   ArrayView1D(Array1D<Numeric>& array, std::size_t offset, std::size_t length)
         : data(array.data), offset(offset), length(length)
   {
#ifndef NDEBUG
      std::cout << "ArrayView1D" << " c-tor from array (offset, length)" << std::endl;
#endif
      assert(offset >= 0);
      assert(offset + length <= array.data_size);
   }

   // ArrayView c-tors

   ArrayView1D(const ArrayView1D<Numeric>& view)
         : data(view.data), offset(view.offset), length(view.length)
   {
#ifndef NDEBUG
      std::cout << "ArrayView1D" << " c-tor from view" << std::endl;
#endif
   }
   ArrayView1D(const ArrayView1D<Numeric>& view, std::size_t offset, std::size_t length)
         : data(view.data), offset(view.offset + offset), length(length)
   {
#ifndef NDEBUG
      std::cout << "ArrayView1D" << " c-tor from view (offset, length)" << std::endl;
#endif
      assert(offset >= 0);
      assert(offset + length <= view.length);
   }

   inline ~ArrayView1D() = default;

   inline std::size_t size() const
   {
      return length;
   }

   inline Numeric& operator()(std::size_t idx)
#ifdef NDEBUG
         noexcept
#endif
   {
#ifndef NDEBUG
      if ( idx < 0 || idx >= size() )
      {
         throw std::runtime_error("Out of bounds");
      }
#endif
      // std::cout << "ArrayView: Access operator at index " << idx << std::endl;
      return data[idx + offset];
   }

   inline const Numeric& operator()(std::size_t idx) const
#ifdef NDEBUG
         noexcept
#endif
   {
#ifndef NDEBUG
      if ( idx < 0 || idx >= size() )
      {
         throw std::runtime_error("Out of bounds");
      }
#endif
      // std::cout << "ArrayView: Const access operator at index " << idx <<
      // std::endl;
      return data[idx + offset];
   }

   inline ArrayView1D<Numeric> view(std::size_t start, std::size_t end) const
   {
      return {*this, start, end - start};
   }

   friend class ConstArrayView1D<Numeric>;
};

template <typename Numeric>
class ConstArrayView1D
{
   const Numeric* data;
   std::size_t offset, length;

 public:
   // Array c-tors

   ConstArrayView1D(const Array1D<Numeric>& array)
         : data(array.data), offset(0), length(array.data_size)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D" << " c-tor from Array" << std::endl;
#endif
   }
   ConstArrayView1D(const Array1D<Numeric>& array, std::size_t offset, std::size_t length)
         : data(array.data), offset(offset), length(length)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D" << " c-tor from Array (offset, length)" << std::endl;
#endif
      assert(offset >= 0);
      assert(offset + length <= array.data_size);
   }

   // ArrayView c-tors

   ConstArrayView1D(const ArrayView1D<Numeric>& view)
         : data(view.data), offset(view.offset), length(view.length)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D" << " c-tor from ArrayView" << std::endl;
#endif
   }

   ConstArrayView1D(const ArrayView1D<Numeric>& view, std::size_t offset, std::size_t length)
         : data(view.data), offset(view.offset + offset), length(length)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D" << " c-tor from ArrayView (offset, length)" << std::endl;
#endif
      assert(offset >= 0);
      assert(offset + length <= view.length);
   }

   // ConstArrayView c-tors

   ConstArrayView1D(const ConstArrayView1D<Numeric>& view)
         : data(view.data), offset(view.offset), length(view.length)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D" << " c-tor from ConstArrayView" << std::endl;
#endif
   }

   ConstArrayView1D(const ConstArrayView1D<Numeric>& view, std::size_t offset, std::size_t length)
         : data(view.data), offset(view.offset + offset), length(length)
   {
#ifndef NDEBUG
      std::cout << "ConstArrayView1D"
                << " c-tor from ConstArrayView (offset, length)" << std::endl;
#endif
      assert(offset >= 0);
      assert(offset + length <= view.length);
   }

   inline ~ConstArrayView1D() = default;

   inline std::size_t size() const
   {
      return length;
   }

   inline const Numeric& operator()(std::size_t idx) const
#ifdef NDEBUG
         noexcept
#endif
   {
#ifndef NDEBUG
      if ( idx < 0 || idx >= size() )
      {
         throw std::runtime_error("Out of bounds");
      }
#endif
      // std::cout << "ConstArrayView: Const access operator at index " << idx <<
      // std::endl;
      return data[idx + offset];
   }

   inline ConstArrayView1D<Numeric> view(std::size_t start, std::size_t end) const
   {
      return {*this, start, end - start};
   }

   friend class Array1D<Numeric>;
};
