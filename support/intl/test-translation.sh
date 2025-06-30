#!/bin/bash

set -euxo pipefail

(
  cd test

  xgettext --no-location --sort-output --omit-header -o part1.pot part1.c
  xgettext --no-location --sort-output --omit-header -o part2.pot part2.c

  msgmerge --update --backup=none --sort-output --no-fuzzy-matching zh_CN/part1.po part1.pot
  msgmerge --update --backup=none --sort-output --no-fuzzy-matching zh_CN/part2.po part2.pot

  msgfmt -o zh_CN/part1.mo zh_CN/part1.po
  msgfmt -o zh_CN/part2.mo zh_CN/part2.po
)

xmake build test

xmake install -o pkg test

LC_ALL=en_US.UTF-8 ./pkg/bin/test.exe
LC_ALL=zh_CN.UTF-8 ./pkg/bin/test.exe | iconv -f CP936 -t UTF-8

LC_ALL=zh_CN.UTF-8 LANGUAGE=zh_SG:en_US ./pkg/bin/test.exe | iconv -f CP936 -t UTF-8
