#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include <sstream>

using namespace std;

// initialize child parent processes
void child ();
void parent();
// initialize C String
char * commandString[3];

// tokenizes a character Array into a vector of strings
vector <string> getTokenVector(string userLine){
	vector<string> tmpVec;

	stringstream streamObj = stringstream (userLine);
	string currentToken;

	while(streamObj>> currentToken)
		tmpVec.push_back(currentToken);
	return tmpVec;
}

/*
* Main does this
* add tokenizer
* put tokenized items in command line
* uses dup2 for output
*/

int main(){
	// inputs command
	string userLine;
	cout<<"Enter the first command:\n";
	getline(cin, userLine);
	// breaks down line by tokenizer
	vector<string> cmd1TokenVector= getTokenVector(userLine);

	string command= cmd1TokenVector[0]; // this is the ls unix command
	string argument1= cmd1TokenVector[1]; // this is an argument for the ls

	commandString[0]=(char*) command.c_str(); //this is how to convert a string variable to a c_string
	commandString[1]=(char*) argument1.c_str();
	commandString[2]=NULL;

	// forks into 2 processes
	pid_t pid;
	pid= fork();
	if (pid== 0) // a child was created and you are inside it
		child();
	else if (pid< 0) // no new child was created (fail)
	{
		cout<<"No New Child Was created \n";
		return 1; // 1 is something bad happended
	}
	else // must be the parent
	{
		wait(0);
		parent();
	}
	return 0;
}

// runs if child process
void child (){
	int fd = open("outputredir.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	dup2(fd,1);

	// print statement if child process is invalid
	if( execvp(commandString[0], commandString) < 0 ){
		cout<< "Sorry. Couldn't change the execution image for the child process"<<endl;
	}

	string msg = "This statement will never be executed, if execvp was successfull";
	execvp(commandString[0],commandString);
	write(fd, (char *)msg.c_str(), msg.size());
}

// runs if parent process
void parent(){
	int fd = open("outputredir.txt", O_WRONLY | O_APPEND);
	dup2(fd,1);
	string tempString = "Back to the parent\n";
	write(fd, (char *)tempString.c_str(), tempString.size());
}




