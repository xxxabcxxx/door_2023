#ifndef STACK_H_
#define STACK_H_

#include <stdint.h>

// #ifdef __cplusplus
// extern "C" {
// #endif // __cplusplus

#include "rtthread.h"

    static inline void StackFree(void* ptr)
    {
        rt_free(ptr);
    }

    static inline void* StackMalloc(uint64_t size)
    {
        return rt_malloc(size);
    }

    typedef struct Stack
    {
        uint32_t stack_max_size;
        uint32_t stack_remain_size;
        void** sp;
        void** stack_buf;
    }Stack;

    static inline void Push(Stack* stack, void* stack_obj)
    {
        if (stack->stack_remain_size == 0)
            return;
        --stack->stack_remain_size;
        stack->sp++;
        *(stack->sp) = stack_obj;
    }

    static inline void Pop(Stack* stack)
    {
        if (stack->stack_remain_size == stack->stack_max_size)
            return;
        ++stack->stack_remain_size;
        stack->sp--;
    }

    static inline void* StackInit(Stack* stack, uint32_t stack_max_size)
    {
        stack->stack_max_size = stack->stack_remain_size = stack_max_size;
        return stack->sp = stack->stack_buf = (void**)StackMalloc(stack_max_size * sizeof(void*));
    }

    static inline Stack* StackCreate(uint32_t stack_max_size)
    {
        Stack* stack = (Stack*)StackMalloc(sizeof(Stack));
        if (stack == NULL)
        {
            return NULL;
        }
        if (StackInit(stack, stack_max_size))
            return stack;
        else
        {
            StackFree(stack);
            return NULL;
        }
    }

// #ifdef __cplusplus
// }
// #endif // __cplusplus

#endif // !STACK_H_
