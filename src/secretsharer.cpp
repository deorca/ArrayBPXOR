//
//  secretsharer.cpp
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 06/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#include <string>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include "boost/filesystem.hpp"
#include "blocksecretsharer.hh"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


using namespace std;

// Prints help-message
void print_help(bool unknown_command);

// Create a new key
void create_key();

// Create shares of a file
bool create_file_shares(string filepath, string output_message);

// Reconstruct a file from shares
bool reconstruct_file_from_shares(string filepath, string output_message);

// Gets contents of a file, based upon file-path
unsigned char * get_file_contents(string filepath, int * size_read);

// Puts a buffer to a file, based upon
void put_file_contents(string filepath, unsigned char * contents, int length);

// Gets current path of the running exe
int get_current_path(char * current_path, int length);

// Creates a share-name based upon the input filename
string create_share_name(string filename, int share_index);

struct passwd *pw = getpwuid(getuid());
const string homedir = pw->pw_dir;
const string _key_file_name = "/.ssk";
const string _key_file_path = homedir + _key_file_name;

const int _max_message_length = 100;


/* The format for this command-line tool is as follows -
 *  
 *  secretsharer -h                 : displays the help-mesage
 *  secretsharer -k                 : create new key. Keys will be
 *                                  : - stored in file 
 *                                  :   ./.key
 *
 *  secretsharer -c <file-path>     : creates shares from the file
 *                                  : - identified by <file-path>
 *
 *  secretsharer -r <file-path>     : reconstructs a file from shares
 *                                  : - of a file based on the format
 *                                  : - of <file-path>. Format will 
 *                                  : - be simple -
 *                                  :       <file-path>_%
 *                                  : - where % is the index of the 
 *                                  : - share-file
 */
int main(int argc, char * argv[])
{
    string output_message;
    int readbytes = 0;
    unsigned char * bytesread;
    string arg2;
    
    // Check there are enough arguments
    if (argc == 2 && argv[1][0] == '-')     // argument must begin with a dash, '-'
    {
        switch (argv[1][1])
        {
            case 'h':
                print_help(false);
                break;
                
            case 'k':
                create_key();
                break;
                
            default:
                print_help(true);
                break;
        }
    }
    // first argument only must begin with a dash, '-'
    else if (argc == 3 && argv[1][0] == '-' && argv[2][0] != '-')
    {
	arg2 = argv[2];
        switch (argv[1][1])
        {
            case 'c':
                create_file_shares(arg2, output_message);
                break;
                
            case 'r':
                reconstruct_file_from_shares(arg2, output_message);
                break;
                
            case 'g':
                bytesread = get_file_contents(arg2, &readbytes);
                cout << "\nget_file_contents operation read " << readbytes << " bytes from file '" << arg2 << "'\n";
                cout << "bytes read: '" << bytesread << "'\n";
                break;
                
            default:
                print_help(true);
                break;
        }
        
    }
    // first argument only must begin with a dash, '-'
    else if (argc == 4 && argv[1][0] == '-' && argv[2][0] != '-' && argv[3][0] != '-')
    {
	arg2 = argv[2];
        switch (argv[1][1])
        {
            case 'p':
                put_file_contents(arg2, (unsigned char *)argv[3], sizeof(argv[3]));
                break;
                
            default:
                print_help(true);
                break;
        }
    }
    else
    {
        // Print help message
        print_help(argc != 1);
    }
    
    return (0);
}


// Prints help-message
void print_help(bool unknown_command)
{
    if (unknown_command)
    {
        cout << "UNKNOWN COMMAND\n\n";
    }

    cout << "\n\nThe format for this command-line tool is as follows -\n";

    cout << "  secretsharer -h                 : displays the help-mesage\n";
    cout << "  secretsharer -k                 : create new key. Keys will be\n";
    cout << "                                  : - stored at\n";
    cout << "                                  :   ./.key\n\n";
    cout << "  secretsharer -c <file-path>     : creates shares from the file\n";
    cout << "                                  : - identified by <file-path>\n\n";
    cout << "  secretsharer -r <file-path>     : reconstructs a file from shares\n";
    cout << "                                  : - of a file based on the format\n";
    cout << "                                  : - of <file-path>. Format will\n";
    cout << "                                  : - be simple -\n";
    cout << "                                  :       <file-path>_%\n";
    cout << "                                  : - where % is the index of the\n";
    cout << "                                  : - share-file\n";

}

// Create a new key
void create_key()
{
    //cout << "\ncreate_key() called\n\n";
    
    unsigned char * buffer;
    int length = BlockSecretSharer::KeyLength();

    ISecretSharer * secretSharer = new BlockSecretSharer();

    buffer = secretSharer->Key();

    put_file_contents(_key_file_path, buffer, length);

    delete secretSharer;
}

