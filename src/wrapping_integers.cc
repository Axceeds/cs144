#include "wrapping_integers.hh"
#include <cmath>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32(static_cast<uint32_t>(static_cast<uint32_t>(n)+zero_point.raw_value_));
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.

  uint32_t offset = raw_value_ - zero_point.raw_value_;
  uint64_t t = (checkpoint & 0xFFFFFFFF00000000) + offset;
  uint64_t ret = t;
  if (abs(int64_t(t + (1ul << 32) - checkpoint)) < abs(int64_t(t - checkpoint)))
      ret = t + (1ul << 32);
  if (t >= (1ul << 32) && abs(int64_t(t - (1ul << 32) - checkpoint)) < abs(int64_t(ret - checkpoint)))
      ret = t - (1ul << 32);
  return ret;
}
