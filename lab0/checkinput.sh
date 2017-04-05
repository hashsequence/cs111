#!/bin/sh

touch newfile.txt
echo "blah blah cat master" > newfile.txt
./lab0 --input newfile.txt
if [ $? -eq 0 ] 
then
echo "true2!"
fi
