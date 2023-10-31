#!/bin/bash

# Generate samples
mkdir -p samples
> samples/empty
cal >samples/tempfile

# TEST 01
{
	echo -n "Fuzz test 01: "

	echo "../zip 2>/dev/null"
	../zip 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 02
{
	echo -n "Fuzz test 02: "

	echo "../zip noexist 2>/dev/null"
	../zip noexist 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 03
{
	echo -n "Fuzz test 03: "

	echo "../zip /dev/zero 2>/dev/null"
	../zip /dev/zero 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 04
{
	echo -n "Fuzz test 04: "

	echo "../zip -m samples/tempfile 2>/dev/null"
	../zip -m samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 05
{
	echo -n "Fuzz test 05: "

	echo "../zip -masdjjadsj samples/tempfile 2>/dev/null"
	../zip -masdjjadsj samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 06
{
	echo -n "Fuzz test 06: "

	echo "../zip -m BB samples/tempfile 2>/dev/null"
	../zip -mBB samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 06
{
	echo -n "Fuzz test 06: "

	echo "../zip -s samples/tempfile 2>/dev/null"
	../zip -s samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 07
{
	echo -n "Fuzz test 07: "

	echo "../zip -s 0 samples/tempfile 2>/dev/null"
	../zip -s 0 samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 08
{
	echo -n "Fuzz test 08: "

	echo "../zip -s -100 samples/tempfile 2>/dev/null"
	../zip -s -100 samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 09
{
	echo -n "Fuzz test 09: "

	echo "../zip -s bred samples/tempfile 2>/dev/null"
	../zip -s bred samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 10
{
	echo -n "Fuzz test 10: "

	echo "../zip -s 120abc samples/tempfile 2>/dev/null"
	../zip -s 120abc samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 11
{
	echo -n "Fuzz test 11: "

	echo "../zip -s 1000000000000000000 samples/tempfile 2>/dev/null"
	../zip -s 1000000000000000000 samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 12
{
	echo -n "Fuzz test 12: "

	echo "../zip -s 100000000000000 -m GB samples/tempfile 2>/dev/null"
	../zip -s 100000000000000 -m GB samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 13
{
	echo -n "Fuzz test 13: "

	echo "../zip -t -s 1 samples/tempfile 2>/dev/null"
	../zip -t -s 1 samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 14
{
	echo -n "Fuzz test 14: "

	echo "../zip --s 1 samples/tempfile 2>/dev/null"
	../zip --s 1 samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 15
{
	echo -n "Fuzz test 15: "

	echo "../zip samples 2>/dev/null"
	../zip samples 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 16
{
	echo -n "Fuzz test 16: "

	echo "../zip '%s %n' 2>/dev/null"
	../zip '%s %n' 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 17
{
	echo -n "Fuzz test 17: "

	echo "../zip '    ' 2>/dev/null"
	../zip '    ' 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 17
{
	echo -n "Fuzz test 17: "

	echo "../zip '' 2>/dev/null"
	../zip '' 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 18
{
	echo -n "Fuzz test 18: "

	echo "../unzip 2>/dev/null"
	../unzip 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 19
{
	echo -n "Fuzz test 19: "

	echo "../unzip '   ' 2>/dev/null"
	../unzip '    ' 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 20
{
	echo -n "Fuzz test 20: "

	echo "../unzip '' 2>/dev/null"
	../unzip '' 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 21
{
	echo -n "Fuzz test 21: "

	echo "../unzip noexist 2>/dev/null"
	../unzip noexist 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 22
{
	echo -n "Fuzz test 22: "

	echo "../unzip samples 2>/dev/null"
	../unzip samples 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 23
{
	echo -n "Fuzz test 23: "

	echo "../unzip /dev/zero 2>/dev/null"
	../unzip /dev/zero 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 24
{
	echo -n "Fuzz test 24: "

	echo "../unzip samples/tempfile 2>/dev/null"
	../unzip samples/tempfile 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

# TEST 25
{
	echo -n "Fuzz test 25: "

	echo "../unzip samples/empty 2>/dev/null"
	../unzip samples/empty 2>/dev/null
	if [ $? -ne 0 ]; then echo "successed"; else echo "failed"; fi
	
	echo;
}

