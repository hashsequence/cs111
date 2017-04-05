#!/bin/sh
export PATH=/usr/local/cs/bin:$PATH
NUM_PASSED=0
NUM_FAILED=0

touch a.txt
touch b.txt
touch c.txt

echo "cat in the hat" > a.txt

./simpsh --rdonly a.txt --rdonly b.txt --wronly c.txt
echo "first case tests if multiple rdonly works"
if [ $? -ne 0 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo "first case passed"
fi

rm -rf a.txt b.txt c.txt
touch a.txt
touch b.txt
touch c.txt
echo "cat in the hat" > a.txt



./simpsh --rdonly a.txt --rdonly b.txt --wronly c.txt
diff b.txt c.txt
echo "second case test that it should have error when attempting to write into read only file"
if [ $? != 0 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo "second case passed"
fi

rm -rf a.txt b.txt c.txt
touch a.txt
touch b.txt
touch c.txt
echo "cat in the hat" > a.txt

echo "third case test if error is produced when command have less than four arguments"
./simpsh --rdonly a.txt --rdonly b.txt --wronly c.txt --command 0 1 2

if [ $? -ne 1 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo "third case passed"
fi

rm -rf a.txt b.txt c.txt


touch a
touch b
touch c

echo "cartman" > a; \
./simpsh --verbose --rdonly a --wronly b --wronly c --ignore 11 --abort --command 0 1 2 cat
if [ $? -ne 0 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo "fourth  case passed"
fi
 
./simpsh --verbose --rdonly a --wronly b --wronly c --close 1  --command 0 1 2 cat
if [ $? -ne 0 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo "fifth case passed"
fi

./simpsh --verbose --rdonly a --wronly b --wronly c --catch 11 --abort --command 0 1 2 cat
if [ $? -ne 11 ]; then
    NUM_FAILED=`expr $NUM_FAILED + 1`
else
    NUM_PASSED=`expr $NUM_PASSED + 1`
    echo
    echo "sixth case passed"
fi

rm a b c
touch a.txt
touch b.txt
touch c.txt
touch a1.txt
touch b1.txt
touch c1.txt
touch a2.txt
touch b2.txt
touch c2.txt
echo "cat in the hat" > a.txt
./simpsh  --rdonly a.txt --rdonly b.txt --wronly c.txt --verbose --rdonly a1.txt --rdonly b1.txt --wronly c1.txt --rdonly a2.txt --rdonly b2.txt --wronly c2.txt   --command 0 1 2 ls -l -a --command 3 4 5 tr A-Z a-z --command 6 7 8 cat b - --wait
cat b.txt
rm -rf a.txt b.txt c.txt a1.txt b1.txt c1.txt a2.txt b2.txt c2.txt 

touch a.txt
touch b.txt
touch c.txt

echo "cat in the hat" > a.txt
./simpsh --verbose --rdonly a.txt --rdonly b.txt --wronly c.txt --command 0 1 2 ls -l -a --command 0 1 2 tr A-Z a-z --command 0 1 2 cat b - --wait
cat b.txt
rm -rf a.txt b.txt c.txt 

touch a
touch c
touch d
touch b

./simpsh \
  --rdonly a \
  --pipe \
  --pipe \
  --verbose\
  --creat --trunc --wronly c \
  --creat --append --wronly d \
  --command 3 5 6 tr A-Z a-z \
  --command 0 2 6 sort \
  --command 1 4 6 cat b - \
  --wait

rm a b c d
echo "$NUM_PASSED test passed and $NUM_FAILED test failed"

rm simpsh
