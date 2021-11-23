/* SPDX-License-Identifier: 0BSD */
/* Copyright 2021 Alexander Kozhevnikov <mentalisttraceur@gmail.com> */

#ifndef TUNION_H
#define TUNION_H

#include <stddef.h> /* offsetof */

#define DEFINE_UNION(name, members) \
union name \
{ \
    struct \
    { \
        int tag; \
        union \
        { \
            TUNION_REDUCE((TUNION_UNION)(name), members) \
        } \
        value; \
    } \
    unsafe; \
}; \
struct tunion_enum_##name \
{ \
    TUNION_REDUCE((TUNION_ENUM)(name)char tunion_nil;, members) \
}; \
TUNION_REDUCE((TUNION_GETTER_AND_SETTER)(name), members)

#define TUNION_ENUM(_, member) char TUNION_DELETE member;

#define TUNION_UNION(_, member) TUNION_UNWRAP member;

#define TUNION_GETTER_AND_SETTER(name, member) \
    static \
    int TUNION_METHOD(name, _get_, member) \
    (union name instance, TUNION_FIRST(member) * value) \
    { \
        if(instance.unsafe.tag != TUNION_TAG(name, member)) \
            return 0; \
        *value = (instance.unsafe.value. TUNION_DELETE member); \
        return 1; \
    } \
    static \
    TUNION_FIRST(member) TUNION_METHOD(name, _set_, member) \
    (union name * instance, TUNION_FIRST(member) value) \
    { \
        instance->unsafe.tag = TUNION_TAG(name, member); \
        instance->unsafe.value. TUNION_DELETE member = value; \
        return value; \
    }

#define TUNION_METHOD(name, _get_or_set_, member) \
    TUNION_CATENATE TUNION_UNWRAP((name##_get_or_set_, TUNION_DELETE member))
#define TUNION_TAG(name, member) \
    offsetof(struct tunion_enum_##name, TUNION_DELETE member)

#define TUNION_DELETE(x)
#define TUNION_UNWRAP(x) x
#define TUNION_CATENATE(left, right) left##right
#define TUNION_FIRST(sequence) TUNION_FIRST_ sequence)
#define TUNION_FIRST_(x) x TUNION_DELETE(

#define TUNION_REDUCE(macro_state_result, sequence) \
    TUNION_SCAN(TUNION_SCAN(TUNION_SCAN(TUNION_DELETE TUNION_DELETE \
        TUNION_WALK(TUNION_OPEN_1 sequence) \
        macro_state_result \
        TUNION_WALK(TUNION_CLOSE_1 sequence) \
    )))
#define TUNION_SCAN(x) x

#define TUNION_WALK(macro_sequence) TUNION_CATENATE(macro_sequence, _END)

#define TUNION_OPEN_1(x) \
    TUNION_STEP TUNION_OPEN TUNION_UNWRAP(()) TUNION_OPEN_2
#define TUNION_OPEN_2(x) \
    TUNION_STEP TUNION_OPEN TUNION_UNWRAP(()) TUNION_OPEN_1
#define TUNION_OPEN_1_END
#define TUNION_OPEN_2_END
#define TUNION_OPEN() (

#define TUNION_CLOSE_1(x) TUNION_CLOSE TUNION_UNWRAP((x)) TUNION_CLOSE_2
#define TUNION_CLOSE_2(x) TUNION_CLOSE TUNION_UNWRAP((x)) TUNION_CLOSE_1
#define TUNION_CLOSE_1_END
#define TUNION_CLOSE_2_END
#define TUNION_CLOSE(x) , x)

#define TUNION_STEP(macro_state_result, x) \
    (TUNION_FIRST(macro_state_result)) \
    (TUNION_FIRST(TUNION_DELETE macro_state_result)) \
    TUNION_DELETE TUNION_DELETE macro_state_result \
    TUNION_FIRST(macro_state_result) \
        (TUNION_FIRST(TUNION_DELETE macro_state_result), x)

#endif /* TUNION_H */
