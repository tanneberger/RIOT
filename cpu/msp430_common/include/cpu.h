/*
 * Copyright (C) 2014, Freie Universitaet Berlin (FUB) & INRIA.
 * All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_msp430_common TI MSP430
 * @ingroup     cpu
 * @brief       Texas Instruments MSP430 specific code
 *
 * @{
 * @file
 * @brief       Texas Instruments MSP430 specific code
 *
 */

#ifndef CPU_H
#define CPU_H

#include <stdio.h>

#include <msp430.h>
#include "board.h"

#include "sched.h"
#include "thread.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wordsize in bit for MSP430 platforms
 */
#define WORDSIZE 16

/**
 * @brief   Macro for defining interrupt service routines
 */
#define ISR(a,b)        void __attribute__((naked, interrupt (a))) b(void)

/**
 * @brief   The current ISR state (inside or not)
 */
extern volatile int __irq_is_in;

/**
 * @brief   Save the current thread context from inside an ISR
 */
static inline void __attribute__((always_inline)) __save_context(void)
{
    __asm__("push r15");
    __asm__("push r14");
    __asm__("push r13");
    __asm__("push r12");
    __asm__("push r11");
    __asm__("push r10");
    __asm__("push r9");
    __asm__("push r8");
    __asm__("push r7");
    __asm__("push r6");
    __asm__("push r5");
    __asm__("push r4");

    __asm__("mov.w r1,%0" : "=r"(thread_get_active()->sp));
}

/**
 * @brief   Restore the thread context from inside an ISR
 */
static inline void __attribute__((always_inline)) __restore_context(void)
{
    __asm__("mov.w %0,r1" : : "m"(thread_get_active()->sp));

    __asm__("pop r4");
    __asm__("pop r5");
    __asm__("pop r6");
    __asm__("pop r7");
    __asm__("pop r8");
    __asm__("pop r9");
    __asm__("pop r10");
    __asm__("pop r11");
    __asm__("pop r12");
    __asm__("pop r13");
    __asm__("pop r14");
    __asm__("pop r15");
    __asm__("reti");
}

/**
 * @brief   Run this code on entering interrupt routines
 */
static inline void __attribute__((always_inline)) __enter_isr(void)
{
    extern char __stack;    /* defined by linker script to end of RAM */
    __save_context();
    __asm__("mov.w %0,r1" : : "i"(&__stack));
    __irq_is_in = 1;
}

/**
 * @brief   Run this code on exiting interrupt routines
 */
static inline void __attribute__((always_inline)) __exit_isr(void)
{
    __irq_is_in = 0;

    if (sched_context_switch_request) {
        sched_run();
    }

    __restore_context();
}

/**
 * @brief   Returns the last instruction's address
 *
 * @todo:   Not supported
 */
static inline uintptr_t cpu_get_caller_pc(void)
{
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
