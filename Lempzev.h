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
		bool isTokenDouble;
		bool isDouble() {
			return isTokenDouble;
		}
		void mergeToken(Token t){};
	};

	class TokenDouble : public Token {
	public:
		int len, offset;

		// TokenDouble constructor
		TokenDouble(int l, int o)
		{
			isTokenDouble = true;
			len = l;
			offset = o;
		}
	};

	class TokenTriple : public Token {
	public:
		int code, strLen;
		string chars;

		// TokenDouble constructor
		TokenTriple(int c, int l, string str)
		{
			isTokenDouble = false;
			code = l;
			strLen = l;
			chars = str;
		}

		void mergeToken(TokenTriple t) {
			strLen += t.strLen;
			chars += t.chars;
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
	// Construct new Lempzev
	Lempzev();

	// Encode the incoming message using Lempzev
	void encode(fstream & input, fstream & output, int variation);

	// Decode incoming message
	// will rebuild Lempzev tree and analyze incoming data
	void decode(fstream & input, fstream & output, int variation);

private:
	unordered_map<char, Bits> encodingTable;
};

// Constructor for Lempzev
inline Lempzev::Lempzev()
{

}
#endif
