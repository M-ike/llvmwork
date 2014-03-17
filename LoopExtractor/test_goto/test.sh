#!/bin/sh 
rm *_loop_*

mv test_goto_old.c test_goto.c

./driver test_goto.c

