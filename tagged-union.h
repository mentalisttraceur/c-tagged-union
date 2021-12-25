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
                TAGGED_UNION_MAP(TAGGED_UNION_UNION, name, members) \
            } \
            value; \
        } \
        unsafe; \
    }; \
    struct TAGGED_UNION_CATENATE(tagged_union_tags_, name) \
    { \
        char tagged_union_nil; \
        TAGGED_UNION_MAP(TAGGED_UNION_ENUM, name, members) \
    }; \
    TAGGED_UNION_MAP(TAGGED_UNION_GETTER_AND_SETTER, name, members)

#define TAGGED_UNION_UNION(_, member) TAGGED_UNION_UNWRAP member;

#define TAGGED_UNION_ENUM(_, member) char TAGGED_UNION_DELETE member;

#define TAGGED_UNION_GETTER_AND_SETTER(name, member) \
    static \
    int \
    TAGGED_UNION_SCAN_CATENATE(name##_get_, TAGGED_UNION_DELETE member) \
    (union name instance, TAGGED_UNION_FIRST(member) * value) \
    { \
        if(instance.unsafe.tag != tagof(name, TAGGED_UNION_DELETE member)) \
            return 0; \
        *value = (instance.unsafe.value. TAGGED_UNION_DELETE member); \
        return 1; \
    } \
    static \
    TAGGED_UNION_FIRST(member) \
    TAGGED_UNION_SCAN_CATENATE(name##_set_, TAGGED_UNION_DELETE member) \
    (union name * instance, TAGGED_UNION_FIRST(member) value) \
    { \
        instance->unsafe.tag = tagof(name, TAGGED_UNION_DELETE member); \
        instance->unsafe.value. TAGGED_UNION_DELETE member = value; \
        return value; \
    }

#define TAGGED_UNION_DELETE(x)
#define TAGGED_UNION_UNWRAP(x) x
#define TAGGED_UNION_CATENATE(left, right) left##right
#define TAGGED_UNION_SCAN_CATENATE(left, right) \
    TAGGED_UNION_CATENATE(left, right)
#define TAGGED_UNION_FIRST(sequence) TAGGED_UNION_FIRST_ sequence)
#define TAGGED_UNION_FIRST_(x) x TAGGED_UNION_DELETE(

#define TAGGED_UNION_MAP(macro, parameter, sequence) \
    TAGGED_UNION_SCAN(TAGGED_UNION_SCAN(TAGGED_UNION_SCAN( \
        TAGGED_UNION_DELETE TAGGED_UNION_DELETE \
            TAGGED_UNION_WALK(TAGGED_UNION_OPEN_1 sequence) \
            (macro)(parameter) \
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

#define TAGGED_UNION_STEP(macro_parameter_result, x) \
    macro_parameter_result \
    TAGGED_UNION_FIRST(macro_parameter_result) \
        (TAGGED_UNION_FIRST(TAGGED_UNION_DELETE macro_parameter_result), x)

#define tagof(name, member) \
    (int ) \
    offsetof(struct TAGGED_UNION_CATENATE(tagged_union_tags_, name), member)

#endif /* TAGGED_UNION_H */
