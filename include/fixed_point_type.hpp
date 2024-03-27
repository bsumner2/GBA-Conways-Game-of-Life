#ifndef _FIXED_PT_TYPE_HPP_
#define _FIXED_PT_TYPE_HPP_

#include "gba_types.h"

#define FIX_SHIFT 8
#define FIX_SCALE (1<<FIX_SHIFT)
#define FIX_MASK (FIX_SCALE-1)
#define FIX_SCALEF ((float)FIX_SCALE)
#define FIX_SCALEF_INVERSE (1.0/FIX_SCALEF)


class FP24_8_t {
 public:
  FP24_8_t(void);
  explicit FP24_8_t(s32_t value, bool is_raw_value=false);
  explicit FP24_8_t(float value);

  FP24_8_t(const FP24_8_t& copy);
  explicit FP24_8_t(const FP24_8_t& operand1, const FP24_8_t& operand2, 
      char operation);
   
  u32_t ToUInt32(void) const;
  s32_t ToInt32(void) const;
  float ToFloat(void) const;

  FP24_8_t operator + (s32_t) const;
  FP24_8_t operator - (s32_t) const;
  FP24_8_t operator * (s32_t) const;
  FP24_8_t operator / (s32_t) const;

  FP24_8_t& operator = (s32_t);
  FP24_8_t& operator *= (s32_t);
  FP24_8_t& operator += (s32_t);
  FP24_8_t& operator -= (s32_t);
  FP24_8_t& operator /= (s32_t); 


  FP24_8_t operator + (const FP24_8_t&) const;
  FP24_8_t operator - (const FP24_8_t&) const;
  FP24_8_t operator * (const FP24_8_t&) const;
  FP24_8_t operator / (const FP24_8_t&) const;

  FP24_8_t& operator = (const FP24_8_t&);
  FP24_8_t& operator *= (const FP24_8_t&);
  FP24_8_t& operator += (const FP24_8_t&);
  FP24_8_t& operator -= (const FP24_8_t&);
  FP24_8_t& operator /= (const FP24_8_t&); 
  
  bool operator > (const FP24_8_t&) const;
  bool operator < (const FP24_8_t&) const;
  bool operator >= (const FP24_8_t&) const;
  bool operator <= (const FP24_8_t&) const;
  bool operator == (const FP24_8_t&) const;

  bool operator > (s32_t) const;
  bool operator < (s32_t) const;
  bool operator >= (s32_t) const;
  bool operator <= (s32_t) const;
  bool operator == (s32_t) const;

  u32_t RawValue(void) const { return _raw_val; }

    
 private:
  s32_t _raw_val;

};






#endif  /* _FIXED_PT_TYPE_HPP_ */
