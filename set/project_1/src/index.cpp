#include "api/input.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;


int main(int argc, char* argv[]) {
	if(argc != 2) {
		cout << "Incorrect Usage" << endl;
		cout << "Usage: " << argv[0] << " <input_dir>" << endl;
		return 1;
	}

	vector<string> inputFiles = load_input(argv[1]);
	int parse_success = parse_input(inputFiles, argv[1]);
	if(parse_success) {

	}
	else {
		cout << "Failed to parse...exiting index" << endl;
		return 1;
	}

	return 0;
}