// Create shares of a file
bool create_file_shares(string file_path, string output_message)
{
    bool success = false;
    
    unsigned char * buffer;
    int length = 0;
    string share_name;

    // Ensure input-file exists
    buffer = get_file_contents(file_path, &length);


    if (length > 0)
    {
	// Get key and set up sharer
	BlockSecretSharer * sharer = NULL;
	int read_size = 0;
	unsigned char * key = get_file_contents(_key_file_path, &read_size);
	if (read_size > 0)
	{
	    sharer = new BlockSecretSharer(key);
	    delete[] key;
	}
	else
	{
	    sharer = new BlockSecretSharer();
	}
	
	map<int, vector<unsigned char> > shares = sharer->CreateShares(buffer, length, 4);
	
	delete[] buffer;
	delete sharer;

	for (int i = 1; i <= 4; i++)
	{
	    share_name = create_share_name(file_path, i);
	    put_file_contents(share_name, shares[i].data(), length);
	}
	success = true;
    }
    
    return success;
}

// Reconstruct a file from shares
bool reconstruct_file_from_shares(string file_path, string output_message)
{
    bool success = false;

    // Look for locally-stored key
    BlockSecretSharer * sharer = NULL;
    int read_size = 0;
    unsigned char * key = get_file_contents(_key_file_path, &read_size);
    if (read_size > 0)
    {
	sharer = new BlockSecretSharer(key);	// Key found, use for this operation
	delete[] key;

	map<int, vector<unsigned char> > shares;
	unsigned char * buffer;
	int length;

	// Get at least 2 shared files from local directory
	for (int i = 1; i <= 4; i++)
	{
	    length = 0;
	    string share_name = create_share_name(file_path, i);
	    buffer = get_file_contents(share_name, &length);

	    if (length > 0)
	    {
		shares[i] = vector<unsigned char>(buffer, buffer + length);
		delete[] buffer;
		if (shares.size() >= 2)
		    break;
	    }
	}
	
	if (shares.size() >= 2)
	{
	    // Found at least 2 shared files from local directory. Do reconstruction
	    unsigned char * reconstructed_file = sharer->ReconstructSecret(shares, length, length);

	    // Write reconstructed data to original filename.
	    put_file_contents(file_path, reconstructed_file, length);
	    
	    delete[] reconstructed_file;
	    
	    success = true;
	}
	else
	{
	    cout << "\n\nNo shared files found.\n\n";
	}
	delete sharer;
    }
    else
    {
	cout << "\n\nNo key found, reconstruction would fail!\n\n";
	return false;
    }
    
    return success;
}

// Gets contents of a file, based upon file-path, returns length of buffer
unsigned char * get_file_contents(string file_path, int * read_size)
{
    
    streampos size;
    char * memblock;
    
    ifstream file (file_path.c_str(), ios::in|ios::ate|ios::binary); // Set position to end of file
    if (file.is_open())
    {
        size = file.tellg();        // Position is at the end of file, so gives us file-size
        memblock = new char[size];
        file.seekg (0, ios::beg);
        file.read(memblock, size);
        file.close();
        
        *read_size = size;
    }
    else cout << "\n\nUnable to open file '" << file_path << "'\n\n";
    
    return (unsigned char *)memblock;
}

// Puts a buffer to a file, based upon file-path
void put_file_contents(string file_path, unsigned char * contents, int length)
{
    //cout << "\nput_file_contents() called\n\n";
    
    char * memblock;
    
    ofstream file (file_path.c_str(), ios::out|ios::binary);
    if (file.is_open())
    {
        memblock = (char *)contents;
        file.write(memblock, length);
        file.close();
    }
    else cout << "\n\nUnable to open file '" << file_path << "'\n\n";
}

// Gets current path of the running exe
int get_current_path(string current_path)
{
    int length = current_path.length();
    char * c_str = new char [length+1];
    strcpy(c_str, current_path.c_str());

    if (!GetCurrentDir(c_str, length))
    {
	return errno;
    }
    return (0);
}

// Creates a share-name based upon the input filename
string create_share_name(string filename, int share_index)
{
    string sharename;

    // Split extension and basename from filename
    std::string::size_type pos;
    string extensionmarker(".");
    pos = filename.find_last_of(extensionmarker);
    char char_share_index = '0' + share_index;

    if (pos == string::npos || pos == 0)
    {
	// No match found or dot found at first position, use filename as basename
	sharename = filename;
    }
    else
    {
	// Get basename from filename to position
	sharename = filename.substr(0, pos);
    }

    // Add share-index to basename
    sharename += "_";
    sharename += char_share_index;

    // Add extension to new basename and copy to sharename
    sharename += filename.substr(pos);

    return sharename;
}
