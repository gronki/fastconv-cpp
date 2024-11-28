#include "myarray.hpp"
#include <iostream>

template <typename T>
Array1D<T> getrange(std::size_t n)
{
   Array1D<T> range{n};
   for ( std::size_t i = 0; i < range.size(); i++ )
   {
      range(i) = i;
   }
   return range;
}

template <typename T>
T sum(const ConstArrayView1D<T>& arr)
{
   T result = 0;
   for ( size_t i = 0; i < arr.size(); i++ )
   {
      result += arr(i);
   }
   return result;
}

int main()
{

   Array1D<float> arr1 = getrange<float>(20);
   std::cout << sum<float>(arr1.view(5, 6)) << std::endl;
   std::cout << arr1.view(5, 6)(0) << std::endl;
   Array1D<float> arr2{arr1.view(10, 12)};
   std::cout << sum<float>(arr2) << std::endl;
}
