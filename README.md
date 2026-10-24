A simple interpreted language with an assembly-like syntax

## Build
Dependencies:\
[glib-2.0](https://docs.gtk.org/glib/index.html)
~~~sh
make
~~~

## Usage
~~~sh
./lang [FILE].dyn
~~~
Code example:
```
let a b sum
set a 30
set b 70
set sum a+b
print sum
```
~~~
Output: 100
~~~