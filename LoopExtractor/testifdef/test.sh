#!/bin/sh 
rm *_loop_*

mv ifdef_old.c ifdef.c

.././driver ifdef.c

