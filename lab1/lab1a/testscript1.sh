#!/bin/sh

./simpsh --verbose --command 0 1 2 ls a b
./simpsh --verbose --command 0 1 2 ls -a -b --rdonly tr tc
./simpsh --verbose --command 0 1 2 ls -a -b  
