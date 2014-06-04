#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
using namespace std;
#include "Lempzev.h"

// SHOW_TOKEN_LIST will show list of tokens when true
#define SHOW_TOKEN_LIST false

// helper method to shift the window forward
// during decoding
void shift(vector<char> & window, int n) {
	// rotate moves all the contents of window left by n amount
	rotate(window.begin(), window.begin() + n, window.end());
	// sets position in lookahead to null
	for (int i = window.size() - n; i < window.size(); i++){
		window.at(i) = NULL;
	}
}

// helper method to slide the window forward
// during encoding and take in new input values
void slide(vector<char> & window, vector<char> & input, int & position, int n) {
	// rotate moves all the contents of window left by n amount
	rotate(window.begin(), window.begin() + n, window.end());
	// retrieves next set of chars to put into lookahead
	for (int i = window.size() - n; i < window.size(); i++){
		// if there's still chars to input, we'll push them in
		if (position < input.size()){
			window.at(i) = input.at(position);
			position++;
		}
		// otherwise we'll start pushing NULLS into the window
		else{
			window.at(i) = NULL;
		}
	}
}

// encode input and write to output
// Variation 1 stands for LZ1 where windowSize = 2048
// Variation 2 stands for LZ2 where windowSize = 4096
void Lempzev::encode(vector<char> & input, fstream & output, int variation)
{
	// Prepare variation dependent variables
	int windowSize;
	int offsetBitCount;
	char magicNumber;

	// Initialize to correct variation
	if (variation == 1){
		windowSize = 2048;
		offsetBitCount = 11;
		magicNumber = 17;
	}
	else if (variation == 2){
		windowSize = 4096;
		offsetBitCount = 12;
		magicNumber = 19;
	}
	// If we somehow get an incorrect variation
	// we quit
	else{
		cout << "Error: Wrong Variation\n";
		system("PAUSE");
		exit(1);
	}

	// Create and initialize window
	// To be safe we make sure it is NULL
	vector<char> window = vector<char>(windowSize);
	for (int i = 0; i < windowSize; i++){
		window.at(i) = NULL;
	}

	// Prepare bit stream for output
	Bits BITS = Bits();

	// InputPointer determines how far in input stream we are
	int inputPointer = 0;

	// Magic number to add
	BITS.addChar(magicNumber);

	// initialize window to store first 16 values at the tail
	slide(window, input, inputPointer, 16);

	// intialize vectors to hold tokens
	// calculate lookAheadIndex location
	vector<Token> tokens = vector<Token>();
	const int lookAheadIndex = windowSize - 16;
	
	// Begin encoding
	//
	// While there's still chars to look at in the lookAhead buffer
	// we will attempt to match the largest string of chars
	// in lookAhead with window.
	while (window.at(lookAheadIndex) != NULL) {
		// Initialize the pointer to be right before the lookAhead index
		int windowPointer = lookAheadIndex - 1;
		// offset and len for double token
		int offset, len = 0;
		// Prepare competeter variables so we can determine
		// which string was the longest
		int bestOffset = 0;
		int bestLength = 1;
		// We also need to ensure we found a match
		bool foundMatch = false;
		// Token that we will make
		Token t;

		// Begin character comparison
		// windowPointer will continue to move backwards from lookAhead index to
		// beginning of window
		while (windowPointer > 0 && window.at(windowPointer) != NULL) {
			// When we find a comparison, we want to determine how long the match is
			if (window.at(windowPointer) == window.at(lookAheadIndex)) {
				len = 1;
				// If we continue to find a match, add the length
				// Second conditional limits that we don't go beyond windowSize
				while (lookAheadIndex + len < windowSize && window.at(windowPointer + len) == window.at(lookAheadIndex + len))
					len++;
				// Offset is the difference of windowPointer and lookAheadIndex
				offset = lookAheadIndex - windowPointer;
			}
			// If we find a better match, update best candidates
			if (len > bestLength) {
				foundMatch = true;
				bestOffset = offset;
				bestLength = len;
			}
			// Move windowPointer downwards
			windowPointer--;
		}

		// If we have found a match of len 2 or higher, create a TokenDouble
		if (foundMatch){
			t = Token(bestLength, bestOffset);
			slide(window, input, inputPointer, bestLength);
		}
		// Otherwise create a TokenTriple
		else {
			t = Token(0, 1, window.at(lookAheadIndex));
			slide(window, input, inputPointer, 1);
		}

		// Now we need to determine if we should push back our new token
		// or merge it with the last consecutive literal
		// If the new Token is a double
		if (t.isDouble) {
			// Simply add it to our tokens list
			tokens.push_back(t);
		}
		else {
			// If there's no tokens, we can't compare current token to last token!
			// We just push it onto the tokens list
			if (tokens.size() == 0)
				tokens.push_back(t);
			// If there are tokens, we need to examine the last token to see if
			// we can combine two literals.  This will help compress more space
			else {
				//Check if the last token was triple/literal and has space
				if (!tokens.back().isDouble && tokens.back().chars.size() < 15)
					// If it was, merge to that token
					tokens.back().mergeToken(t);
				// Otherwise, simply push back new token
				else
					tokens.push_back(t);
			}
		}
	}
	
	// Add EOF character
	tokens.push_back(Token(0, 0));

	// Reveals TOKEN LIST when user assigns SHOW_TOKEN_LIST true
	if (SHOW_TOKEN_LIST){
		for (int i = 0; i < tokens.size(); i++) {
			Token current = tokens.at(i);
			if (current.isDouble){
				cout << "<" << current.len << ", " << current.offset << ">\n";
			}
			else{
				cout << "<" << current.code << ", " << current.strLen << ", ";
				for (int j = 0; j < current.chars.size(); j++){
					cout << current.chars.at(j);
				}
				cout << ">\n";
			}
		}
	}
	
	// Convert all tokens into bits
	for (int i = 0; i < tokens.size(); i++){
		// Get the current token in tokens
		Token current = tokens.at(i);
		// If it's a double (matched token)
		if (current.isDouble){
			// I realize we can save space here
			// Token doubles take up 4 + offsetBitSize
			// EOF doubles take up 8 bits
			// So we can save a couple bits by simply inputting 8 0s
			if (current.len == 0){
				for (int j = 0; j < 8; j++)
					BITS.addBit(0);
			}
			// Else just add in double bits
			else{
				BITS.addInt(current.len - 1, 4);
				BITS.addInt(current.offset, offsetBitCount);
			}
		}
		else{
			// Add in triple bits
			BITS.addInt(current.code, 4);
			BITS.addInt(current.strLen, 4);
			for (int j = 0; j < current.chars.size(); j++){
				BITS.addChar(current.chars.at(j));
			}
		}
	}

	// Convert binary bits into chars to be transferred back into output
	while (BITS.good()){
		output.put(BITS.getChar());
	}
}

