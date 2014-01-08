//
//  svvim.h
//  SvvimGram
//
//  Created by Matthew Owen on 1/6/14.
//
//

#ifndef SvvimGram_svvim_h
#define SvvimGram_svvim_h

namespace svvim {
  
  
  /** 
   *  Random Uniform Float on [a , b]
   *  @param {float} a the lower bound on the random uniform float
   *  @param {float} b the upper bound on the random uniform float
   *  @return {float} a random float x, s.t. a <= x <= b
   */
  float random (float a, float b) {
    float x = rand() / (float) RAND_MAX;
    return a + (b-a) * x;
  }
  
  
  class TimeCounter {
  public:
    TimeCounter() {
      mStart      = clock();
      mSinceTime  = clock();
    }
    float getElapsedSeconds () {
      mLast   = mNow;
      mNow    = clock();
      mSince  = mNow - mLast;
      return (float) (mNow - mStart) / CLOCKS_PER_SEC;
    }
    float since () {
      clock_t now = clock();
      mNow        = now;
      mSince      = now - mSinceTime;
      mSinceTime  = now;
      return mSince;
    }
  private:
    clock_t mStart;
    clock_t mLast;
    clock_t mNow;
    float   mSince;
    float   mSinceTime;
  };
  
  
  
  namespace cinder {
  
    // 1
  
    namespace ci = cinder;
  }
}

namespace svv = svvim;

#endif