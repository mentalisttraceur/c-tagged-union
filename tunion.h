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
        TUNION_WALK(TUNION_UNION_1 members) \
    } unsafe; \
    int:(0 * sizeof( \
        enum \
        { \
            TUNION_WALK(TUNION_ENUM_0 members) \
        } \
    )); \
}
#define TUNION_CATENATE(left, right) left##right
#define TUNION_WALK(macro_and_members) TUNION_CATENATE(macro_and_members, _END)
#define TUNION_UNION_1(member) TUNION_UNION(member) TUNION_UNION_2
#define TUNION_UNION_2(member) TUNION_UNION(member) TUNION_UNION_1
#define TUNION_UNION_1_END
#define TUNION_UNION_2_END
#define TUNION_ENUM_0(member) TUNION_ENUM(member) TUNION_ENUM_1
#define TUNION_ENUM_1(member) TUNION_COMMA TUNION_ENUM(member) TUNION_ENUM_2
#define TUNION_ENUM_2(member) TUNION_COMMA TUNION_ENUM(member) TUNION_ENUM_1
#define TUNION_ENUM_1_END
#define TUNION_ENUM_2_END
#define TUNION_UNION(member) \
    struct TUNION_MEMBER TUNION_EMPTY (TUNION_DELETE_TYPE member) \
    { \
        int tag; \
        TUNION_UNWRAP_TYPE member; \
    } TUNION_DELETE_TYPE member;
#define TUNION_ENUM(member) TUNION_DELETE_TYPE member
#define TUNION_UNWRAP_TYPE(type) type
#define TUNION_DELETE_TYPE(type)
#define TUNION_COMMA TUNION_COMMA_ TUNION_EMPTY ()
#define TUNION_COMMA_() ,
#define TUNION_EMPTY
#define TUNION_MEMBER(name) tunion_member_##name

static
void * tunion_set_tag(void * instance, int tag)
{
    return memcpy(instance, &tag, sizeof(int));
}
#define UNION_SET(instance, member, value) \
( \
    ( \
        (struct tunion_member_##member * ) \
            tunion_set_tag(&((instance).unsafe.member), (member)) \
    )->member = (value) \
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
    (member), \
    (1 ? (pointer) : &((instance).unsafe.member.member)), \
    sizeof(*(pointer)), \
    offsetof(struct tunion_member_##member, member) \
)

#endif /* TUNION_H */