// Decodes Lempzev compressed files
void Lempzev::decode(vector<char> & input, fstream & output, int variation)
{
	// Intialize window and bit stream
	vector<char> window;
	Bits BITS = Bits();
	int numOffsetBits;
	int windowSize;

	// Initialize based off variation
	if (variation == 1){
		numOffsetBits = 11;
		windowSize = 2048;
	}
	else{
		numOffsetBits = 12;
		windowSize = 4096;
	}
		
	// Convert all input chars into bits
	for (int i = 1; i < input.size(); i++) {
		BITS.addChar(input.at(i));
	}

	// Prepare the window to match the windowSize
	window = vector<char>(windowSize);
	// Prepare lookAheadIndex
	int lookAheadIndex = windowSize - 16;

	// We will continue this process until eof is found
	while (1) {
		// Get first four bits to determine len/code
		int code = BITS.getInt(4);
		// if code is not 0, we know it's a double (matched token)
		if (code > 0){
			// increment code by 1 to get the proper len
			int len = code + 1;
			// get offset based off numOffsetBits
			int offset = BITS.getInt(numOffsetBits);
			// As we update the lookAheadIndex, output chars
			for (int i = 0; i < len; i++){
				window.at(lookAheadIndex + i) = window.at(lookAheadIndex + i - offset);
				output.put(window.at(lookAheadIndex + i));
			}
			// Shift the window by len
			shift(window, len);
		}
		// We know it's a triple (literal token)
		else {
			// Get strlen to determine how many characters will follow
			int strlen = BITS.getInt(4);
			// If 0, we found the eof
			if (strlen == 0) {
				return;
			}
			// Otherwise we print all the literals
			else {
				// For every character to output, take it out of BIT string
				// put it into the lookAhead buffer
				// output chars
				for (int i = 0; i < strlen; i++) {
					char tempC = BITS.getChar();
					window.at(lookAheadIndex + i) = tempC;
					output.put(window.at(lookAheadIndex + i));
				}
			}
			// Shift the window by strlen
			shift(window, strlen);
		}
	}
}

