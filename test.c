#include <stdio.h>

#include "tunion.h"

struct b { int b; };

DECLARE_UNION(my_union,
    ((int) x)
    ((struct b) y)
    ((char *) z)
);

void try_all_three(union my_union m)
{
    int x;
    struct b y;
    char * z;
    if(UNION_GET(m, x, &x)) fputs("x case\n", stdout);
    if(UNION_GET(m, y, &y)) fputs("y case\n", stdout);
    if(UNION_GET(m, z, &z)) fputs("z case\n", stdout);
}

void test_evaluated_only_once(union my_union m)
{
    int xa[4096];
    union my_union ma[4096];
    int * xp = xa;
    union my_union * mp = ma;
    struct b b = {13};
    *xa = 0;
    *ma = m;
    UNION_SET(*mp++, x, *xp++);
    if(--mp == ma) fputs("set evaluated union once\n", stdout);
    if(--xp == xa) fputs("set evaluated value once\n", stdout);
    UNION_GET(*mp++, x, xp++);
    if(--mp == ma) fputs("get success evaluated union once\n", stdout);
    if(--xp == xa) fputs("get success evaluated value once\n", stdout);
    UNION_SET(*mp, y, b);
    UNION_GET(*mp++, x, xp++);
    if(--mp == ma) fputs("get failure evaluated union once\n", stdout);
    if(--xp == xa) fputs("get failure evaluated value once\n", stdout);
}


int main(int argc, char * * argv)
{
    union my_union m;
    struct b b = {13};
    int x, y;
    UNION_SET(m, x, argc);
    try_all_three(m);
    UNION_SET(m, y, b);
    try_all_three(m);
    UNION_SET(m, z, *argv);
    try_all_three(m);
    test_evaluated_only_once(m);
    x = 12345;
    y = 2;
    UNION_SET(m, y, b);
    UNION_GET(m, x, &x);
    if(x == 12345) fputs("get noop works\n", stdout);
    UNION_SET(m, x, x);
    UNION_GET(m, x, &y);
    if(x == y) fputs("set get copy works\n", stdout);
    if(x == UNION_SET(m, x, x))
        fputs("set passthrough x\n", stdout);
    if(b.b == UNION_SET(m, y, b).b)
        fputs("set passthrough y\n", stdout);
    if(*argv == UNION_SET(m, z, *argv))
        fputs("set passthrough z\n", stdout);
    return 0;
}
