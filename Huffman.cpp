#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <vector>
#include <bitset>
#include <unordered_map>
using namespace std;
#include "Huffman.h"
#define EOF -1

void Huffman::buildHuffman(fstream & input)
{
	// Obtain frequencies of all chars from filestream
	unordered_map<char, int> frequencyTable = unordered_map<char, int>();
	while (input.good())
	{
		char c = input.get();
		if (input.good()) {
			frequencyTable[c]++;
		}
	}

	// Add in artificial end of file symbol
	frequencyTable[EOF] = 1;

	/*
	int sum = 0;
	for (auto it = frequencyTable.begin(); it != frequencyTable.end(); ++it){
		sum += it->second;
		cout << it->first << ": " << it->second << ": " << sum << "\n";
	}
	cout << "Total sum of symbols: " << sum << "\n";
	*/

	// Reset pointer in filestream
	input.clear();
	input.seekg(0);
	
	// Create leaf Nodes for each char
	priority_queue<Node*, vector<Node*>, MyComparator > pq;
	for (auto it = frequencyTable.begin(); it != frequencyTable.end(); ++it){
		pq.push(new Node(it->first, it->second));
	}

	// Populate binary tree
	while (pq.size() > 1){
		NodePointer nodeOne = pq.top();
		int nodeOneFrequency = nodeOne->frequency;

		pq.pop();
		NodePointer nodeTwo = pq.top();
		int nodeTwoFrequency = nodeTwo->frequency;

		pq.pop();
		// Create internal node
		NodePointer newNode = new Node('-', nodeOneFrequency + nodeTwoFrequency);

		//cout << newNode->frequency << "\n";
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

void Huffman::buildEncodingTable(Huffman::NodePointer np, Bits BITS) {
	if (np->left != NULL){
		Bits leftBITS = BITS;
		Bits rightBITS = BITS;

		leftBITS.addBit(0);
		rightBITS.addBit(1);

		buildEncodingTable(np->left, leftBITS);
		buildEncodingTable(np->right, rightBITS);
	}
	else {
		encodingTable[np->data] = BITS;
	}
};

void Huffman::printTree(Huffman::NodePointer np, int offset)
{
	if (np){
		printTree(np->right, offset + 4);
		cout << setw(offset) << ' ' << np->data << "\n";
		printTree(np->left, offset + 4);
	}
}

void Huffman::displayTree()
{
	cout << "Huffman Binary Tree\n";
	cout << "-------------------\n\n";
	printTree(rootNode, 0);
	cout << "\n";
}

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

Huffman::Bits Huffman::encodeTree(NodePointer np, Bits BITS){
	Bits returnBITS = BITS;

	if (np->left != NULL){
		returnBITS.addBit(0);

		returnBITS = encodeTree(np->left, returnBITS);
		returnBITS = encodeTree(np->right, returnBITS);
	}
	else {
		returnBITS.addBit(1);
		returnBITS.addChar(np->data);
	}

	return returnBITS;
};

string Huffman::encode(fstream & input)
{
	string code;
	Bits BITS = Bits();
	char charToAdd;

	// Magic number to add
	charToAdd = 13;
	BITS.addChar(charToAdd);

	// Encode Huffman Tree
	BITS.mergeBits(encodeTree(rootNode, Bits()));

	// Encode message
	while (input.good())
	{
		char c = input.get();
		if (input.good()) {
			BITS.mergeBits(encodingTable[c]);
		}
	}

	// Add artificial end of symbol
	Bits endBITS = encodingTable[EOF];
	BITS.mergeBits(endBITS);

	// Convert binary bits into chars to be transferred back into string
	while (BITS.good()){
		code.push_back(BITS.getChar());
	}

	// Reset pointer in filestream
	input.clear();
	input.seekg(0);

	return code;
}

void Huffman::buildDecodingTree(NodePointer np, Bits & BITS) {
	//BITS.displayBits();
	if (BITS.getBit()) {
		np->data = BITS.getChar();
	}
	else {
		np->left = new Node('-');
		buildDecodingTree(np->left, BITS);
		np->right = new Node('-');
		buildDecodingTree(np->right, BITS);
	}
}

string Huffman::decode(fstream & input)
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

	buildDecodingTree(rootNode, BITS);
	displayTree();

	NodePointer pointer = rootNode;
	while (1){
		if (pointer->left == NULL || pointer->right == NULL){
			char c = pointer->data;
			if (c == EOF){
				break;
			};
			resultToWrite.push_back(c);
			pointer = rootNode;
		}
		else {
			if (BITS.getBit()){
				pointer = pointer->right;
			}
			else{
				pointer = pointer->left;
			}
		}

		//cout << resultToWrite << "\n";
	}

	// Reset pointer in filestream
	input.clear();
	input.seekg(0);

	return resultToWrite;
}

