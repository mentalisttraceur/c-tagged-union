/* SPDX-License-Identifier: 0BSD */
/* Copyright 2021 Alexander Kozhevnikov <mentalisttraceur@gmail.com> */

#ifndef TAGGED_UNION_H
#define TAGGED_UNION_H

#include <stddef.h> /* offsetof */

#define DEFINE_UNION(name, members) \
union name \
{ \
    struct \
    { \
        int tag; \
        union \
        { \
            TAGGED_UNION_REDUCE((TAGGED_UNION_UNION)(name), members) \
        } \
        value; \
    } \
    unsafe; \
}; \
struct tagged_union_tags_##name \
{ \
    TAGGED_UNION_REDUCE( \
        (TAGGED_UNION_ENUM)(name)char tagged_union_nil;, members \
    ) \
}; \
TAGGED_UNION_REDUCE((TAGGED_UNION_GETTER_AND_SETTER)(name), members)

#define TAGGED_UNION_ENUM(_, member) char TAGGED_UNION_DELETE member;

#define TAGGED_UNION_UNION(_, member) TAGGED_UNION_UNWRAP member;

#define TAGGED_UNION_GETTER_AND_SETTER(name, member) \
    static \
    int TAGGED_UNION_METHOD(name, _get_, member) \
    (union name instance, TAGGED_UNION_FIRST(member) * value) \
    { \
        if(instance.unsafe.tag != TAGGED_UNION_TAG(name, member)) \
            return 0; \
        *value = (instance.unsafe.value. TAGGED_UNION_DELETE member); \
        return 1; \
    } \
    static \
    TAGGED_UNION_FIRST(member) TAGGED_UNION_METHOD(name, _set_, member) \
    (union name * instance, TAGGED_UNION_FIRST(member) value) \
    { \
        instance->unsafe.tag = TAGGED_UNION_TAG(name, member); \
        instance->unsafe.value. TAGGED_UNION_DELETE member = value; \
        return value; \
    }

#define TAGGED_UNION_METHOD(name, _get_or_set_, member) \
    TAGGED_UNION_CATENATE TAGGED_UNION_UNWRAP( \
        (name##_get_or_set_, TAGGED_UNION_DELETE member) \
    )
#define TAGGED_UNION_TAG(name, member) \
    (int )offsetof(struct tagged_union_tags_##name, TAGGED_UNION_DELETE member)

#define TAGGED_UNION_DELETE(x)
#define TAGGED_UNION_UNWRAP(x) x
#define TAGGED_UNION_CATENATE(left, right) left##right
#define TAGGED_UNION_FIRST(sequence) TAGGED_UNION_FIRST_ sequence)
#define TAGGED_UNION_FIRST_(x) x TAGGED_UNION_DELETE(

#define TAGGED_UNION_REDUCE(macro_state_result, sequence) \
    TAGGED_UNION_SCAN(TAGGED_UNION_SCAN(TAGGED_UNION_SCAN( \
        TAGGED_UNION_DELETE TAGGED_UNION_DELETE \
            TAGGED_UNION_WALK(TAGGED_UNION_OPEN_1 sequence) \
            macro_state_result \
            TAGGED_UNION_WALK(TAGGED_UNION_CLOSE_1 sequence) \
    )))
#define TAGGED_UNION_SCAN(x) x

#define TAGGED_UNION_WALK(macro_sequence) \
    TAGGED_UNION_CATENATE(macro_sequence, _END)

#define TAGGED_UNION_OPEN_1(x) \
    TAGGED_UNION_STEP TAGGED_UNION_OPEN TAGGED_UNION_UNWRAP(()) \
    TAGGED_UNION_OPEN_2
#define TAGGED_UNION_OPEN_2(x) \
    TAGGED_UNION_STEP TAGGED_UNION_OPEN TAGGED_UNION_UNWRAP(()) \
    TAGGED_UNION_OPEN_1
#define TAGGED_UNION_OPEN_1_END
#define TAGGED_UNION_OPEN_2_END
#define TAGGED_UNION_OPEN() (

#define TAGGED_UNION_CLOSE_1(x) \
    TAGGED_UNION_CLOSE TAGGED_UNION_UNWRAP((x)) TAGGED_UNION_CLOSE_2
#define TAGGED_UNION_CLOSE_2(x) \
    TAGGED_UNION_CLOSE TAGGED_UNION_UNWRAP((x)) TAGGED_UNION_CLOSE_1
#define TAGGED_UNION_CLOSE_1_END
#define TAGGED_UNION_CLOSE_2_END
#define TAGGED_UNION_CLOSE(x) , x)

#define TAGGED_UNION_STEP(macro_state_result, x) \
    (TAGGED_UNION_FIRST(macro_state_result)) \
    (TAGGED_UNION_FIRST(TAGGED_UNION_DELETE macro_state_result)) \
    TAGGED_UNION_DELETE TAGGED_UNION_DELETE macro_state_result \
    TAGGED_UNION_FIRST(macro_state_result) \
        (TAGGED_UNION_FIRST(TAGGED_UNION_DELETE macro_state_result), x)

#define tagof(name, member) TAGGED_UNION_TAG(name, ()member)

#endif /* TAGGED_UNION_H */
