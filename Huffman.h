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
	// Every node has a priority, data, left node and right node
	class Node
	{
	public:
		int frequency;
		char data;
		Node *left,
			 *right;

		// Node constructor
		// requires a char and a number
		Node(char item, int number)
		{
			frequency = number;
			data = item;
			left = NULL;
			right = NULL;
		}

		Node(char item)
		{
			frequency = 0;
			data = item;
			left = NULL;
			right = NULL;
		}
	};

	// For simplicity sake we add a NodePointer
	typedef Node * NodePointer;

	// Used to compare within priority queue
	struct MyComparator {
		bool operator() (Node* one, Node* two) {
			return one->frequency > two->frequency;
		}
	};

	// Bits is used to manipulate bits within Huffman Encoding
	class Bits
	{
	public:
		vector<bool> BITS;
		int pointer;
		int size;

		// Bits constructor
		Bits()
		{
			BITS = vector<bool>();
			pointer = 0;
			size = 0;
		}

		// Reset the pointer of Bits back to start
		void reset() {
			pointer = 0;
		}

		// Notifies us if there are still bits we can access
		bool good() {
			return pointer < size;
		}

		// Returns current bit and moves pointer
		bool getBit() {
			if (pointer < size){
				bool toReturn = BITS[pointer];
				pointer++;
				return toReturn;
			}
			else{
				cout << "ERROR: bit not found\n";
				system("PAUSE");
				exit(1);
			}
		}

		// Return next char if there are any bits available
		// If there are not enough bits, we fill in the remaining bits with 0
		char getChar(){
			if (pointer < size){
				char c = 0;
				bool bits[8];

				for (int i = 0; i < 8; i++){
					if (good()){
						bits[i] = getBit();
					}
					else {
						bits[i] = 0;
					}
				}

				if (bits[0]){
					c = -128;
				}

				for (int i = 1; i < 8; i++)
				{
					if (bits[i])
						c += pow(2, 7-i);
				}

				return c;
			}
			else{
				cout << "ERROR: char not found\n";
				system("PAUSE");
				exit(1);
			}
		}

		// Add a character and turn it into bits
		void addChar(char c){
			if (c < 0){
				addBit(1);
				c += 128;
			}
			else {
				addBit(0);
			}

			for (int i = 6; i >= 0; i--){
				int currentPower = pow(2, i);
				if (c >= currentPower){
					addBit(1);
					c = c - currentPower;
				}
				else {
					addBit(0);
				}
			}
		}

		// Add a single bit
		void addBit(bool b){
			BITS.push_back(b);
			size++;
		}

		// Merge new bits to current Bit
		void mergeBits(Bits b) {
			while (b.good()) {
				addBit(b.getBit());
			}
		}

		void displayRemainingBits() {
			for (int i = pointer; i < size; i++){
				cout << BITS[i];
			}
		}

		// Display all current bits in BITS
		void displayBits() {
			for (int i = 0; i < size; i++){
				cout << BITS[i];
			}
		}
	};

public:
	// Construct new Huffman
	Huffman();

	// Build the huffman tree given an input
	// Also builds the encoding table corresponding with tree
	void buildHuffman(fstream & input);

	// Recursive function that takes huffman tree and creates an encoding table
	void buildEncodingTable(NodePointer np, Bits BITS);

	// Recursive function used in display tree
	void printTree(NodePointer root, int offset);

	// Displays the Huffman tree
	void displayTree();

	// Displays the encoding table
	void displayTable();

	// Encode Huffman tree
	Bits encodeTree(NodePointer np, Bits BITS);

	// Encode the incoming message using current HUffmantree
	string encode(fstream & input);

	void buildDecodingTree(NodePointer np, Bits & BITS);

	// Decode incoming message
	// will rebuild Huffman tree and analyze incoming data
	string decode(fstream & input);

private:
	NodePointer rootNode;
	unordered_map<char, Bits> encodingTable;
};

// Constructor for Huffman
inline Huffman::Huffman()
{
	rootNode = new Node('-', 0);
	encodingTable = unordered_map<char, Bits>();
}
#endif