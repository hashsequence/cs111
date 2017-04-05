#!/bin/sh

sort lab3b_check.txt.1 > a
sort lab3b_check.txt > b

diff a b

if [ $? -eq 0 ]
then 
    echo "no differences"
else
    echo "is different"
fi

rm -rf a b 
