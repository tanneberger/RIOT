/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Blinky application
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "reactor-uc/environment.h"
#include "reactor-uc/reaction.h"
#include "reactor-uc/reactor.h"
#include "reactor-uc/timer.h"

#define UNUSED(x) (void)(x)

typedef struct {
  Timer super;
  Reaction *effects[1];
} MyTimer;

typedef struct {
  Reaction super;
} MyReaction;

struct MyReactor {
  Reactor super;
  MyReaction my_reaction;
  MyTimer timer;
  Reaction *_reactions[1];
  Trigger *_triggers[1];
} __attribute__ ((aligned (32)));

int timer_handler(Reaction *self) {
  UNUSED(self);
  //struct MyReactor *self = (struct MyReactor *)_self->parent;
  //char pid[40];
  //sprintf(pid, "Toggling LED: %lli\n", self->parent->env->current_tag.time);
  //puts(pid);

  puts("dasdasads");

#ifdef LED0_TOGGLE
        LED0_TOGGLE;
#else
        puts("Blink! (No LED present or configured...)");
#endif
  return 0;
}

void MyReaction_ctor(MyReaction *self, Reactor *parent) {
  Reaction_ctor(&self->super, parent, timer_handler, NULL, 0);
  self->super.level = 0;
}

void MyReactor_ctor(struct MyReactor *self, Environment *env) {
  self->_reactions[0] = (Reaction *)&self->my_reaction;
  self->_triggers[0] = (Trigger *)&self->timer;
  Reactor_ctor(&self->super, env, NULL, 0, self->_reactions, 1, self->_triggers, 1);
  MyReaction_ctor(&self->my_reaction, &self->super);
  Timer_ctor(&self->timer.super, &self->super, 0, SEC(1), self->timer.effects, 1);
  self->timer.super.super.register_effect(&self->timer.super.super, &self->my_reaction.super);
}

int main(void) {
  struct MyReactor my_reactor;
  Environment env;
  Environment_ctor(&env, (Reactor *)&my_reactor);
  MyReactor_ctor(&my_reactor, &env);
  env.assemble(&env);
  env.start(&env);
}
