#ifndef _VEC_HPP_
#define _VEC_HPP_


#include "fixed_point_type.hpp"

template<class T>
class Vec2 {
 public:
  explicit Vec2(const T& x=T(), const T& y=T()) : vals({x, y}) {
    return;
  }

  Vec2(const Vec2& src) : vals({src.vals[0], src.vals[1]}) {
    return;
  }
  
  Vec2 operator + (const Vec2& other) const {
    return Vec2(vals[0] + other.vals[0], vals[1] + other.vals[1]);
  }


  T operator * (const Vec2& other) const {
    return vals[0] * other.vals[0] + vals[1] * other.vals[1];
  }

  Vec2 operator - (const Vec2& other) const {
    return Vec2(vals[0] - other.vals[0], vals[1] - other.vals[1]); 
  }

  Vec2 operator * (const T& scalar) const {
    return Vec2(vals[0] * scalar, vals[1]*scalar);
  }
  
  Vec2 operator / (const T& scalar) const {
    return Vec2(vals[0] / scalar, vals[1] / scalar);
  }

  Vec2& operator += (const Vec2& other) {
    vals[0] += other.vals[0];
    vals[1] += other.vals[1];
    return *this;
  }

  Vec2& operator -= (const Vec2& other) {
    vals[0] -= other.vals[0];
    vals[1] -= other.vals[1];
    return *this;
  }

  Vec2& operator *= (const T& scalar) {
    vals[0] *= scalar;
    vals[1] *= scalar;
    return *this;
  }
  
  Vec2& operator /= (const T& scalar) {
    vals[0] /= scalar;
    vals[1] /= scalar;
    return *this;
  }

  T& operator [] (int component) {
    return vals[component];
    
  }

  T& X(void) const {
    return vals[0];
  }

  T& Y(void) const {
    return vals[1];
  } 


 private:
  T vals[2];
};


#endif  /* _VEC_HPP_ */
