#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <vector>
#include <bitset>
#include <string>
#include <tr1/unordered_map>
using namespace std;
#include "Lempzev.h"

#define private vector<char> window;

// helper method to slide the window forward
// implement this later
private void slide(fstream & input, int n) {
	char c;
	while (input.good()) {
		c = input.get();
	}
	window.push_back(c);
}

string Lempzev::encode(fstream & input)
{
	window = vector<char>(2048);
	vector<Token> matchingTokens = new vector<Token>(2048);
	string code;
	Bits BITS = Bits();
	char charToAdd;

	// Magic number to add
	charToAdd = 17;
	BITS.addChar(charToAdd);

	// initialize window to store first 16 values at the tail
	for (int i = 0; i < 16; i++) {
		if (input.good()) {
			window.push_back(input.get());
		}
	}

	// pointer for quick reference to the lookahead (inside the sliding window)
	// 2033 is the index where the lookahead begins (the last 16 chars in the window)
	int STARTOFLOOKAHEAD = 2033;
	int matchStartIndex = -1;
	bool foundMatch = false;
	int windowIndex, len = 0;
	int offset;
	int optimalOffset = 0;
	int longestLength = 1;
	Token t;

	// search the window for a match with a prefix of the lookahead buffer
	// 2033 is the maximum offset we can have for window size 2048
	while (windowIndex < STARTOFLOOKAHEAD) {

		if (window.at(windowIndex) == window.at(STARTOFLOOKAHEAD)) {
			len = 1;
			while (window.at(windowIndex + len) == window.at(STARTOFLOOKAHEAD + len) && STARTOFLOOKAHEAD+len < 2048) {
				len++;
			}
			offset = STARTOFLOOKAHEAD - windowIndex;
		}
		if (len > longestLength) {
			foundMatch = true;
			optimalOffset = offset;
			longestLength = len;
		}
	}
	if (foundMatch)
		t = new TokenDouble(longestLength, optimalOffset);
	else {
		t = new TokenTriple(0, 1, 0);
	}

	if (t.isDouble()) {
		matchingTokens.push_back(t);
	}
	else {
		if (matchingTokens.back().isDouble())
			matchingTokens.push_back(t);
		else
			matchingTokens.back().mergeToken(t);
	}


	// Convert binary bits into chars to be transferred back into string
	while (BITS.good()){
		code.push_back(BITS.getChar());
	}

	// Reset pointer in filestream
	input.clear();
	input.seekg(0);

	// Add artificial end of symbol
	Bits endBITS = encodingTable[EOF];
	BITS.mergeBits(endBITS);

	return code;
}

string Lempzev::decode(fstream & input)
{
	string resultToWrite;
	Bits BITS = Bits();

	int count = 0;

	// PROBLEM IS HERE
	char c;
	while (input.get(c))
	{
		BITS.addChar(c);
	}

	cout << "count result: ";
	cout << count << " " << BITS.size / 8 << "\n";

	// Reset pointer in filestream
	input.clear();
	input.seekg(0);

	return resultToWrite;
}

