Charles Hsueh
56282680

Andrew Leader


Project 3: Compression

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
	continue to run without error and the user can compare files.
	All resulting files will be appended with a -c.
	i.e. HUFF test1.txt //will output encoded test1.txt-c 

	HUFF performs Huffman encoding on file1
	LZ1 performs Lempel-Ziv encoding on file1
	LZ2 performs Lempel-Ziv encoding on file1
	EXPAND performs inverse algorithms.  It gets the first byte and reads
		the magic number and determines what inverse function to do.  More
		information on decoding can be found in the previous analysis.
	COMPARE requires two files.  These files will be checked char by char to
		determine if they are equal.
	EXIT will exit the program.

Huff analysis:
	HUFF performs Huffman encoding on file1

	The most costly part of encoding is converting every char into its
	encoded bit.  I decided on use of an map to search for char and return
	the matching amount of bits.  Appending bits is also costly procedure
	in our algorithm.  I believe if we cut down on the amount of data
	allocation in our BITS data frame we could significantly boost our
	speed.

	Decoding is very speedy.  There is no search time and the algorithm
	stops when it reaches the artificial EOF char.

LZ1 analysis

LZ2 analysis
	