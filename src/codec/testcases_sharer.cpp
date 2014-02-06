//
//  testcases_2.cpp
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 05/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

//#define TEST_BLOCK_SECRET_SHARER

#include <stdlib.h>
#include <iostream>
#include "blocksecretsharer.hh"
#include <map>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    int unsigned_char_max = 255;
#ifdef TEST_BLOCK_SECRET_SHARER
    int secret_length = 7;
#else
    int secret_length = 8192;
#endif
    int b = BlockSecretSharer::KeyLength();
    
    BlockSecretSharer *secret_sharer = new BlockSecretSharer();
    
    unsigned char file[secret_length];
    for (int i = 0; i < secret_length; i++)
        file[i] = (i + 1) % unsigned_char_max;
    map<int, vector<unsigned char> > shares = secret_sharer->CreateShares(file, secret_length, 4);
    
    cout << "\nStart -\n\n";
    
    unsigned char * key = secret_sharer->Key();
    cout << "Key:         ";
    bool write_elipsis = false;
    for (int i = 0; i < b; i++)
    {
        if (i < 6 || i > b - 6)
	{
	    cout << "'" << int(key[i]) << "' ";
        }
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    
    write_elipsis = false;
    cout << "\nInput:       ";
    for (int i = 0; i < secret_length; i++)
    {
        if (i < 6 || i > secret_length - 6)
        {
    	    cout << "'" << int(file[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    
    cout << "\n\n";
    
    unsigned char ** shares_array = new unsigned char * [4];
    int share_size = shares[1].size();
    
    for (int j = 1; j <= 4; j++)
    {
        shares_array[j-1] = new unsigned char[share_size];
        
	write_elipsis = false;
        cout << "\n" << "Share j" << j << ":     ";
        for (int i = 0; i < share_size; i++)
        {
	    shares_array[j-1][i] = shares[j].at(i);
	    if (i < 6 || i > share_size - 6)
	    {
		cout << "'" << int(shares[j].at(i)) << "' ";
	    }
	    else if (!write_elipsis)
	    {
	        cout << " ... ";
	        write_elipsis = true;
	    }
        }
    }
    
    cout << "\n\n" << "****Test-case 1: indices = j1 & j2 \n";
    unsigned char * reconstructed_secret =
        secret_sharer->ReconstructSecret(shares_array, 4, share_size, secret_length);
    
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    
    
    cout << "\n\n- End.\n\n";
    
    //delete shares;
    
    delete secret_sharer;
    
    return (0);
}
