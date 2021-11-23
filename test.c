#include <stdio.h>
#include <string.h>

#include "tunion.h"

struct b { int b; };

DEFINE_UNION(my_union,
    ((int) x)
    ((struct b) y)
    ((char *) z)
)

void try_all_three(union my_union m)
{
    int x;
    struct b y;
    char * z;
    if(my_union_get_x(m, &x)) fputs("x case\n", stdout);
    if(my_union_get_y(m, &y)) fputs("y case\n", stdout);
    if(my_union_get_z(m, &z)) fputs("z case\n", stdout);
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
    my_union_set_x(mp++, *xp++);
    if(--mp == ma) fputs("set evaluated union once\n", stdout);
    if(--xp == xa) fputs("set evaluated value once\n", stdout);
    my_union_get_x(*mp++, xp++);
    if(--mp == ma) fputs("get success evaluated union once\n", stdout);
    if(--xp == xa) fputs("get success evaluated value once\n", stdout);
    my_union_set_y(mp, b);
    my_union_get_x(*mp++, xp++);
    if(--mp == ma) fputs("get failure evaluated union once\n", stdout);
    if(--xp == xa) fputs("get failure evaluated value once\n", stdout);
}


int main(int argc, char * * argv)
{
    union my_union m;
    struct b b = {13};
    int x, x2;
    my_union_set_x(&m, argc);
    try_all_three(m);
    my_union_set_y(&m, b);
    try_all_three(m);
    my_union_set_z(&m, *argv);
    try_all_three(m);
    memset(&m, 0, sizeof(union my_union));
    try_all_three(m);
    test_evaluated_only_once(m);
    x = 12345;
    x2 = 2;
    my_union_set_y(&m, b);
    my_union_get_x(m, &x);
    if(x == 12345) fputs("get noop works\n", stdout);
    my_union_set_x(&m, x);
    my_union_get_x(m, &x2);
    if(x == x2) fputs("set get copy works\n", stdout);
    if(x == my_union_set_x(&m, x))
        fputs("set passthrough x\n", stdout);
    if(b.b == my_union_set_y(&m, b).b)
        fputs("set passthrough y\n", stdout);
    if(*argv == my_union_set_z(&m, *argv))
        fputs("set passthrough z\n", stdout);
    return 0;
}
