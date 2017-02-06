#!/bin/bash

rm test

gcc -o test -I. -I./include -O0 -g -DUNIT_TEST -DQUEUE_TEST -Wall -Wextra -fno-strict-aliasing -std=gnu99 queue.c

./test
