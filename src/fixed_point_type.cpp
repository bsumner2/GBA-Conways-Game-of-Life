#include "fixed_point_type.hpp"

FP24_8_t::FP24_8_t(s32_t value, bool raw_value) 
    : _raw_val(raw_value ? value : (value<<FIX_SHIFT)) {
  return;
}

FP24_8_t::FP24_8_t(float f_val) : _raw_val((s32_t)(f_val*FIX_SCALEF)) {
  return;
}

FP24_8_t::FP24_8_t(void) : _raw_val(0) {
  return;
}

FP24_8_t::FP24_8_t(const FP24_8_t& operand1, const FP24_8_t& operand2, 
    char operation) : _raw_val(operand1._raw_val) {
  switch (operation) {
    case '+':
      _raw_val += operand2._raw_val;
      break;
    case '-':
      _raw_val -= operand2._raw_val;
      break;
    case '*':
      _raw_val *= operand2._raw_val;
      _raw_val >>= FIX_SHIFT;
      break;
    case '/':
      _raw_val <<= FIX_SHIFT;
      _raw_val /= operand2._raw_val;
      break;
    default:
      _raw_val = 0;
      break;
  }
  return;
}




FP24_8_t::FP24_8_t(const FP24_8_t& copy) : _raw_val(copy._raw_val) {
  return;
}

u32_t FP24_8_t::ToUInt32(void) const {
  return (~_raw_val+1)>>FIX_SHIFT;
}

s32_t FP24_8_t::ToInt32(void) const {
  return _raw_val>>FIX_SHIFT;
}


float FP24_8_t::ToFloat(void) const {
  return _raw_val/FIX_SCALEF;
}


FP24_8_t FP24_8_t::operator + (const FP24_8_t& addend) const {
  return FP24_8_t(_raw_val + addend._raw_val, true);
}


FP24_8_t FP24_8_t::operator - (const FP24_8_t& subtrahend) const {
  return FP24_8_t(this->_raw_val - subtrahend._raw_val, true);
}

FP24_8_t& FP24_8_t::operator = (const FP24_8_t& new_val) {
  this->_raw_val = new_val._raw_val;
  return *this;
}

FP24_8_t FP24_8_t::operator * (const FP24_8_t& factor) const {
  return FP24_8_t((this->_raw_val*factor._raw_val)>>FIX_SHIFT, true);
}

FP24_8_t FP24_8_t::operator / (const FP24_8_t& divisor) const {
  return FP24_8_t(((_raw_val<<FIX_SHIFT)/(divisor._raw_val)), true);
}

bool FP24_8_t::operator > (const FP24_8_t& rhs) const {
  return _raw_val > rhs._raw_val;
}

bool FP24_8_t::operator < (const FP24_8_t& rhs) const {
  return _raw_val < rhs._raw_val;
}

bool FP24_8_t::operator >= (const FP24_8_t& rhs) const {
  return _raw_val >= rhs._raw_val;
}

bool FP24_8_t::operator <= (const FP24_8_t& rhs) const {
  return _raw_val <= rhs._raw_val;
}

bool FP24_8_t::operator == (const FP24_8_t& rhs) const {
  return _raw_val == rhs._raw_val;
}




FP24_8_t& FP24_8_t::operator += (const FP24_8_t& addend) {
  _raw_val += addend._raw_val;
  return *this;
}


FP24_8_t& FP24_8_t::operator -= (const FP24_8_t& subtrahend) {
  _raw_val -= subtrahend._raw_val;
  return *this;
}

FP24_8_t& FP24_8_t::operator *= (const FP24_8_t& factor) {
  _raw_val *= factor._raw_val;
  _raw_val >>= FIX_SHIFT;
  return *this;
}


FP24_8_t& FP24_8_t::operator /= (const FP24_8_t& divisor) {
  _raw_val *= FIX_SCALE;
  _raw_val /= (divisor._raw_val);
  return *this;
}


bool FP24_8_t::operator > (s32_t val) const {
  return _raw_val > (val<<FIX_SHIFT);
}


bool FP24_8_t::operator < (s32_t val) const {
  return _raw_val < (val<<FIX_SHIFT);
}

bool FP24_8_t::operator >= (s32_t val) const {
  return _raw_val >= (val<<FIX_SHIFT);
}

bool FP24_8_t::operator <= (s32_t val) const {
  return _raw_val <= (val<<FIX_SHIFT);
}

bool FP24_8_t::operator == (s32_t val) const {
  return _raw_val == (val<<FIX_SHIFT);
}

FP24_8_t FP24_8_t::operator + (s32_t val) const {
  return FP24_8_t((val<<FIX_SHIFT) + _raw_val, true);
}

FP24_8_t FP24_8_t::operator - (s32_t val) const {
  return FP24_8_t(_raw_val - (val<<FIX_SHIFT), true);
}

FP24_8_t FP24_8_t::operator * (s32_t val) const {
  return FP24_8_t(_raw_val*val, true);
}

FP24_8_t FP24_8_t::operator / (s32_t val) const {
  return FP24_8_t(_raw_val/val);
}
         
FP24_8_t& FP24_8_t:: operator = (s32_t val) {
  _raw_val = val<<FIX_SHIFT;
  return *this;
}

FP24_8_t& FP24_8_t:: operator *= (s32_t val) {
  _raw_val *= val;
  return *this;
}

FP24_8_t& FP24_8_t:: operator += (s32_t val) {
  _raw_val += val<<FIX_SHIFT;
  return *this;
}

FP24_8_t& FP24_8_t:: operator -= (s32_t val) {
  _raw_val -= val<<FIX_SHIFT;
  return *this;
}

FP24_8_t& FP24_8_t:: operator /= (s32_t val) {
  _raw_val /= val;
  return *this;
}
 
