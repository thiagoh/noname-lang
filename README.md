# noname language

### build

```
$ make clean
$ make noname
```

### test

```
$ ./noname
>
def foo() {
  let aaaaaaaaa1 = 321;
  let aaaaaaaaa2 = 321;
  let aaaaaaaaa3 = 321;
  let aaaaaaaaa4 = 321;
  let aaaaaaaaa5 = 321;
  let aaaaaaaaa6 = 321;
};

def baz() {
  let cd2 = 3;
  let my_returned_var = 3.14159;
  return my_returned_var;
};

def bar() {
  let cd1 = 3;
  foo();
  let cd2 = 3;
};

baz();
```