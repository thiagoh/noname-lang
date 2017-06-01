#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <string.h>
#include <string>

typedef struct datatype_t {
  int type;
  void* void_v;
  long long_v;
  double double_v;
  int int_v;
  float float_v;
} datatype_t;
typedef struct datatype_t datatype_t;

datatype_t function_return_a(datatype_t param_a, datatype_t param_b) {
  int i1 = 2;
  int i2 = 2;
  if (i1 > i2) {
    return param_b;
  }

  int a = 3 + 4;

  return param_b;
}

datatype_t function_return_copy_a_void_v(datatype_t param_a,
                                         datatype_t param_b) {
  datatype_t out;
  out.void_v = param_a.void_v;

  return out;
}
datatype_t function_return_full_copy(datatype_t param_a, datatype_t param_b) {
  datatype_t out = param_a;
  return out;
}
datatype_t function_conditional(datatype_t param_a) {
  datatype_t int_value2;
  datatype_t int_value;
  int_value.type = 0;
  int_value.void_v = 0;
  int_value.int_v = 0;
  int_value.long_v = 0;
  int_value.double_v = 0;
  int_value.float_v = 0;

  if (param_a.type == 12) {
    long actual_long_v = *(long*)param_a.void_v + 777;
    int_value.void_v = new long(actual_long_v);
    // int_value.long_v = void_v;
  } else if (param_a.type == 13) {
    double actual_double_v = *(double*)param_a.void_v + 777;
    int_value.void_v = new double(actual_double_v);
    // int_value.double_v = void_v;
  }

  return int_value;
}

datatype_t __anon_expr() {
  datatype_t var_a;
  var_a.type = 12;
  var_a.void_v = new long(321);
  datatype_t var_b;
  var_b.type = 12;
  var_b.void_v = new long(123);

  // return function_conditional(var_a, var_b);
  // return function_conditional(var_a);
  datatype_t datatype_t_out = function_return_a(var_a, var_b);

  return var_a;
}

int main() {
  datatype_t d = __anon_expr();
  printf("%d", d.type);

  void* pointer_to_long_333 = new long(333);

  datatype_t data1;
  data1.type = 444;
  data1.void_v = pointer_to_long_333;

  function_return_copy_a_void_v(data1, data1);
  return 0;
}