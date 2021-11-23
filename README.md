# Tagged Unions for C

Trying to bring us the best tagged unions C can buy.

About as close as we can get in C to type-safe tagged unions,
with automatic checking at compile-time and run-time.

Less boilerplate to write and read, less room for human error.

The implementation was written very carefully to not
stray into unportable or undefined behavior at all.

Performs just as good as a manual implementation
with reasonable compiler optimizations turned on.


## Versioning

This library follows the SemVer 2.0.0 specification.

(The current version is `1.0.0-beta` - a pre-release of
1.0.0, and as such not yet set in stone, but if no flaws
are found, this will become the actual `1.0.0` release.)


## Usage

The macro `DEFINE_UNION` defines a C `union` type
with a tag and the actual union nested inside it,
and tiny `static` getter and setter functions for
each member. For example, this:

```c
#include <tunion.h>

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

static int my_union_set_foo(union * my_union, int) { /* ... */ }
static float * my_union_set_bar(union * my_union, float *) { /* ... */ }
static struct whatever my_union_set_qux(union * my_union, struct whatever) { /* ... */ }
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
false without trying to write to the pointer:

```c
my_union_set_foo(instance, 1);
int x;
float * f;
assert(my_union_get_foo(instance, &x));
assert(!my_union_get_bar(instance, &f));
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
`.unsafe.tag = 0` on it, it is "nil": it has no value and
all of the getter functions for that union will safely
return false. (The getters will safely return false for
any tag value that does not match a union member, but the
zero tag is specifically reserved for this purpose, so no
valid member tag will ever have the value zero.)

Tag values are compile-time constant, non-zero, start
at one for the first member of a union, and count up
by one for each member.


## C Name Collisions

Besides `DEFINE_UNION`, the `tunion.h` header only
defines identifiers whose first six characters are
`tunion` or `TUNION`, which are all private and
subject to change.

Besides defining a union type and helper functions
as documented, `DEFINE_UNION` will only ever
define identifiers that start with `tunion`.
