/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_object_pool_test_init = false;
#include "/home/hobby/dev/FixedContainers/test/inc/object_pool_test.h"

static object_pool_test suite_object_pool_test;

static CxxTest::List Tests_object_pool_test = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_object_pool_test( "./test/inc/object_pool_test.h", 21, "object_pool_test", suite_object_pool_test, Tests_object_pool_test );

static class TestDescription_suite_object_pool_test_testCapacity : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testCapacity() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 25, "testCapacity" ) {}
 void runTest() { suite_object_pool_test.testCapacity(); }
} testDescription_suite_object_pool_test_testCapacity;

static class TestDescription_suite_object_pool_test_testAvailable : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testAvailable() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 31, "testAvailable" ) {}
 void runTest() { suite_object_pool_test.testAvailable(); }
} testDescription_suite_object_pool_test_testAvailable;

static class TestDescription_suite_object_pool_test_testOrderAllocate : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testOrderAllocate() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 46, "testOrderAllocate" ) {}
 void runTest() { suite_object_pool_test.testOrderAllocate(); }
} testDescription_suite_object_pool_test_testOrderAllocate;

static class TestDescription_suite_object_pool_test_testUnorderedAllocate : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testUnorderedAllocate() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 65, "testUnorderedAllocate" ) {}
 void runTest() { suite_object_pool_test.testUnorderedAllocate(); }
} testDescription_suite_object_pool_test_testUnorderedAllocate;

static class TestDescription_suite_object_pool_test_testResetOnRelease : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testResetOnRelease() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 84, "testResetOnRelease" ) {}
 void runTest() { suite_object_pool_test.testResetOnRelease(); }
} testDescription_suite_object_pool_test_testResetOnRelease;

static class TestDescription_suite_object_pool_test_testAllocateException : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_object_pool_test_testAllocateException() : CxxTest::RealTestDescription( Tests_object_pool_test, suiteDescription_object_pool_test, 94, "testAllocateException" ) {}
 void runTest() { suite_object_pool_test.testAllocateException(); }
} testDescription_suite_object_pool_test_testAllocateException;

#include "/home/hobby/dev/FixedContainers/test/inc/allocation_guard_test.h"

static allocation_guard_test suite_allocation_guard_test;

static CxxTest::List Tests_allocation_guard_test = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_allocation_guard_test( "./test/inc/allocation_guard_test.h", 10, "allocation_guard_test", suite_allocation_guard_test, Tests_allocation_guard_test );

static class TestDescription_suite_allocation_guard_test_test_enable_and_disable : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_allocation_guard_test_test_enable_and_disable() : CxxTest::RealTestDescription( Tests_allocation_guard_test, suiteDescription_allocation_guard_test, 14, "test_enable_and_disable" ) {}
 void runTest() { suite_allocation_guard_test.test_enable_and_disable(); }
} testDescription_suite_allocation_guard_test_test_enable_and_disable;

static class TestDescription_suite_allocation_guard_test_test_new_operator : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_allocation_guard_test_test_new_operator() : CxxTest::RealTestDescription( Tests_allocation_guard_test, suiteDescription_allocation_guard_test, 22, "test_new_operator" ) {}
 void runTest() { suite_allocation_guard_test.test_new_operator(); }
} testDescription_suite_allocation_guard_test_test_new_operator;

static class TestDescription_suite_allocation_guard_test_test_new_array_operator : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_allocation_guard_test_test_new_array_operator() : CxxTest::RealTestDescription( Tests_allocation_guard_test, suiteDescription_allocation_guard_test, 34, "test_new_array_operator" ) {}
 void runTest() { suite_allocation_guard_test.test_new_array_operator(); }
} testDescription_suite_allocation_guard_test_test_new_array_operator;

#include "/home/hobby/dev/FixedContainers/test/inc/fixed_array_test.h"

static fixed_array_test suite_fixed_array_test;

static CxxTest::List Tests_fixed_array_test = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_fixed_array_test( "./test/inc/fixed_array_test.h", 26, "fixed_array_test", suite_fixed_array_test, Tests_fixed_array_test );

static class TestDescription_suite_fixed_array_test_test_constructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_constructor() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 30, "test_constructor" ) {}
 void runTest() { suite_fixed_array_test.test_constructor(); }
} testDescription_suite_fixed_array_test_test_constructor;

static class TestDescription_suite_fixed_array_test_test_copy_constructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_copy_constructor() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 40, "test_copy_constructor" ) {}
 void runTest() { suite_fixed_array_test.test_copy_constructor(); }
} testDescription_suite_fixed_array_test_test_copy_constructor;

static class TestDescription_suite_fixed_array_test_test_at : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_at() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 61, "test_at" ) {}
 void runTest() { suite_fixed_array_test.test_at(); }
} testDescription_suite_fixed_array_test_test_at;

