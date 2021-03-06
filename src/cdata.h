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

#ifndef CDATAH
#define CDATAH
#include "mytypes.h"
#include "cstruct.h"

struct mydatablock {
  struct cdata        cd;
  struct mydatablock *next;
};

void cdata_init(struct cdata *cd, int checksum);

void cdata_ncbytes(struct cdata *cd, int ncb);

void cdata_nubytes(struct cdata *cd, int nub);


#endif
