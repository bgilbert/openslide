/*
 *  OpenSlide, a library for reading whole slide image files
 *
 *  Copyright (c) 2007-2010 Carnegie Mellon University
 *  All rights reserved.
 *
 *  OpenSlide is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, version 2.1.
 *
 *  OpenSlide is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with OpenSlide. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OPENSLIDE_OPENSLIDE_HASH_H_
#define OPENSLIDE_OPENSLIDE_HASH_H_

#include <config.h>

#include "openslide.h"

#ifndef _MSC_VER
#include <stdbool.h>
#endif
#include <stdint.h>
#include <tiffio.h>
#include <glib.h>

struct _openslide_hash;

// constructor
struct _openslide_hash *_openslide_hash_quickhash1_create(void);

// hashers
bool _openslide_hash_tiff_tiles(struct _openslide_hash *hash, TIFF *tiff);
void _openslide_hash_string(struct _openslide_hash *hash, const char *str);
bool _openslide_hash_file(struct _openslide_hash *hash, const char *filename);
bool _openslide_hash_file_part(struct _openslide_hash *hash,
			       const char *filename,
			       int64_t offset, int64_t size);

// accessors
const char *_openslide_hash_get_string(struct _openslide_hash *hash);
double _openslide_hash_get_time(struct _openslide_hash *hash);

// destructor
void _openslide_hash_destroy(struct _openslide_hash *hash);

#endif