static class TestDescription_suite_fixed_array_test_test_at_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_at_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 74, "test_at_const" ) {}
 void runTest() { suite_fixed_array_test.test_at_const(); }
} testDescription_suite_fixed_array_test_test_at_const;

static class TestDescription_suite_fixed_array_test_test_back : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_back() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 88, "test_back" ) {}
 void runTest() { suite_fixed_array_test.test_back(); }
} testDescription_suite_fixed_array_test_test_back;

static class TestDescription_suite_fixed_array_test_test_back_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_back_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 96, "test_back_const" ) {}
 void runTest() { suite_fixed_array_test.test_back_const(); }
} testDescription_suite_fixed_array_test_test_back_const;

static class TestDescription_suite_fixed_array_test_test_begin_and_end : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_begin_and_end() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 104, "test_begin_and_end" ) {}
 void runTest() { suite_fixed_array_test.test_begin_and_end(); }
} testDescription_suite_fixed_array_test_test_begin_and_end;

static class TestDescription_suite_fixed_array_test_test_begin_and_end_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_begin_and_end_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 123, "test_begin_and_end_const" ) {}
 void runTest() { suite_fixed_array_test.test_begin_and_end_const(); }
} testDescription_suite_fixed_array_test_test_begin_and_end_const;

static class TestDescription_suite_fixed_array_test_test_cbegin_and_cend : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_cbegin_and_cend() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 142, "test_cbegin_and_cend" ) {}
 void runTest() { suite_fixed_array_test.test_cbegin_and_cend(); }
} testDescription_suite_fixed_array_test_test_cbegin_and_cend;

static class TestDescription_suite_fixed_array_test_test_data : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_data() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 161, "test_data" ) {}
 void runTest() { suite_fixed_array_test.test_data(); }
} testDescription_suite_fixed_array_test_test_data;

static class TestDescription_suite_fixed_array_test_test_data_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_data_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 168, "test_data_const" ) {}
 void runTest() { suite_fixed_array_test.test_data_const(); }
} testDescription_suite_fixed_array_test_test_data_const;

static class TestDescription_suite_fixed_array_test_test_empty : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_empty() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 175, "test_empty" ) {}
 void runTest() { suite_fixed_array_test.test_empty(); }
} testDescription_suite_fixed_array_test_test_empty;

static class TestDescription_suite_fixed_array_test_test_fill : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_fill() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 184, "test_fill" ) {}
 void runTest() { suite_fixed_array_test.test_fill(); }
} testDescription_suite_fixed_array_test_test_fill;

static class TestDescription_suite_fixed_array_test_test_front : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_front() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 195, "test_front" ) {}
 void runTest() { suite_fixed_array_test.test_front(); }
} testDescription_suite_fixed_array_test_test_front;

static class TestDescription_suite_fixed_array_test_test_front_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_front_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 203, "test_front_const" ) {}
 void runTest() { suite_fixed_array_test.test_front_const(); }
} testDescription_suite_fixed_array_test_test_front_const;

static class TestDescription_suite_fixed_array_test_test_max_size : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_max_size() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 211, "test_max_size" ) {}
 void runTest() { suite_fixed_array_test.test_max_size(); }
} testDescription_suite_fixed_array_test_test_max_size;

static class TestDescription_suite_fixed_array_test_test_ary_operator : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_ary_operator() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 218, "test_ary_operator" ) {}
 void runTest() { suite_fixed_array_test.test_ary_operator(); }
} testDescription_suite_fixed_array_test_test_ary_operator;

static class TestDescription_suite_fixed_array_test_test_ary_operator_const : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_ary_operator_const() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 229, "test_ary_operator_const" ) {}
 void runTest() { suite_fixed_array_test.test_ary_operator_const(); }
} testDescription_suite_fixed_array_test_test_ary_operator_const;

static class TestDescription_suite_fixed_array_test_test_rbegin_and_rend : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_rbegin_and_rend() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 241, "test_rbegin_and_rend" ) {}
 void runTest() { suite_fixed_array_test.test_rbegin_and_rend(); }
} testDescription_suite_fixed_array_test_test_rbegin_and_rend;

static class TestDescription_suite_fixed_array_test_test_size : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_size() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 260, "test_size" ) {}
 void runTest() { suite_fixed_array_test.test_size(); }
} testDescription_suite_fixed_array_test_test_size;

static class TestDescription_suite_fixed_array_test_test_swap : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_fixed_array_test_test_swap() : CxxTest::RealTestDescription( Tests_fixed_array_test, suiteDescription_fixed_array_test, 267, "test_swap" ) {}
 void runTest() { suite_fixed_array_test.test_swap(); }
} testDescription_suite_fixed_array_test_test_swap;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
