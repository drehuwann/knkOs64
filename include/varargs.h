#pragma once

#include "typedefs.h"
#include "heap.h"

typedef struct {
    u64 size;
    void *arg_addr;
} argument;

argument *make_arg_ub(u8 *const arg);
argument *make_arg_sb(s8 *const arg);
argument *make_arg_uw(u16 *const arg);
argument *make_arg_sw(s16 *const arg);
argument *make_arg_ud(u32 *const arg);
argument *make_arg_sd(s32 *const arg);
argument *make_arg_uq(u64 *const arg);
argument *make_arg_sq(s64 *const arg);
argument *make_arg_bl(bool *const arg);
argument *make_arg_vp(void **const arg);

typedef struct arg_list {
    argument *p_arg;
    struct arg_list *next;
} arg_list;

/// @brief pops out pointer on first element of list.
/// @param p_list address of FIFO *arg_list. On success, list points on new head
/// @return pointer on popped argument, or nullptr on failure.
argument *arg_pop(arg_list **p_list);

/// @brief pushes arg at the end of the list.
/// @param list address of FIFO arg_list (if null, head of list is allocated.)
/// @param arg pointer on argument to push.
/// @return listptr (or new allocated listptr) on Success. on Failure : nullptr
arg_list *arg_push(arg_list *list, argument *const arg);

/// @brief recursively pops args in list until list is freed.
/// @warning ### Dangerous !!! ### 
/// @param args the head of the list to be freed. 
void arg_flush(arg_list *args);