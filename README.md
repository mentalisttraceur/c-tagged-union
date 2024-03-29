# Tagged Unions for C

Trying to bring us the best tagged unions C can buy.

Less boilerplate to write and read, less room for human error.

About as close as we can get in C to type-safe tagged unions,
with automatic checking at compile-time and run-time.

The implementation was written very carefully to not
stray into unportable or undefined behavior at all.

Performs just as well as a manual implementation when
reasonable compiler optimizations are turned on.


## Usage

The macro `DEFINE_UNION` defines a C `union` type
with a tag and the actual union nested inside it,
and tiny `static` getter and setter functions for
each member. For example, this:

```c
#include "tagged-union.h"

DEFINE_UNION(my_union,
    ((int) foo)
    ((float *) bar)
    ((struct whatever) qux)
)
```

will define all of this:

```c
union my_union {
    struct {
        int tag;
        union {
            int foo;
            float * bar;
            struct whatever qux;
        } value;
    } unsafe;
};

static int my_union_get_foo(union my_union, int *) { /* ... */ }
static int my_union_get_bar(union my_union, float * *) { /* ... */ }
static int my_union_get_qux(union my_union, struct whatever *) { /* ... */ }

static int my_union_set_foo(union my_union *, int) { /* ... */ }
static float * my_union_set_bar(union my_union *, float *) { /* ... */ }
static struct whatever my_union_set_qux(union my_union *, struct whatever) { /* ... */ }
```

The top-level type is `union` rather than `struct` because in the
big picture it still functions as a union of values. The fact
that there is a structure of tag and union value buried in there
is a lower-level implementation detail.

The nested struct member is named `unsafe` to draw attention
when writing and reviewing code that the normal tagged union
safeties are being bypassed. Since this is C, there might be
a good reason to do so, but it should be explicit.

The "set" functions assign a value to a member of the tagged
union, and set the tag value to match that member:

```c
union my_union instance;
my_union_set_foo(instance, 42):
```

The "set" functions also pass the assigned value through
themselves as a return value, so they can be chained and
composed like regular assignment:

```c
#include <assert.h>
assert(my_union_set_foo(instance, 123) == 123);
```

The "get" functions are conditional: if the current tag
matches the member you are trying to get, they write
the value of that member to the pointer you passed in,
and return true; if the tag does not match, they return
false without accessing the member or writing to the
pointer:

```c
int x = 1;
float * f = (float *) 0x2;
my_union_set_foo(instance, 3);
assert(my_union_get_foo(instance, &x));
assert(x == 3);
assert(!my_union_get_bar(instance, &f));
assert(f == (float *) 0x2);
```

This lets you handle all cases of a tagged union like this:

```c
int foo;
float * bar;
struct whatever qux;
if(my_union_get_foo(instance, &foo))
    /* active member is "foo"; handle `foo` ... */
else if(my_union_get_bar(instance, &bar))
    /* ... */
else if(my_union_get_qux(instance, &qux))
    /* ... */
else
    /* union is either nil or corrupted */
```

If a tagged union has been zeroed out, or has its tag set
to zero, for example by `calloc`, `memset`, or setting
`.unsafe.tag = 0` on it, it is "nil": it has no value.
The zero tag is specifically reserved for this purpose:
no member will ever be assigned tag value zero.

Tag values are compile-time constant, start at one
in each union, and count up by one for each member.

The `tagof` macro takes the name of the tagged union
type and the name of one of its members, and expands
to an integer constant expression of type `int`
containing the tag value for that member:

```c
assert(tagof(my_union, foo) == 1);
```


## C Name Collisions

Besides `DEFINE_UNION` and `tagof`, the `tagged-union.h`
header only defines identifiers that start with
`tagged_union_` or `TAGGED_UNION_`.

Besides defining a union type and helper functions
as documented, `DEFINE_UNION` will only ever
define identifiers that start with `tagged_union_`.

`tagged-union.h` contains `#include <stddef.h>`.
