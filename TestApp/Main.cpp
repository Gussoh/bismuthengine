#include "stdafx.h"
#include "Test.h"
#include "BasicOutput.h"
#include "AudioTest.h"
#include "RendererTest.h"

typedef std::vector<Test*> TestList;
TestList tests;

void setupTests() {
	tests.push_back(new BasicOutput());
	//tests.push_back(new AudioTest());
	tests.push_back(new RendererTest());
}

void runTests() {
	for (TestList::iterator iter = tests.begin(); iter != tests.end(); ++iter) {
		(*iter)->run();
	}
}

void clearTests() {
	int a;
	for (TestList::iterator iter = tests.begin(); iter != tests.end(); ++iter) {
		delete (*iter);
	}

	std::cin >> a;
}

int main(int, char**) {
	std::cout << "BismuthEngine TestApp" << std::endl;

	setupTests();
	runTests();
	clearTests();

	return 0;
}
