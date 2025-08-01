/*
 *  OpenSlide, a library for reading whole slide image files
 *
 *  Copyright (c) 2023 Benjamin Gilbert
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

#include <stdio.h>
#include <errno.h>
#include "openslide-common.h"
#include "slidetool.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

struct output open_output(const char *filename) {
  struct output out;
  GError *tmp_err = NULL;
  if (filename) {
    out.fp = common_fopen(filename, "wb", &tmp_err);
    if (!out.fp) {
      common_fail("%s", tmp_err->message);
    }
  } else {
    if (isatty(1)) {
      common_fail("Will not write binary output to terminal");
    }
    out.fp = stdout;
  }
  return out;
}

void _close_output(struct output *out) {
  if (out->fp != stdout) {
    if (fclose(out->fp)) {
      common_fail("Can't close output: %s", g_strerror(errno));
    }
  } else {
    if (fflush(out->fp)) {
      common_fail("Can't flush stdout: %s", g_strerror(errno));
    }
  }
}
