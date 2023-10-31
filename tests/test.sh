#!/bin/bash

zip_in=""
zip_out=""
unzip_in=""
unzip_out=""

function check() {
	if [ ! -f "$zip_out" ]; then return 1; fi

	cmp "$zip_in" "$zip_out" 2>/dev/null 1>&2
	if [ $? -eq 0 ]; then return 1; fi

	if [ ! -f "$unzip_out" ]; then return 1; fi

	cmp "$zip_in" "$unzip_out" 2>/dev/null 1>&2
	if [ $? -ne 0 ]; then return 1; fi

	return 0
}

# Generate samples
mkdir -p samples

touch samples/empty.a

cat ../src/zip/main.c >samples/tmp.txt

> samples/bigtmp.txt
for i in $(seq 1 100); do
	cat ../src/zip/* ../src/unzip/* ../include/*.h >>samples/bigtmp.txt
	cat ../zlib-1.3/*.c ../zlib-1.3/*.c >>samples/bigtmp.txt
done

# TEST 00
{
  echo "Test 00: "

  zip_in="samples/empty.a"
  zip_out="empty.a.zip"
  unzip_in="$zip_out"
  unzip_out="empty.a"

  zip="../zip $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 01
{
  echo "Test 01: "

  zip_in="samples/tmp.txt"
  zip_out="tmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="tmp.txt"

  zip="../zip $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 02
{
  echo "Test 02: "

  zip_in="samples/tmp.txt"
  zip_out="tmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="tmp.txt"

  zip="../zip -s 1 $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 03
{
  echo "Test 03: "

  zip_in="samples/tmp.txt"
  zip_out="tmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="tmp.txt"

  zip="../zip -s 1 -m KB $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 04
{
  echo "Test 04: "

  zip_in="samples/tmp.txt"
  zip_out="tmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="tmp.txt"

  zip="../zip -s 2 -m KB $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 05
{
  echo "Test 05: "

  zip_in="samples/tmp.txt"
  zip_out="a123.zip"
  unzip_in="$zip_out"
  unzip_out="a123"

  zip="../zip -s 50 -m MB $zip_in $zip_out"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 06
{
  echo "Test 06: "

  zip_in="samples/bigtmp.txt"
  zip_out="bigtmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="bigtmp.txt"

  zip="../zip $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 07
{
  echo "Test 07: "

  zip_in="samples/bigtmp.txt"
  zip_out="bigtmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="bigtmp.txt"

  zip="../zip -s 100 $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 08
{
  echo "Test 08: "

  zip_in="samples/bigtmp.txt"
  zip_out="bigtmp.txt.zip"
  unzip_in="$zip_out"
  unzip_out="bigtmp.txt"

  zip="../zip -s 100 -m MB $zip_in"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 09
{
  echo "Test 09: "

  zip_in="samples/bigtmp.txt"
  zip_out="123.a"
  unzip_in="$zip_out"
  unzip_out="123.a.out"

  zip="../zip -s 1000 -m B $zip_in $zip_out"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 10
{
  echo "Test 10: "

  zip_in="samples/bigtmp.txt"
  zip_out=".zip"
  unzip_in="$zip_out"
  unzip_out="unzip.out"

  zip="../zip -s 1000 -m MB $zip_in $zip_out"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 11
{
  echo "Test 11: "

  zip_in="../zip"
  zip_out="binary.zip"
  unzip_in="$zip_out"
  unzip_out="binary"

  zip="../zip -s 40 -m KB $zip_in $zip_out"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

# TEST 12
{
  echo "Test 12: "

  zip_in="../zip"
  zip_out="binary.out.zip"
  unzip_in="$zip_out"
  unzip_out="binary.out"

  zip="../zip -s 1 -m B $zip_in $zip_out"
  echo "$zip"
  `$zip`

  unzip="../unzip $unzip_in"
  echo "$unzip"
  `$unzip`

  if ! check; then echo "failed"; else echo "successed"; fi

  rm "$zip_out" "$unzip_out"

  echo
}

