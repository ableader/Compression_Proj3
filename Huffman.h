#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#ifndef HUFFMAN
#define HUFFMAN
class Huffman
{
private:
	// Node class
	// Every node has a priority
	class Node
	{
	public:
		int frequency;
		char data;
		Node *left,
			 *right;

		// Node constructor
		Node(char item, int number)
		{
			frequency = number;
			data = item;
			left = NULL;
			right = NULL;
		}
	};
	typedef Node * NodePointer;

public:
	Huffman();

	void buildHuffmanTree(stringstream & input);

	void printTree(ostream & out, NodePointer root, int indent);

	void displayHuffmanTree(ostream & out);

	void encode(ifstream & messageIn);

	void decode(ifstream & messageIn);

private:
	NodePointer myRoot;
};

inline Huffman::Huffman()
{
	myRoot = new Node('-', 0);
}

inline void Huffman::displayHuffmanTree(ostream & out)
{
	printTree(out, myRoot, 0);
}
#endif