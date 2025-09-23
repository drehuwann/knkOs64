#include "ring3.h"

void test_user_function() {
    const u8 *st = (const u8 *)"welcome to ring 3";
    arg_list *p_arglist =
        arg_push((arg_list *)0, make_arg_vp((void **const)(&st)));
    _BP_
    kprintf((const u8 *)"I just wanna say : %s !", p_arglist);
}
