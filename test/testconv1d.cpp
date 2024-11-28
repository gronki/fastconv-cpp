#include "conv1.hpp"
#include "pad.hpp"
#include "ref.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

constexpr std::array<int64_t, 9> array_sizes = {
      1024, 3238, 10240, 32382, 102400, 323817, 1024000, 3238172, 10240000};
constexpr std::array<int64_t, 13> kernel_sizes = {3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 19, 25, 31};

void fill_array(Array1D<float>& x)
{
   for ( std::size_t i = 0; i < x.size(); ++i )
   {
      x(i) = std::sin(0.072 * i) + std::sin(0.0013 * i) + std::sin(0.02 * i);
   }
}

void run_test(const std::string& test_title, Conv1DBase<float>* conv)
{
   using namespace std::chrono;

   double time_total = 0;
   double verif_x = 0, verif_y = 0;

   for ( const auto& array_size : array_sizes )
   {
      Array1D<float> x(array_size);

      for ( const auto& kernel_size : kernel_sizes )
      {
         Array1D<float> k(kernel_size);
         fill_array(k);
         conv->set_kernel(k);

         int64_t reps = std::max(1,
               static_cast<int>(std::
                           round(3 *
                                 (std::pow(static_cast<double>(*std::max_element(array_sizes.begin(),
                                                 array_sizes.end())),
                                        0.8) *
                                       std::pow(static_cast<double>(*std::max_element(kernel_sizes.begin(),
                                                      kernel_sizes.end())),
                                             0.5)) /
                                 (std::pow(static_cast<double>(array_size), 0.8) *
                                       std::pow(static_cast<double>(kernel_size), 0.5)))));

         // std::cout << array_size << " " << kernel_size << " " << reps <<
         // std::endl;

         for ( int64_t l = 0; l < reps; ++l )
         {
            fill_array(x);

            auto t1 = high_resolution_clock::now();
            Array1D<float> y = std::move(conv->conv(x));
            auto t2 = high_resolution_clock::now();

            time_total += duration<double>(t2 - t1).count();

            for ( int64_t ix = 0; ix < x.size(); ix++ )
            {
               verif_x += x(ix);
            }
            for ( int64_t iy = 0; iy < y.size(); iy++ )
            {
               verif_y += y(iy);
            }
         }
      }
   }

   std::cout << test_title << " --> " << std::fixed << std::setprecision(3) << "time = " << time_total
             << std::setw(17) << std::setprecision(0) << "  verif_x = " << verif_x
             << "  verif_y = " << verif_y << std::endl;
}

int main()
{
   // Assuming Conv1DRef and Conv1DPad are concrete implementations of Conv1DBase
   Conv1DRef<float> conv1d_ref;
   run_test("Conv1DRef", (Conv1DBase<float>*)&conv1d_ref);

   Conv1DPad<float> conv1d_pad_4(4), conv1d_pad_8(8), conv1d_pad_16(16);
   run_test("Conv1DPad (modulo=4)", (Conv1DBase<float>*)&conv1d_pad_4);
   run_test("Conv1DPad (modulo=8)", (Conv1DBase<float>*)&conv1d_pad_8);
   run_test("Conv1DPad (modulo=16)", (Conv1DBase<float>*)&conv1d_pad_16);

   return 0;
}
