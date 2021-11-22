/* SPDX-License-Identifier: 0BSD */
/* Copyright 2021 Alexander Kozhevnikov <mentalisttraceur@gmail.com> */

#ifndef TUNION_H
#define TUNION_H

#include <stddef.h> /* offsetof, size_t */
#include <string.h> /* memcpy */

#define DECLARE_UNION(name, members) \
union name \
{ \
    union \
    { \
        TUNION_REDUCE(TUNION_UNION, name, members) \
    } unsafe; \
    int:(0 * sizeof(union { \
        struct tunion_enum_##name \
        { \
            TUNION_REDUCE(TUNION_ENUM, name, members) \
        } name; \
        TUNION_REDUCE(TUNION_ALIAS, name, members) \
    })); \
}

#define TUNION_UNION(_, member) \
    TUNION_MEMBER_STRUCT(TUNION_DELETE member) \
    { \
        int tag; \
        TUNION_UNWRAP member; \
    } TUNION_DELETE member;
#define TUNION_MEMBER_STRUCT(name) struct TUNION_CATENATE(tunion_member_, name)

#define TUNION_ENUM(_, member) char TUNION_DELETE member;

#define TUNION_ALIAS(name, member) \
    TUNION_ENUM_STRUCT(TUNION_DELETE member) \
    { \
        TUNION_ENUM_STRUCT(name) tunion; \
    } TUNION_DELETE member;
#define TUNION_ENUM_STRUCT(name) struct TUNION_CATENATE(tunion_enum_, name)

#define TUNION_DELETE(x)
#define TUNION_UNWRAP(x) x
#define TUNION_CATENATE(left, right) left##right

#define TUNION_REDUCE(macro, state, sequence) \
    TUNION_TRIM( \
        TUNION_WALK(TUNION_OPEN_1 sequence) \
        ()(macro)state \
        TUNION_WALK(TUNION_CLOSE_1 sequence) \
    )

#define TUNION_TRIM(result_macro_state) TUNION_FIRST(result_macro_state)
#define TUNION_FIRST(sequence) TUNION_FIRST_ sequence)
#define TUNION_FIRST_(x) x TUNION_DELETE(

#define TUNION_WALK(macro_sequence) TUNION_CATENATE(macro_sequence, _END)

#define TUNION_OPEN_1(x) \
    TUNION_STEP TUNION_OPEN TUNION_UNWRAP() () TUNION_OPEN_2
#define TUNION_OPEN_2(x) \
    TUNION_STEP TUNION_OPEN TUNION_UNWRAP() () TUNION_OPEN_1
#define TUNION_OPEN_1_END
#define TUNION_OPEN_2_END
#define TUNION_OPEN() (

#define TUNION_CLOSE_1(x) TUNION_CLOSE TUNION_UNWRAP() (x) TUNION_CLOSE_2
#define TUNION_CLOSE_2(x) TUNION_CLOSE TUNION_UNWRAP() (x) TUNION_CLOSE_1
#define TUNION_CLOSE_1_END
#define TUNION_CLOSE_2_END
#define TUNION_CLOSE(x) , x)

#define TUNION_STEP(result_macro_state, x) \
    ( \
        TUNION_FIRST(result_macro_state) \
        TUNION_FIRST(TUNION_DELETE result_macro_state) \
        (TUNION_DELETE TUNION_DELETE result_macro_state, x) \
    ) \
    (TUNION_FIRST(TUNION_DELETE result_macro_state)) \
    TUNION_DELETE TUNION_DELETE result_macro_state


static
void * tunion_set_tag(void * instance, int tag)
{
    return memcpy(instance, &tag, sizeof(int));
}
#define UNION_SET(instance, member, value) \
( \
    ((struct tunion_member_##member * ) tunion_set_tag( \
        &((instance).unsafe.member), \
        offsetof(struct tunion_enum_##member, tunion.member) \
    ))->member = (value) \
)
static
int tunion_get(
    void * instance, int tag, void * destination, size_t size, size_t offset
)
{
    int tag_;
    memcpy(&tag_, instance, sizeof(int));
    if(tag_ != tag)
    {
        return 0;
    }
    memcpy(destination, (unsigned char * )instance + offset, size);
    return 1;
}
#define UNION_GET(instance, member, pointer) tunion_get( \
    &(instance), \
    offsetof(struct tunion_enum_##member, tunion.member), \
    (1 ? (pointer) : &((instance).unsafe.member.member)), \
    sizeof(*(pointer)), \
    offsetof(struct tunion_member_##member, member) \
)

#endif /* TUNION_H */
