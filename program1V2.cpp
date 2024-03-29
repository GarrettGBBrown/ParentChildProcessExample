
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstring>

using namespace std;

// determines whether input string is within passkeys.txt file
string isPassword(string inputPass) {

	ifstream validPasswords("passkeys.txt");
	string validPassword;
	
	while (getline(validPasswords,validPassword)){
		if(validPassword==inputPass){
		return "Valid Password Detected";
		}
	}
	return "Invalid Password";
}

// uses piping to send data between child and parent processes
// takes in string for password, convert to C String, then compare against list of passwords

int main()
{
    int fd1[2]; // First Pipe
    int fd2[2]; // Second Pipe
    char fixed_str[] = "ERROR";

    pid_t p; // creates process id for both active processes
    
    // inputs password attempt
    string inputString;
    cout << "Enter Password: " << endl;
    cin >> inputString;
    
   // convert string value to C string
   const char* input_c_str = inputString.c_str();
 
    // checks validity of pipes 
    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    // creates fork
    p = fork();

    // checks for valid fork
    if (p < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    }
 
    // Parent process
    else if (p > 0) {
        char concat_str[100];
 
        close(fd1[0]); // Close reading end of first pipe
 
        // Write input string to writing end of first pipe.
	    write(fd1[1], input_c_str, strlen(input_c_str) + 1);

        // close pipe 1 write end
 	    close(fd1[1]);

        // wait for child process for send data
        wait(NULL);
 
        // Wait for child to send a string
 
        close(fd2[1]); // Close writing end of second pipe
 
        // Read string from child, print it and close reading end.
        read(fd2[0], concat_str, 100);
        printf("%s\n", concat_str);
        close(fd2[0]);
    }
 
    // child process
    else {
        close(fd1[1]); // Close writing end of first pipe
 
        // Read a string using first pipe
        char concat_str[100];
	
        // Concatenate a fixed string with it
        int k = strlen(concat_str);
        int i;
        char temp_c_str[100];
        
        // read from reading of first pipe
        read(fd1[0], temp_c_str, 100);

        string tempString = temp_c_str;
    	string results = isPassword(tempString);
    	strcpy(fixed_str, results.c_str()); 
        
        // write value to write of pipe 2 (to parent)
        write(fd2[1], fixed_str, strlen(fixed_str) + 1);
        
        // remove
        for (i = 0; i < strlen(fixed_str); i++)
            concat_str[k++] = fixed_str[i];
 	
        concat_str[k] = '\0'; // string ends with '\0'
 
        // Close read ends
        close(fd1[0]);
        close(fd2[0]);
 
        // Write concatenated string and close writing end
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        
        close(fd2[1]);
 
        exit(0);
    }
}

