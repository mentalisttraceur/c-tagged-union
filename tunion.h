/* SPDX-License-Identifier: 0BSD */
/* Copyright 2021 Alexander Kozhevnikov <mentalisttraceur@gmail.com> */

#include <stddef.h> /* size_t */
#include <string.h> /* memcpy */

#define DECLARE_UNION(name, members) \
union name \
{ \
    struct \
    { \
        union \
        { \
            int tag_space; \
            enum \
            { \
                TUNION_WALK(TUNION_ENUM_0 members) \
            } tag_enum; \
        } tag; \
        union \
        { \
            TUNION_WALK(TUNION_UNION_1 members) \
        } type; \
    } tunion; \
}
#define TUNION_CATENATE(left, right) left##right
#define TUNION_WALK(macro_and_members) TUNION_CATENATE(macro_and_members, _END)
#define TUNION_ENUM_0(member) TUNION_DELETE_TYPE member TUNION_ENUM_1
#define TUNION_ENUM_1(member) \
    TUNION_COMMA TUNION_DELETE_TYPE member TUNION_ENUM_2
#define TUNION_ENUM_2(member) \
    TUNION_COMMA TUNION_DELETE_TYPE member TUNION_ENUM_1
#define TUNION_ENUM_1_END
#define TUNION_ENUM_2_END
#define TUNION_UNION_1(member) TUNION_UNWRAP_TYPE member; TUNION_UNION_2
#define TUNION_UNION_2(member) TUNION_UNWRAP_TYPE member; TUNION_UNION_1
#define TUNION_UNION_1_END
#define TUNION_UNION_2_END
#define TUNION_UNWRAP_TYPE(type) type
#define TUNION_DELETE_TYPE(type)
#define TUNION_COMMA TUNION_COMMA_ TUNION_EMPTY() ()
#define TUNION_COMMA_() ,
#define TUNION_EMPTY()

static
void tunion_set(void * instance, int tag, void * source, size_t size)
{
    memcpy(instance, &tag, sizeof(int));
    memcpy((unsigned char * )instance + sizeof(int), source, size);
}
#define UNION_SET(instance, member, pointer) tunion_set( \
    &(instance), \
    (member), \
    (1 ? (pointer) : &((instance).tunion.type.member)), \
    sizeof(*(pointer)) \
)
static
int tunion_get(void * instance, int tag, void * destination, size_t size)
{
    int tag_;
    memcpy(&tag_, instance, sizeof(int));
    if(tag_ != tag)
    {
        return 0;
    }
    memcpy(destination, (unsigned char * )instance + sizeof(int), size);
    return 1;
}
#define UNION_GET(instance, member, pointer) tunion_get( \
    &(instance), \
    (member), \
    (1 ? (pointer) : &((instance).tunion.type.member)), \
    sizeof(*(pointer)) \
)
