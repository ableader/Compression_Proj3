#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
using namespace std;
#include "Huffman.h"

#define LOOP_TIMES 5; 

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
		if (command == "EXIT") {
			cout << "Exiting program\n";
			running = false;
		}

		// These set of commands are used for compression and decoding
		// They have their own "else if" block for character reading purposes
		else if (command == "HUFF" || command == "LZ1" || command == "LZ2" || command == "EXPAND") {
			// We open our input file into our filestream
			fstream infile(filename1);

			// If the file does not exist
			if (!infile.is_open()) {
				// We close the filestream
				infile.close();
				// Allow user to input the content
				cout << "'" << filename1 << "' file is not found.  Please enter input stream:\n";
				getline(cin, input);
				// Create a new file called inputStream
				filename1 = "inputStream";
				createFile(filename1);
				// Open new filestream with new file "inputStream"
				// and input the content into the file
				infile = fstream(filename1);
				infile << input;
			}

			// Create a second file to take in the output of whatever operation we perform
			filename2 = filename1 + ".result";
			createFile(filename2);
			// Open new filestream for output file
			fstream outfile(filename2);

			// Prepare string to input into outfile
			string resultToWrite;

			// Timer for analysis purpose
			clock_t begin, end;
			begin = clock();

			// Loop for timing accuracy purposes
			// for (int i = 0; i < LOOP_TIMES; i++) {
			// HUFF
			cout << "\n";
			if (command == "HUFF"){
				cout << "Starting Huff Operation: \n";

			}

			// LZ1
			else if (command == "LZ1"){
				cout << "Starting LZ1 Operation: \n";

			}

			// LZ2
			else if (command == "LZ2"){
				cout << "Starting LZ2 Operation: \n";

			}

			// EXPAND
			else if (command == "EXPAND"){
				cout << "Starting EXPAND Operation: \n";

			}
			//}

			// Stop timer
			end = clock();

			// Write the resulting string into outfile
			// We include this within the loop
			outfile << resultToWrite;

			// Analysis of result of program
			float time_elapsed = (double)(end - begin) / CLOCKS_PER_SEC / LOOP_TIMES;
			float infileSize;
			float outfileSize;
			float percentChange;

			// Obtain size of infile
			infile.seekg(0, ifstream::end);
			infileSize = infile.tellg();

			// Obtain size of outfile
			outfile.seekg(0, ifstream::end);
			outfileSize = outfile.tellg();

			// Calculate change from original file to new file
			percentChange = (outfileSize / infileSize) * 100;

			// Close for safety reasons
			infile.close();
			outfile.close();
			
			// Output Analysis
			cout << "\n---------------\n";
			cout << "'" << filename1 << "' size: " << infileSize << '\n';
			cout << "'" << filename2 << "' size: " << outfileSize << '\n';
			cout << "Percentage compression saving: " << percentChange << "%\n";
			cout << "Time needed to perform operation: " << time_elapsed << "\n";
		}

		// COMAPRE:
		// compares two files to ensure they are exactly the same.
		// Primarily used for debugging purposes
		else if (command == "COMPARE") {
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
