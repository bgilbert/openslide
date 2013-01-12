#!/usr/bin/env python
#
#  OpenSlide, a library for reading whole slide image files
#
#  Copyright (c) 2012-2013 Carnegie Mellon University
#  All rights reserved.
#
#  OpenSlide is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, version 2.1.
#
#  OpenSlide is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with OpenSlide. If not, see
#  <http://www.gnu.org/licenses/>.
#

#
# Convert a slide file into a fake Hamamatsu VMU slide that's good enough
# to fool OpenSlide.
#

from __future__ import division
from ConfigParser import RawConfigParser
from fractions import gcd
from openslide import OpenSlide
import struct
import sys

BUF_HEIGHT = 512

class VmuLevel(object):
    def __init__(self, osr, level):
        self._osr = osr
        self.level = level
        self.width, self.height = osr.level_dimensions[level]
        self.column_width = gcd(self.width, 400)
        self.downsample = osr.level_downsamples[level]

    def save(self, path):
        with open(path, 'w') as fh:
            # Header
            fh.write(struct.pack('<2c2x3i8xi4x', 'G', 'N', self.width,
                    self.height, self.column_width, 32))

            # Body
            for col in xrange(self.width // self.column_width):
                # BUF_HEIGHT rows at a time
                for i in xrange((self.height + BUF_HEIGHT - 1) // BUF_HEIGHT):
                    rows = min(BUF_HEIGHT, self.height - i * BUF_HEIGHT)
                    img = self._osr.read_region(
                            (int(col * self.column_width * self.downsample),
                            int(i * BUF_HEIGHT * self.downsample)),
                            self.level,
                            (self.column_width, rows)).load()
                    for y in xrange(rows):
                        for x in xrange(self.column_width):
                            pix = [v << 4 for v in img[x, y]]
                            # FIXME: ignores alpha
                            fh.write(struct.pack('<3h', *pix[0:3]))


def make_vmu(in_path, out_base, with_map=True):
    path_conf = out_base + '.vmu'

    with OpenSlide(in_path) as osr:
        levels = [VmuLevel(osr, 0)]
        if with_map:
            levels.append(VmuLevel(osr, osr.get_best_level_for_downsample(32)))
        level_paths = ['%s.l%d' % (out_base, i) for i in range(len(levels))]
        for i, l in enumerate(levels):
            print 'Level %d: %d pixels/column' % (i, l.column_width)
        for i, l in enumerate(levels):
            l.save(level_paths[i])

    section = 'Uncompressed Virtual Microscope Specimen'
    conf = {
        'NoLayers': '1',
        'ImageFile': level_paths[0],
        'MapFile': level_paths[1] if len(levels) > 1 else '',
        'BitsPerPixel': '36',
        'PixelOrder': 'RGB',
    }

    c = RawConfigParser()
    c.optionxform = str
    c.add_section(section)
    for k, v in conf.iteritems():
        c.set(section, k, v)
    with open(path_conf, 'w') as fh:
        c.write(fh)


if __name__ == '__main__':
    with_map = '-1' not in sys.argv
    argv = [v for v in sys.argv if v != '-1']
    if len(argv) != 3:
        print 'Usage: %s [-1] infile outbase' % argv[0]
        sys.exit(1)
    make_vmu(argv[1], argv[2], with_map)
