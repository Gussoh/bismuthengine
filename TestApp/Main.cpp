#include <iostream>
#include <vector>
#include "Test.h"
#include "BasicOutput.h"

typedef std::vector<Test*> TestList;
TestList tests;

void setupTests() {
	tests.push_back(new BasicOutput());
}

void runTests() {
	for (TestList::iterator iter = tests.begin(); iter != tests.end(); ++iter) {
		(*iter)->run();
	}
}

void clearTests() {
	for (TestList::iterator iter = tests.begin(); iter != tests.end(); ++iter) {
		delete (*iter);
	}
}

int main(int, char**) {
	std::cout << "BismuthEngine TestApp" << std::endl;

	setupTests();
	runTests();
	clearTests();

	return 0;
}
