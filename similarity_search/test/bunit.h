/**
 * Non-metric Space Library
 *
 * Authors: Bilegsaikhan Naidan (https://github.com/bileg), Leonid Boytsov (http://boytsov.info).
 * With contributions from Lawrence Cayton (http://lcayton.com/).
 *
 * For the complete list of contributors and further details see:
 * https://github.com/searchivarius/NonMetricSpaceLib 
 * 
 * Copyright (c) 2010--2013
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 */

#ifndef _BUNIT_H_
#define _BUNIT_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include <utility>
#include <tuple>
#include <exception>

namespace similarity {

using TestFunc = std::function<void()>;

class TestException : std::exception {
 public:
  TestException(const std::string& msg) : msg_(msg) {}
  const char* what() const noexcept { return msg_.c_str(); }
 private:
  const std::string msg_;
};

const std::string kDisable = "DISABLE_";

class TestRunner {
 public:
  ~TestRunner() {}

  static TestRunner& Instance();
  void AddTest(const std::string& test_name, TestFunc& test_func);
  int RunAllTests();

 private:
  TestRunner() {}
  // <test_name, test_func, is_disabled>
  std::vector<std::tuple<std::string,TestFunc,bool>> tests_;
};

#define TEST(test_name) \
  class _Test_##test_name##_ { \
   public: \
    _Test_##test_name##_() { \
      TestFunc test_func = std::bind(&_Test_##test_name##_::Test, this); \
      similarity::TestRunner::Instance().AddTest(#test_name, test_func); \
    } \
    ~_Test_##test_name##_() {} \
   private: \
    void Test(); \
  }; \
  similarity::_Test_##test_name##_ _test_##test_name##_; \
  void _Test_##test_name##_::Test()


template <typename T>
inline T ABS(const T& x) {
  return x >= 0 ? x : -x;
}

template <typename T>
inline bool EQ(const T& x, const T& y) {
  return x == y;
}

template <>
inline bool EQ<float>(const float& x, const float& y) {
  return ABS(x - y) <= 1e-5;
}

template <>
inline bool EQ<double>(const double& x, const double& y) {
  return ABS(x - y) <= 1e-5;
}

template <typename T>
static inline void Expect_EQ(const std::string& msg,
                             const T& expected,
                             const T& actual,
                             const std::string& file_name,
                             int line_num) {
  if (!EQ(expected, actual)) {
    std::stringstream ss;
    ss << file_name << "(" << line_num << "): "
       << "EXPECT_EQ(" << msg << ") " << std::endl
       << "   expected: " << expected << std::endl
       << "   actual  : " << actual
       << std::endl;
    throw TestException(ss.str());
  }
}

template <typename T>
static inline void Expect_NE(const std::string& msg,
                             const T& expected,
                             const T& actual,
                             const std::string& file_name,
                             int line_num) {
  if (EQ(expected, actual)) {
    std::stringstream ss;
    ss << file_name << "(" << line_num << "): "
       << "EXPECT_NE(" << msg << ") " << std::endl
       << "   expected: " << expected << std::endl
       << "   actual  : " << actual
       << std::endl;
    throw TestException(ss.str());
  }
}

static inline void Expect_True(const std::string& msg,
                               const int condition,
                               const std::string& file_name,
                               int line_num) {
  if (!condition) {
    std::stringstream ss;
    ss << file_name << "(" << line_num << "): "
       << "EXPECT_TRUE(" << msg << ") " << std::endl;
    throw TestException(ss.str());
  }
}

static inline void Expect_False(const std::string& msg,
                                const int condition,
                                const std::string& file_name,
                                int line_num) {
  if (condition) {
    std::stringstream ss;
    ss << file_name << "(" << line_num << "): "
       << " EXPECT_FALSE(" << msg << ") " << std::endl;
    throw TestException(ss.str());
  }
}

#define EXPECT_EQ(expected, actual) \
  similarity::Expect_EQ(#expected ", " #actual, (expected), (actual), __FILE__, __LINE__)

#define EXPECT_NE(expected, actual) \
  similarity::Expect_NE(#expected ", " #actual, (expected), (actual), __FILE__, __LINE__)

#define EXPECT_TRUE(condition) \
  similarity::Expect_True(#condition, (condition), __FILE__, __LINE__)

#define EXPECT_FALSE(condition) \
  similarity::Expect_False(#condition, (condition), __FILE__, __LINE__)

}     // namespace similarity

#endif    // _BUNIT_H_