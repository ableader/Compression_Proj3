Charles Hsueh
Andrew Leader 45136371

6/4/2014
CS 165
-----------------------
Project 3: Compression
-----------------------

How to use the program:
	The driver program main will prompt the user for input.
	There are 6 total commands that we can use:

		HUFF <file1>
		LZ1 <file1>
		LZ2 <file1>
		EXPAND <file1>
		COMPARE file1 file2
		EXIT

	<file1> indicates that the file is optional and if not specified,
	will ask the user for an input stream.  The input stream will
	be put into a file called inputStream so that the program will
	continue to run without error and the user can compare inputStream files.
	All resulting files will be appended with a -c.

	i.e.	//will output Huffman-encoded test1.txt-c 
		HUFF test1.txt 

	Command descriptions:
		HUFF performs Huffman encoding on file1
		LZ1 performs Lempel-Ziv with Window Size 2048 encoding on file1
		LZ2 performs Lempel-Ziv with Window Size 4096 encoding on file1
		EXPAND performs inverse algorithms.  It gets the first byte and reads
			the magic number and determines what inverse function to do.  More
			information on decoding can be found in the previous analysis.
		COMPARE requires two files.  These files will be checked char by char to
			determine if they are equal.
		EXIT will exit the program.

	There are also some constants you can define in the top of each file to
	get more information about a certain algorithm.  We used these for debugging.
		main.cpp
			SHOW_OUTPUT_STREAM 	// shows resulting output stream
			SHOW_HUFFMAN_TREE 	// shows huffman tree when huff is 
							// used to encode or decode
			SHOW_ENCODING_TABLE	// shows huffman encoding table
		Huffman.cpp
			DISPLAY_FREQUENCY_TABLE	// during encoding, show frequency table
		Lempzev.cpp
			SHOW_TOKEN_LIST		// during encoding, shows all tokens

-----------------------

Benchmark Results:

------------------
news
------------------

size		387168 bytes

HUFFsiz		254930 bytes
%save		34.16 %
t-cmprss	.56 seconds
t-expand	.49 seconds

LZ1siz		219355 bytes
%save		43.34 %
t-cmprss	18 seconds
t-expand	5 seconds

LZ2siz		202494 bytes
%save		47.7 %
t-cmprss	31 seconds
t-expand	10 seconds

WinRARsiz	126011 bytes
%save		67.45 %
t-cmprss	.18 seconds
t-expand	.21 seconds

------------------
book1
------------------

size		768771 bytes

HUFFsiz		438481 bytes
%save		42.96 %
t-cmprss	.97 seconds
t-expand	.86 seconds

LZ1siz		252438 bytes
%save		67.16 %
t-cmprss	21 seconds
t-expand	7 seconds

LZ2siz		233534 bytes
%save		69.62%
t-cmprss	37 seconds
t-expand	12 seconds

WinRARsiz	277131 bytes
%save		63.95%
t-cmprss	.20 seconds
t-expand	.13 seconds

------------------
kennedy.xls
------------------

size		1.02974E6 bytes

HUFFsiz		643105 bytes
%save		37.55 %
t-cmprss	2 seconds
t-expand	1 seconds

LZ1siz		252438 bytes
%save		67.16 %
t-cmprss	141 seconds
t-expand	80 seconds

LZ2siz		233534 bytes
%save		69.62%
t-cmprss	189 seconds
t-expand	98 seconds

WinRARsiz	39627 bytes
%save		74.01%
t-cmprss	.20 seconds
t-expand	.13 seconds

-----------------------

Huff analysis:
	The most costly part of encoding is converting every char into its
	encoded bit.  I decided on use of an map to search for char and return
	the matching amount of bits.  The best way to speed up the process is
	to speed up search time when it came to the encoding table.

	Decoding is speedy in comparison.  There is no search time and the algorithm
	stops when it reaches the artificial EOF char.  Time issue most likely
	comes from BITS Stream class


LZ1/LZ2 analysis:
	LZ1/LZ2 has much space for speed up.  It's clear that for our algorithm
	to speed up, we need to utilize a more efficient data structure.
	We used a vector because it is intuitive and clear, but a trie and/or
	hash table can dramatically speed up search time.  This is important
	because search time is the most costly portion in both Huff and LZ1/LZ2.
	There is most likely another data structure that can better hold window.

	BIT Stream is in general costly for everything.  Adding in chars/ints,
	merging two BIT structures is expensive.







