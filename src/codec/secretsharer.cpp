//
//  secretsharer.cpp
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 06/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "blocksecretsharer.hh"

using namespace std;

// Prints help-message
void print_help(bool unknown_command);

// Create a new key
void create_key();

// Create shares of a file
bool create_file_shares(char * filepath, char * output_message);

// Reconstruct a file from shares
bool reconstruct_file_from_shares(char * filepath, char * output_message);

// Gets contents of a file, based upon file-path
int get_file_contents(const char * filepath, unsigned char * contents);

// Puts a buffer to a file, based upon
void put_file_contents(const char * filepath, unsigned char * contents, int length);

const char * _key_path = "~/.secretsharer/.key";
const int _max_message_length = 100;

/* The format for this command-line tool is as follows -
 *  
 *  secretsharer -h                 : displays the help-mesage
 *  secretsharer -k                 : create new key. Keys will be
 *                                  : - stored at 
 *                                  :   ~/.secretsharer/key
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
    char output_message[_max_message_length];
    int readbytes = 0;
    
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
    else if (argc == 3 && argv[1][0] == '-' && argv[2][0] != '-') // first argument only must begin with a dash, '-'
    {
        switch (argv[1][1])
        {
            case 'c':
                create_file_shares(argv[2], output_message);
                break;
                
            case 'r':
                reconstruct_file_from_shares(argv[2], output_message);
                break;
                
            case 'g':
                unsigned char * bytesread;
                readbytes = get_file_contents(argv[2], bytesread);
                cout << "\nget_file_contents operation read " << readbytes << " bytes from file '" << argv[2] << "'\n";
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
        switch (argv[1][1])
        {
            case 'p':
                put_file_contents(argv[2], (unsigned char *)argv[3], sizeof(argv[3]));
                break;
                
            default:
                print_help(true);
                break;
        }
    }
    else
    {
        // Print help message
        print_help(true);
    }
    
    return (0);
}


// Prints help-message
void print_help(bool unknown_command)
{
    if (unknown_command)
    {
        cout << "\nprint_help() called\n";
        cout << "UNKNOWN COMMAND\n\n";
    }
    else
    {
        cout << "\nprint_help() called\n\n";
    }
}

// Create a new key
void create_key()
{
    cout << "\ncreate_key() called\n\n";
    
    unsigned char * buffer;
    int length = 0;
    
    put_file_contents(_key_path, buffer, length);
}

// Create shares of a file
bool create_file_shares(char * file_path, char * output_message)
{
    cout << "\ncreate_file_shares() called\n\n";
    
    unsigned char * buffer;
    int length = 0;
    
    get_file_contents(file_path, buffer);
    put_file_contents(file_path, buffer, length);
    
    return true;
}

// Reconstruct a file from shares
bool reconstruct_file_from_shares(char * file_path, char * output_message)
{
    cout << "\nreconstruct_file_from_shares() called\n\n";

    unsigned char * buffer;
    int length = 0;
    
    get_file_contents(file_path, buffer);
    put_file_contents(file_path, buffer, length);
    
    return true;
}

// Gets contents of a file, based upon file-path, returns length of buffer
int get_file_contents(const char * file_path, unsigned char * contents)
{
    /*
    cout << "\nget_file_contents() called\n\n";
    
    return (0);
    */
    
    int read_size = 0;
    
    streampos size;
    char * memblock;
    
    ifstream file (file_path, ios::in|ios::ate|ios::binary); // Set position to end of file
    if (file.is_open())
    {
        size = file.tellg();        // Position is at the end of file, so gives us file-size
        memblock = new char[size];
        file.seekg (0, ios::beg);
        file.read(memblock, size);
        file.close();
        
        contents = (unsigned char *)memblock;
        read_size = size;
    }
    else cout << "Unable to open file '" << file_path << "'";
    
    return read_size;
}

// Puts a buffer to a file, based upon file-path
void put_file_contents(const char * file_path, unsigned char * contents, int length)
{
    //cout << "\nput_file_contents() called\n\n";
    
    char * memblock;
    
    ofstream file (file_path, ios::out|ios::binary);
    if (file.is_open())
    {
        memblock = (char *)contents;
        file.write(memblock, length);
        file.close();
    }
    else cout << "Unable to open file '" << file_path << "'";
}
