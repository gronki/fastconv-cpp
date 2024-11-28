#ifndef CONV_BASE_H
#define CONV_BASE_H

#include <cstdint>

class ConvBase
{
 public:
   // Virtual destructor for abstract base class
   virtual ~ConvBase() = default;

   // Public member variable, default initialized to false
   bool preserve_shape = false;
};

#endif // CONV_BASE_H
