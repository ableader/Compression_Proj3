#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include <cmath>
#ifndef LEMPZEV
#define LEMPZEV
using namespace std;

class Lempzev
{
	class Token {
	public:
		int len, offset, code, strLen;
		vector<char> chars;
		bool isDouble;

		Token(){};

		// Token for double
		Token(int l, int o)
		{
			isDouble = true;
			len = l;
			offset = o;
			code = -1;
			strLen = -1;
			chars = vector<char>();
		}

		// Token for triple
		Token(int c, int sl, char ch){
			isDouble = false;
			len = -1;
			offset = -1;
			code = c;
			strLen = sl;
			chars = vector<char>();
			chars.push_back(ch);
		}

		// Only used for Token Triple
		void mergeToken(Token t) {
			strLen += t.strLen;
			for (int i = 0; i < t.chars.size(); i++){
				chars.push_back(t.chars.at(i));
			}
		}
	};

	// Bits is used to manipulate bits within Lempzev Encoding
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
						c += pow(2.0, 7-i);
				}

				return c;
			}
			else{
				cout << "ERROR: char not found\n";
				system("PAUSE");
				exit(1);
			}
		}

		int getInt(int numOfBits){
			int sum = 0;
			for (int i = 0; i < numOfBits; i++){
				if (getBit()){
					sum += pow(2.0, numOfBits - i - 1);
				}
			}
			return sum;
		}

		void addInt(int value, int numOfBits){
			while (--numOfBits >= 0){
				int currentPower = pow(2.0, numOfBits);
				if (value >= currentPower){
					addBit(1);
					value -= currentPower;
				}
				else {
					addBit(0);
				}
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
				int currentPower = pow(2.0, i);
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
				if (i % 8 == 0)
					cout << " ";
				cout << BITS[i];
			}
		}

		// Display all current bits in BITS
		void displayBits() {
			for (int i = 0; i < size; i++){
				if (i % 8 == 0)
					cout << " ";
				cout << BITS[i];
			}
		}
	};

public:
	// Construct new Lempzev
	Lempzev();

	// Encode the incoming message using Lempzev
	void encode(vector<char> & input, fstream & output, int variation);

	// Decode incoming message
	// will rebuild Lempzev tree and analyze incoming data
	void decode(vector<char> & input, fstream & output, int variation);

private:
	unordered_map<char, Bits> encodingTable;
};

// Constructor for Lempzev
inline Lempzev::Lempzev()
{

}
#endif
