#include "varargs.h"

argument *arg_pop(arg_list **p_list) {
    argument *toRet = (argument *)0;
    if (p_list) {
        arg_list *p_arg2pop = *p_list;
        if (p_arg2pop) {
            toRet = p_arg2pop->p_arg;
            *p_list = p_arg2pop->next;
            free(p_arg2pop); //was allocated by arg_push
        }
    }
    return toRet;
}

arg_list *arg_push(arg_list *list, argument *const arg) {
    arg_list *p_list = list;
    arg_list *newarg = (arg_list *)malloc(sizeof(arg_list));
    //newarg to be freed by arg_pop
    if (newarg) {
        newarg->next = (arg_list *)0;
        newarg->p_arg = arg; 
    } else {
        asm volatile("int $0x0d"); //#GP fault
    }
    if (!p_list) return newarg;

    while(p_list->next) p_list = p_list->next; // go to the last element.
    p_list->next = newarg;

    return list;
}

void arg_flush(arg_list *args) {
    while (arg_pop(&args)) { 
        //WARN : Leave this Scope empty !! except for DEBUG messages ;)
    }
}

argument *make_arg_ub(u8 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_sb(s8 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_uw(u16 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_sw(s16 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_ud(u32 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_sd(s32 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_uq(u64 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_sq(s64 *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_bl(bool *const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}

argument *make_arg_vp(void **const arg) {
    argument *toRet = malloc(sizeof(argument));
    if (toRet) {
        toRet->arg_addr = (void *)arg;
        toRet->size = (u64)sizeof(*arg);
    }
    return toRet;
}
