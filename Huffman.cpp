#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tr1/unordered_map>
using namespace std;
#include "Huffman.h"
//--- Definition of buildDecodingTree()
void Huffman::buildHuffmanTree(stringstream & input)
{
	std::tr1::unordered_map<char, int> frequencyTable = std::tr1::unordered_map<char, int>();
	/*while (input.get())
	{
		char c = input.get();
		if (input.good()) {
			frequencyTable[c]++;
		}
	}
	*/
	std::cout << "mymap contains:";
	for (auto it = frequencyTable.begin(); it != frequencyTable.end(); ++it)
		std::cout << " " << it->first << ":" << it->second;
	std::cout << std::endl;
}

//--- Definition of decode()
void Huffman::decode(ifstream & messageIn)
{
	char bit; // next message bit
	Huffman::NodePointer p; // pointer to trace path in decoding tree
	for (;;)
	{
		p = myRoot;
		while (p->left != 0 || p->right != 0)
		{
			messageIn >> bit;
			if (messageIn.eof()) return;
			cout << bit;
			if (bit == '0')
				p = p->left;
			else if (bit == '1')
				p = p->right;
			else
				cerr << "Illegal bit: " << bit << " -- ignored\n";
		}
		cout << "--" << p->data << endl;
	}
}

//--- Definition of printTree()
void Huffman::printTree(ostream & out, Huffman::NodePointer root,
	int indent)
{
	if (root != 0)
	{
		printTree(out, root->right, indent + 8);
		out << setw(indent) << " " << root->data << endl;
		printTree(out, root->left, indent + 8);
	}
}
