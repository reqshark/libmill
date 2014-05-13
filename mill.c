/*
    Copyright (c) 2014 Martin Sustrik  All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "mill.h"

#include <stdio.h>

/******************************************************************************/
/* Generic stuff.                                                             */
/******************************************************************************/

#define mill_cont(ptr, type, member) \
    (ptr ? ((type*) (((char*) ptr) - offsetof(type, member))) : NULL)

void mill_base_init (
    struct mill_base *self,
    mill_handler_fn handler,
    struct mill_base *parent,
    struct mill_loop *loop)
{
    self->handler = handler;
    self->state = 0;
    self->parent = parent;
    self->prev = 0;
    self->next = 0;
    self->loop = loop;
}

void mill_base_term (struct mill_base *self)
{
    assert (0);
}

void mill_base_emit (struct mill_base *self)
{
    mill_loop_emit (self->loop, self->parent, self);
}

/******************************************************************************/
/* Wait coroutine.                                                            */
/******************************************************************************/

static void wait_handler(struct mill_base *base, event ev)
{
    struct mill_coroutine_wait *self = (struct mill_coroutine_wait*) base;
    mill_base_emit (&self->mill_base);
}

static void wait_cb (uv_timer_t *timer)
{
    wait_handler (&mill_cont (timer,
        struct mill_coroutine_wait, timer)->mill_base, 0);
}

void mill_call_wait (
    struct mill_coroutine_wait *self,
    int milliseconds,
    struct mill_base *parent,
    struct mill_loop *loop)
{
    mill_base_init (&self->mill_base, wait_handler, parent, loop);
    uv_timer_init(&loop->uv_loop, &self->timer);
    uv_timer_start(&self->timer, wait_cb, milliseconds, 0);
}

/******************************************************************************/
/* The event loop.                                                            */
/******************************************************************************/

void mill_loop_init (struct mill_loop *self)
{
    int rc;

    rc = uv_loop_init (&self->uv_loop);
    assert (rc == 0);
}

void mill_loop_term (struct mill_loop *self)
{
    int rc;
    rc = uv_loop_close (&self->uv_loop);
    assert (rc == 0);
}

void mill_loop_run (struct mill_loop *self) {
    int rc;

    rc = uv_run (&self->uv_loop, UV_RUN_DEFAULT);
    assert (rc == 0);
}

void mill_loop_emit (struct mill_loop *self, struct mill_base *dst, event e)
{
    assert (0);
}

