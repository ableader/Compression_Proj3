#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
using namespace std;
#include "Lempzev.h"

#define SHOW_TOKEN_LIST false

// helper method to shift the window forward
// during decoding
void shift(vector<char> & window, int n) {
	rotate(window.begin(), window.begin() + n, window.end());
	for (int i = window.size() - n; i < window.size(); i++){
		window.at(i) = NULL;
	}
}

// helper method to slide the window forward
// during encoding and take in new input values
void slide(vector<char> & window, vector<char> & input, int & position, int n) {
	rotate(window.begin(), window.begin() + n, window.end());
	for (int i = window.size() - n; i < window.size(); i++){
		if (position < input.size()){
			window.at(i) = input.at(position);
			position++;
		}
		else{
			window.at(i) = NULL;
		}
	}
}

void Lempzev::encode(vector<char> & input, fstream & output, int variation)
{
	int windowSize;
	int offsetBitCount;
	char magicNumber;

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
		
	else{
		cout << "Error: Wrong Variation\n";
		system("PAUSE");
		exit(1);
	}

	// Create and initialize window
	vector<char> window = vector<char>(windowSize);
	for (int i = 0; i < windowSize; i++){
		window.at(i) = NULL;
	}

	// Prepare bit stream for output
	Bits BITS = Bits();
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
	while (window.at(lookAheadIndex) != NULL) {
		int windowPointer = lookAheadIndex - 1;
		int offset, len = 0;
		int bestOffset = 0;
		int bestLength = 1;
		bool foundMatch = false;
		Token t;

		// Begin character comparison
		while (windowPointer > 0 && window.at(windowPointer) != NULL) {
			// When we find a comparison, we want to determine how long the match is
			if (window.at(windowPointer) == window.at(lookAheadIndex)) {
				len = 1;
				// If we continue to find a match, add the length
				// Second conditional limits that we don't go beyond windowSize
				while (lookAheadIndex + len < windowSize &&
					window.at(windowPointer + len) == window.at(lookAheadIndex + len)
					) {
					len++;
				}
				// Offset is the difference of windowPointer and lookAheadIndex
				offset = lookAheadIndex - windowPointer;
			}
			// If we find a better match, update best candidates
			if (len > bestLength) {
				foundMatch = true;
				bestOffset = offset;
				bestLength = len;
			}
			windowPointer--;
		}

		// If we have found a match of len 2 or higher, create a TokenDouble
		// Otherwise create a TokenTriple
		if (foundMatch){
			t = Token(bestLength, bestOffset);
			slide(window, input, inputPointer, bestLength);
		}
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
			if (tokens.size() == 0)
				tokens.push_back(t);
			else {
				//Check if the last token was triple and has space
				if (!tokens.back().isDouble && tokens.back().chars.size() < 15)
					// If it was, merge to that token
					tokens.back().mergeToken(t);
				else
					// Otherwise, simply push back new token
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
		Token current = tokens.at(i);
		if (current.isDouble){
			// If EOF token
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

	// Convert binary bits into chars to be transferred back into string
	while (BITS.good()){
		output.put(BITS.getChar());
	}
}

void Lempzev::decode(vector<char> & input, fstream & output, int variation)
{
	vector<char> window;
	Bits BITS = Bits();
	int numOffsetBits;
	int windowSize;

	// verify variation
	if (variation == 1){
		numOffsetBits = 11;
		windowSize = 2048;
	}
	else{
		numOffsetBits = 12;
		windowSize = 4096;
	}
		
	// Convert all chars into bits
	for (int i = 1; i < input.size(); i++) {
		BITS.addChar(input.at(i));
	}

	window = vector<char>(windowSize);
	int lookAheadIndex = windowSize - 16;

	while (1) {
		int code = BITS.getInt(4);
		if (code > 0){
			int len = code + 1;
			int offset = BITS.getInt(numOffsetBits);
			for (int i = 0; i < len; i++){
				window.at(lookAheadIndex + i) = window.at(lookAheadIndex + i - offset);
				output.put(window.at(lookAheadIndex + i));
			}
			shift(window, len);
		}
		else {
			int strlen = BITS.getInt(4);
			if (strlen == 0) {
				return;
			}
			else {
				for (int i = 0; i < strlen; i++) {
					char tempC = BITS.getChar();
					window.at(lookAheadIndex + i) = tempC;
					output.put(window.at(lookAheadIndex + i));
				}
			}
			shift(window, strlen);
		}
	}
}

