#include <iostream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <vector>
#include <bitset>
#include <unordered_map>
using namespace std;
#include "Huffman.h"

// When researched, many people agreed char 3 is EOF file token.
// Very appropriate for our usage
#define EOF 3
// Used primarily for debugging
// Shows us frequency that characters appear
#define DISPLAY_FREQUENCY_TABLE false

void Huffman::buildHuffman(vector<char> & input)
{
	// Obtain frequencies of all chars from filestream
	unordered_map<char, int> frequencyTable = unordered_map<char, int>();
	for (int i = 0; i < input.size(); i++){
		frequencyTable[input.at(i)]++;
	}

	// Add in artificial end of file symbol
	if (frequencyTable.count(EOF)) cout << "found EOF\n";
	frequencyTable[EOF] = 1;

	// Displays Frequency Table
	if (DISPLAY_FREQUENCY_TABLE){
		int sum = 0;
		for (auto it = frequencyTable.begin(); it != frequencyTable.end(); ++it){
			sum += it->second;
			cout << it->first << ": " << it->second << ": " << sum << "\n";
		}
		cout << "Total sum of symbols: " << sum << "\n";
	}
	
	// Create leaf Nodes for each char
	priority_queue<Node*, vector<Node*>, MyComparator > pq;
	for (auto it = frequencyTable.begin(); it != frequencyTable.end(); ++it){
		pq.push(new Node(it->first, it->second));
	}

	// Populate binary tree
	while (pq.size() > 1){
		// Get two lowest frequency nodes
		NodePointer nodeOne = pq.top();
		int nodeOneFrequency = nodeOne->frequency;

		pq.pop();
		NodePointer nodeTwo = pq.top();
		int nodeTwoFrequency = nodeTwo->frequency;

		pq.pop();
		// Create internal node of two lowest nodes
		NodePointer newNode = new Node('-', nodeOneFrequency + nodeTwoFrequency);

		// Make two lowest frequency nodes children of internal node
		newNode->left = nodeOne;
		newNode->right = nodeTwo;

		// Push internal node back onto priority queue
		pq.push(newNode);
	}

	// Set root of binary tree
	NodePointer topNode = pq.top();
	rootNode = topNode;

	// Build encoding table
	buildEncodingTable(rootNode, Bits());
}

// Builds the encoding table using the tree
void Huffman::buildEncodingTable(Huffman::NodePointer np, Bits BITS) {
	// We can just check for one side because a binary tree will
	// always have a leaf or a child on either side
	if (np->left != NULL){
		Bits leftBITS = BITS;
		Bits rightBITS = BITS;

		// BIT string to save
		leftBITS.addBit(0);
		rightBITS.addBit(1);

		buildEncodingTable(np->left, leftBITS);
		buildEncodingTable(np->right, rightBITS);
	}
	// Reaching the char, we save the BITS it took to get there
	else {
		encodingTable[np->data] = BITS;
	}
};

// Recursive function to print Huffman tree
void Huffman::printTree(Huffman::NodePointer np, int offset)
{
	if (np){
		printTree(np->right, offset + 4);
		cout << setw(offset) << ' ' << np->data << "\n";
		printTree(np->left, offset + 4);
	}
}

// Use this function to properly display the tree
void Huffman::displayTree()
{
	cout << "Huffman Binary Tree\n";
	cout << "-------------------\n\n";
	printTree(rootNode, 0);
	cout << "\n";
}

// Use this function to properly display the encoding table
void Huffman::displayTable() {
	cout << "Huffman Encoding Table\n";
	cout << "-------------------\n\n";
	for (auto it = encodingTable.begin(); it != encodingTable.end(); ++it){
		cout << it->first << ": ";
		it->second.displayBits();
		cout << "\n";
	}
	cout << "\n";
};

// Encodes the tree into bits
Huffman::Bits Huffman::encodeTree(NodePointer np, Bits BITS){
	Bits returnBITS = BITS;

	if (np->left != NULL){
		// 0 indicates a parent node
		returnBITS.addBit(0);

		returnBITS = encodeTree(np->left, returnBITS);
		returnBITS = encodeTree(np->right, returnBITS);
	}
	else {
		// 1 indicates a leaf node and the need to examine
		// char
		returnBITS.addBit(1);
		returnBITS.addChar(np->data);
	}

	return returnBITS;
};

void Huffman::encode(vector<char> & input, fstream & output)
{
	Bits BITS = Bits();
	char charToAdd;

	// Magic number to add
	charToAdd = 13;
	BITS.addChar(charToAdd);

	// Encode Huffman Tree
	BITS.mergeBits(encodeTree(rootNode, Bits()));

	// Encode message
	for (int i = 0; i < input.size(); i++){
		BITS.mergeBits(encodingTable[input.at(i)]);
	}

	// Add artificial end of symbol
	Bits endBITS = encodingTable[EOF];
	BITS.mergeBits(endBITS);

	// Convert binary bits into chars to be transferred back into output
	while (BITS.good()){
		output.put(BITS.getChar());
	}
}

// build Decoding Tree is takes in BIT stream to create it
// We can take advantage of the fact that binary trees do not
// have NULL leaves.  We can rely on this fact.
void Huffman::buildDecodingTree(NodePointer np, Bits & BITS) {
	// If it's a leaf node
	if (BITS.getBit()) {
		np->data = BITS.getChar();
	}
	// If it's a parent node
	else {
		np->left = new Node('-');
		buildDecodingTree(np->left, BITS);
		np->right = new Node('-');
		buildDecodingTree(np->right, BITS);
	}
}

void Huffman::decode(vector<char> & input, fstream & output)
{
	Bits BITS = Bits();

	// PROBLEM IS HERE
	for (int i = 1; i < input.size(); i++){
		BITS.addChar(input.at(i));
	}

	// Builds the decoding tree from bits
	buildDecodingTree(rootNode, BITS);

	// Take in next bits and traverse the tree
	NodePointer pointer = rootNode;
	while (1){
		// If you hit a leaf
		if (pointer->left == NULL || pointer->right == NULL){
			// Get the char
			char c = pointer->data;
			// Check if it's EOF
			if (c == EOF){
				return;
			};
			// Print out the char
			output.put(c);
			// reset pointer to rootNode
			pointer = rootNode;
		}
		else {
			// Determines which branch to take based off current bit
			if (BITS.getBit()){
				pointer = pointer->right;
			}
			else{
				pointer = pointer->left;
			}
		}
	}
}

