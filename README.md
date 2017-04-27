# noname language

### build

```
$ make clean
$ make noname
```

### test

```
$ ./noname

>def foo() {
  let aaaaaaaaa1 = 321;
  let aaaaaaaaa2 = 321;
  let aaaaaaaaa3 = 321;
  let aaaaaaaaa4 = 321;
  let aaaaaaaaa5 = 321;
  let aaaaaaaaa6 = 321;
};

>undef
>def baz() {
  let cd2 = 3;
  let my_returned_var = 3.14159;
  return my_returned_var;
};
undef
>def bar() {
  let cd1 = 3;
  foo();
  let cd2 = 3;
};
undef
>bar();
baz();
undef
>3.141590
>def qux_int() {
  let var1 = 3;
  let var2 = 2;
  let my_returned_var = 3 + 2;
  return my_returned_var;
};
qux_int();
undef
>5
>def qux_int_vars() {
  let var1 = 30;
  let var2 = 20;
  let my_returned_var = var1 + var2;
  return my_returned_var;
};
qux_int_vars();
undef
>50
>// function that returns the value two
def ret_two() {
  let out = 2;
  return out;
};
undef
>let PI = 3.143; // 3.143 is the value of PI
let mult = ret_two() * PI;
3.143000
>6.286000
>// function that multiplies the result of ret_two() times PI
def call_func_that_calls_func() {
  let ab = ret_two() * PI;
  return ab;
}

;
undef
>call_func_that_calls_func(); 
6.286000
>def nha_hu() { let ab = 23; return ab; }

;
undef
>nha_hu();
23
>
```