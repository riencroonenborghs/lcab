/*  Copyright 2001 Rien Croonenborghs, Ben Kibbey, Shaun Jackman, Ivan Brozovic
 *
 *  This file is part of lcab.
 *  lcab is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  lcab is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with lcab; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdlib.h>
#include "func.h"
#include "warn.h"

struct sllitem *sllitem_init(struct sllitem *item, void *data)
{
  item->data = data;
  item->next = NULL;
  return item;
}

struct sllitem *nth(struct sllitem *item, int idx)
{
  if (idx == 0) return item;
  if (item->next == NULL) return NULL;
  return nth(item->next, idx - 1);
}

/**
 * simple reducer function which will count the number of sslitem(s)
 */
int reducer_count(int acc, struct sllitem *e)
{
  return acc + 1;
}

/**
 * iterating driver over a set of sllitem(s).
 *
 * @param values the values over which to iterate, NULL is ok
 * @param fn your callback function
 * @param ctx a pointer to some extra data to be passed back to your callback
 */
int for_each(struct sllitem *values, int fn(struct sllitem *value, void *ctx), void *ctx)
{
  struct sllitem *p = values;
  int r = 0;

  trace("> for_each, start from %d", values || 0);
  if (fn) {
    while (p) {
      r = fn(p, ctx);
      if (r)
        return r;
      p = p->next;
    }
  }
  return r;
}

int reduce(int initial, struct sllitem *values, int fn(int acc, struct sllitem *value))
{
  if (!fn || !values) return initial;
  struct sllitem *p;
  int acc;

  p = values;
  acc = initial;
  while (p) {
    acc = fn(acc, p);
    p = p->next;
  }
  return acc;
}

struct sllitem *prepend(struct sllitem *accumulator, struct sllitem *addend)
{
  if (!accumulator) return addend;
  if (!addend) return accumulator;
  addend->next = accumulator;
  return addend;
}

struct sllitem *append(struct sllitem *accumulator, struct sllitem *addend)
{
  if (!accumulator) return addend;
  struct sllitem *p = accumulator;
  while (p->next)
    p = p->next;
  p->next = addend;
  return accumulator;
}
