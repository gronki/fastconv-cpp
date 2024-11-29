#include "conv1.hpp"
#include "pad.hpp"
#include "ref.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

const std::vector<int64_t> array_sizes = {10000L, 100000L, 1000000L, 10000000L};
const std::vector<int64_t> kernel_sizes = {3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 19, 25, 31};
bool extra_output = false;

void fill_array(Array1D<float>& x)
{
   for ( std::size_t i = 0; i < x.size(); ++i )
   {
      x(i) = std::sin(0.072 * i) + std::sin(0.0013 * i) + std::sin(0.02 * i);
   }
}

void print_list(const std::string& caption, const std::vector<int64_t>& items)
{
   std::cout << caption << ": ";
   for ( std::size_t i = 0; i < items.size(); i++ )
   {
      if ( i > 0 )
         std::cout << ", ";
      std::cout << items[i];
   }
   std ::cout << std::endl;
}

void run_test(const std::string& test_title, Conv1DBase<float>* conv)
{
   using namespace std::chrono;

   double time_total = 0;
   double verif_x = 0, verif_y = 0;
   double time_avg = 0;

   for ( const auto& array_size : array_sizes )
   {
      Array1D<float> x(array_size);

      for ( const auto& kernel_size : kernel_sizes )
      {
         Array1D<float> k(kernel_size);
         fill_array(k);
         conv->set_kernel(k);

         int64_t reps = std::max(1,
               static_cast<int>(std::round(
                     7 *
                     (std::pow(static_cast<double>(*std::max_element(array_sizes.begin(),
                                     array_sizes.end())),
                            0.8) *
                           std::pow(static_cast<double>(*std::max_element(kernel_sizes.begin(),
                                          kernel_sizes.end())),
                                 0.5)) /
                     (std::pow(static_cast<double>(array_size), 0.8) *
                           std::pow(static_cast<double>(kernel_size), 0.5)))));

         double time_onesize = 0;

         for ( int64_t l = 0; l < reps; ++l )
         {
            fill_array(x);

            auto t1 = high_resolution_clock::now();
            Array1D<float> y = std::move(conv->conv(x));
            auto t2 = high_resolution_clock::now();

            time_total += duration<double>(t2 - t1).count();
            time_onesize += duration<double>(t2 - t1).count();

            for ( int64_t ix = 0; ix < x.size(); ix++ )
            {
               verif_x += x(ix);
            }
            for ( int64_t iy = 0; iy < y.size(); iy++ )
            {
               verif_y += y(iy);
            }
         }

         time_onesize = time_onesize / reps * 1e9L / array_size;
         time_avg += time_onesize;

         if ( extra_output )
         {
            std::cout
                  << "array = " << array_size << "; kernel = " << kernel_size << "; reps = " << reps
                  << std::fixed << std::setw(8) << std::setprecision(5) << "; t = " << time_onesize
                  << std::endl;
         }
      }
   }

   time_avg /= array_sizes.size() * kernel_sizes.size();
   std::cout
         << test_title << " --> " << std::fixed << std::setprecision(2) << "time = " << time_total
         << std::setprecision(5) << "; sec/GOps = " << time_avg << std::setw(17) << std::setprecision(0)
         << "; verif_x = " << verif_x << "; verif_y = " << verif_y << std::endl;
}

void read_env()
{
   const char* buf = std::getenv("EXTRA_OUTPUT");
   if ( !buf )
   {
      extra_output = false;
      return;
   }
   extra_output = std::atoi(buf) != 0;
}

int main()
{

   read_env();

   print_list("array sizes", array_sizes);
   print_list("kernel sizes", kernel_sizes);

   // Assuming Conv1DRef and Conv1DPad are concrete implementations of Conv1DBase
   Conv1DRef<float> conv1d_ref;
   run_test("Conv1DRef", (Conv1DBase<float>*)&conv1d_ref);

   Conv1DPad<float> conv1d_pad_4(4), conv1d_pad_8(8), conv1d_pad_16(16);
   run_test("Conv1DPad (modulo=4)", (Conv1DBase<float>*)&conv1d_pad_4);
   run_test("Conv1DPad (modulo=8)", (Conv1DBase<float>*)&conv1d_pad_8);
   run_test("Conv1DPad (modulo=16)", (Conv1DBase<float>*)&conv1d_pad_16);

   return 0;
}
