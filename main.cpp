#include <stdio.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <time.h>
using namespace std;

#include "Huffman.h"
#include "Lempzev.h"

// Shows resulting output stream in chars
#define SHOW_OUTPUT_STREAM false
// Shows Huffman Tree for both encoding and decoding
#define SHOW_HUFFMAN_TREE false
// Shows Encoding table for Huffman when encoding
#define SHOW_ENCODING_TABLE false

void createFile(string name){
	ofstream newFile(name, ios::out);
	newFile.close();
}

int main()
{
	bool running = true;
	
	while (running){
		system("CLS");

		// User prompt
		cout << "Enter command followed by filename\n\n";
		cout << "i.e. HUFF file1\n";
		cout << "     LZ1 file1\n";
		cout << "     LZ2 file1\n";
		cout << "     EXPAND file1\n";
		cout << "     COMPARE file1 file2\n";
		cout << "     EXIT\n";
		cout << "If file1 or file2 is not found, standard input stream will be used.\n\n";
		cout << "Enter command: ";

		string input;
		string command;
		string filename1;
		string filename2;

		// Get user input
		getline(cin, input);

		// split user input into separate categories
		istringstream inputStream(input);

		inputStream >> command;
		inputStream >> filename1;
		inputStream >> filename2;

		// EXIT:
		// allows user to leave program
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);
		if (command == "exit") {
			cout << "Exiting program\n";
			running = false;
		}

		// These set of commands are used for compression and decoding
		// They have their own "else if" block for character reading purposes
		else if (command == "huff" || command == "lz1" || command == "lz2" || command == "expand") {
			// We open our input file into our filestream
			fstream infile;
			fstream outfile;
			infile.open(filename1, ios::binary|ios::out|ios::in|ios::ate);

			// If the file does not exist
			if (!infile.is_open()) {
				// We close the filestream
				infile.close();
				// Allow user to input the content
				cout << "'" << filename1 << "' file is not found.\n\nPlease enter input stream:\n";
				getline(cin, input);
				// Create a new file called inputStream
				filename1 = "inputStream";
				createFile(filename1);
				// Open new filestream with new file "inputStream"
				// and input the content into the file
				infile.open(filename1);
				infile << input;
			}
			infile.close();
			infile.open(filename1, ios::binary | ios::out | ios::in | ios::ate);

			// Create a second file to take in the output of whatever operation we perform
			filename2 = filename1 + "-c";
			createFile(filename2);
			// Open new filestream for output file
			outfile.open(filename2, ios::binary | ios::out | ios::in | ios::ate);

			// Prepare vector of chars to work with
			ifstream::pos_type pos = infile.tellg();
			vector<char>  charString(pos);
			// TODO: what should first arg be?

			infile.seekg(0, ios::beg);
			infile.read(&charString[0], pos);

			// Timer for analysis purpose
			clock_t begin, end;
			begin = clock();

			// HUFF
			cout << "\n";
			if (command == "huff"){
				cout << "Starting Huff Operation: \n\n";
				
				Huffman h;
				h.buildHuffman(charString);
				if (SHOW_HUFFMAN_TREE) h.displayTree();
				if (SHOW_ENCODING_TABLE) h.displayTable();
				h.encode(charString, outfile);
			}

			// LZ1
			else if (command == "lz1"){
				cout << "Starting LZ1 Operation: \n";
				Lempzev l;
				l.encode(charString, outfile, 1);
			}

			// LZ2
			else if (command == "lz2"){
				cout << "Starting LZ2 Operation: \n";
				Lempzev l;
				l.encode(charString, outfile, 2);
			}

			// EXPAND
			else if (command == "expand"){
				cout << "Starting EXPAND Operation: \n";
				char compressionType = charString.at(0);

				// Inverse Huffman
				if (compressionType == 13){
					cout << "Performing Inverse Huffman: \n";
					Huffman h;
					h.decode(charString, outfile);
					if (SHOW_HUFFMAN_TREE) h.displayTree();
				}
				// Inverse LPZ1
				else if (compressionType == 17){
					cout << "Performing Inverse LPZ1: \n";
					Lempzev l;
					l.decode(charString, outfile, 1);
				}
				// Inverse LPZ2
				else if (compressionType == 19){
					cout << "Performing Inverse LPZ2: \n";
					Lempzev l;
					l.decode(charString, outfile, 2);
				}
				else{
					cout << "File does not match available decompression types.\n";
				}
			}

			// Stop timer
			end = clock();

			// Write the resulting string into outfile
			// We include this within the loop
			infile.close();
			outfile.close();

			// If user wants to see output stream, we open outfile and display result
			if (SHOW_OUTPUT_STREAM) {
				outfile.open(filename2, ios::binary | ios::out | ios::in | ios::ate);

				// Prepare vector of chars to work with
				ifstream::pos_type pos = outfile.tellg();
				vector<char>  charString(pos);
				// TODO: what should first arg be?

				outfile.seekg(0, ios::beg);
				outfile.read(&charString[0], pos);

				cout << "Output Stream:\n";
				for (unsigned int i = 0; i < charString.size(); i++){
					cout << charString.at(i);
				}
				cout << "\n\n";
				outfile.close();
			}

			// Analysis of result of program
			float time_elapsed = (double)((end - begin) / CLOCKS_PER_SEC);
			float infileSize;
			float outfileSize;
			float percentChange;

			// Obtain size of infile
			infile.open(filename1);
			infile.seekg(0, fstream::end);
			infileSize = infile.tellg();
			infile.close();

			// Obtain size of outfile
			outfile.open(filename2);
			outfile.seekg(0, fstream::end);
			outfileSize = outfile.tellg();
			outfile.close();

			// Calculate change from original file to new file
			percentChange = (1 - outfileSize / infileSize) * 100;
			percentChange = ((int)(percentChange * 100 + .5) / 100.0);
			
			// Output Analysis
			cout << "\n---------------\n";
			cout << "'" << filename1 << "' size: " << infileSize << " bytes\n";
			cout << "'" << filename2 << "' size: " << outfileSize << " bytes\n";
			cout << "Compression size saving: " << percentChange << "%\n";
			cout << "Average time: " << time_elapsed << " seconds\n";
		}

		// COMPARE:
		// compares two files to ensure they are exactly the same.
		// Primarily used for debugging purposes
		else if (command == "compare") {
			ifstream stream1(filename1);
			ifstream stream2(filename2);
			bool equal = true;

			// if both files are open
			if (stream1.is_open() && stream2.is_open()) {
				// traverse through all the files
				while (stream1.good() || stream2.good()) {
					// compare character by character
					if (stream1.get() != stream2.get()) {
						equal = false;
						break;
					}
				}
				// output result
				cout << "'" << filename1 << "' and '" << filename2 << "' are ";
				if (!equal) cout << "not ";
				cout << "equal.\n";
			}
			// otherwise, notify which files are not open
			else {
				if (!stream1.is_open())
					cout << "'" << filename1 << "' file is not found.\n";
				if (!stream2.is_open())
					cout << "'" << filename2 << "' file is not found.\n";
			}

			stream1.close();
			stream2.close();
		}

		// ERROR:
		// case for when input is unknown
		else {
			cout << "Incorrect command.  Please try again.\n";
		}

		// pause for examination
		cout << "\n";
		system("PAUSE");
	}
}
