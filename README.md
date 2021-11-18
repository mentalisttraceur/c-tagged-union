# Tagged Unions for C

Trying to bring us the best tagged unions C can buy.

About as close as we can get in C to type-safe tagged unions,
with automatic checking at compile-time and run-time.

Less boilerplate to write and read, less room for human error.

The implementation was written very carefully to not
stray into unportable or undefined behavior at all.

With optimizations turned up high enough on good modern
compilers, the performance should be just as good as a
typical manual implementation with C enums and unions
in most if not all cases.


## Versioning

This library follows the SemVer 2.0.0 specification.

(The current version is `1.0.0-alpha` - a pre-release of
1.0.0, and as such not yet set in stone, but if no flaws
are found, this will become the actual `1.0.0` release.)


## API

For code examples, look at the `test.c` file
in this repo.

The macro `DECLARE_UNION` expands to a C `union`
type declaration, and should be valid anywhere
a union declaration is valid, but bear in mind
that each tagged union's member names are also
`enum` constants, so multiple definitions with
the same member names in the same scope will be
rejected by the compiler.

The macro `UNION_SET` assigns a value to a member
of a tagged union, and sets the tag to match.

The macro `UNION_GET` accesses the value of a
member of a tagged union by writing it to the
given value pointer if the tag matches. It
expands to a boolean result which reports
if the match was successful.

Both `UNION_SET` and `UNION_GET` evaluate each
of their argumments exactly once.

Both `UNION_SET` and `UNION_GET` are written to
maximize the chances of a compile-time error or
warning if the value being assigned is not a
compatible type for that union member type.

Accessing any members on the tagged union directly
through normal C syntax is *not* supported. Tagged
unions are defined to make the naive way of trying
this simply not compile, and the members that are
accessible on the union with C syntax are private
implementation details which are subject to change
and are not even guaranteed to be aligned with how
the macros store data in the union on all systems.



## C Name Collisions

Besides the macros documented as part of the API,
the `tunion.h` header will only ever define
identifiers whose first six characters are
`tunion` or `TUNION`, all of which are private
and not intended for external use.
