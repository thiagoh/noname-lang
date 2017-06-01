#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <string.h>
#include <string>

typedef struct datatype_t {
  int type;
  void* void_v;
  long nop1;
} datatype_t;
typedef struct datatype_t datatype_t;

datatype_t function_conditional(datatype_t param_a, datatype_t param_b) {
  datatype_t int_value;
  int_value.type = 0;
  int_value.void_v = 0;

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
  var_a.void_v = new long(123);

  datatype_t var_b;
  var_b.type = 13;
  var_b.void_v = new double(321);

  datatype_t datatype_t_out = function_conditional(var_a, var_b);

  datatype_t var_output;
  var_output.type = 333;
  return var_output;
}

int main() {
  datatype_t d = __anon_expr();
  printf("%d", d.type);
  return 0;
}