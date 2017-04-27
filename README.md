# noname

```
$ bison -d -v -y -b noname -p noname_yy noname.y && g++ -lm noname.tab.c -I. -I./include/ -I./src -o noname
```

If you want to debug run this. This generates `noname.output`

```
$ bison -d -v -y -b noname --debug -p noname_yy noname.y && g++ -lm noname.tab.c -I. -I./include/ -I./src -o noname 
```