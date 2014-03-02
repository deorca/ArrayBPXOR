//
//  blocksecretsharer.cpp
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 03/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#include "blocksecretsharer.hh"
#include "restrictedbnmatrix.hh"
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include <unistd.h>

using namespace std;

BlockSecretSharer::BlockSecretSharer()
{
    cout << "BlockSecretSharer::BlockSecretSharer() called.\n";
    unsigned char key[B];

    usleep(543123);
    srand ( time(NULL));
    for (int i = 0; i < B; i++)
    {
        key[i] = rand() % 255;
    }
    Initialise(key);
}

BlockSecretSharer::BlockSecretSharer(unsigned char * key)
{
    cout << "BlockSecretSharer::BlockSecretSharer(unsigned char *) called.\n";
    Initialise(key);
}
BlockSecretSharer::~BlockSecretSharer()
{
    cout << "BlockSecretSharer::~BlockSecretSharer() called.\n";
    delete _array_bp_xor_matrix;
}

void BlockSecretSharer::Initialise(unsigned char * key)
{
    for (int i = 0; i < B; i++)
    {
        _key[i] = key[i];
    }
    _array_bp_xor_matrix = new RestrictedBNMatrix(B, N);
    _array_bp_xor_matrix->Initialise(_key);
}

unsigned char * BlockSecretSharer::Key()
{
    return _key;
}
const int BlockSecretSharer::KeyLength()
{
    return B;
}

map<int, vector<unsigned char> > BlockSecretSharer::CreateShares(unsigned char * secret, int length, int max_number_of_shares)
{
    int total_shares;
    // Check length of input secret against max_number_of_shares
    if (max_number_of_shares < 2 || max_number_of_shares >= N)
        total_shares = N - 1;
    else
        total_shares = max_number_of_shares;
    
    // Create & initialise a return-value
    map<int, vector<unsigned char> > shares;
    unsigned char ** array_shares = new unsigned char * [total_shares];
    int output_length = (length % B > 0) ? (int(length / B) + 1) * B: length;
    for (int i = 0; i < total_shares; i++)
    {
        array_shares[i] = new unsigned char[output_length];
    }
    
    // Loop over secret, taking 3 bytes at a time, sharing them, and adding each output to map
    bool end_reached = false;
    int next_block_index = 0;
    unsigned char block_to_pass[B];
    while (!end_reached)
    {
        // Take next block
        unsigned char * block = secret + next_block_index;
        
        // Check for where the last block is shorter than the nominal block-length
        if (length - next_block_index < B)
        {
            // Take last bytes
            int block_length = length - next_block_index;
            for (int i = 0; i < B; i++)
                if (i >= block_length)
                    block_to_pass[i] = 0;
                else
                    block_to_pass[i] = block[i];
        }
        else
        {
            // Take next block
            for (int i = 0; i < B; i++)
                block_to_pass[i] = block[i];
        }
        
        // Share bytes
        unsigned char ** block_shares = _array_bp_xor_matrix->CreateShares(block_to_pass);
        
        // Add each output to share-array
        for (int i = 0; i < total_shares; i++)
        {
            copy(block_shares[i], block_shares[i] + B, array_shares[i] + next_block_index);
	    delete[] block_shares[i];
        }
	delete[] block_shares[total_shares];
        delete[] block_shares;
        
        // Set up index for next iteration
        next_block_index += B;
        if (next_block_index >= length)
            end_reached = true;
    }
    // Create map from share-array
    for (int i = 0; i < total_shares; i++)
    {
        shares[i+1] = vector<unsigned char>(array_shares[i], array_shares[i] + output_length);
	delete[] array_shares[i];
    }
    delete[] array_shares;
    
    return shares;
}
unsigned char * BlockSecretSharer::ReconstructSecret(map<int, vector<unsigned char> > shares, int share_size, int expected_file_size_in_bytes)
{
    if (shares.size() < 2 || shares.size() > N-1)
        throw "Number of shares from which to reconstruct secret should be at least 2";
    
    // Create & initialise a return-value
    unsigned char * reconstructed_secret = new unsigned char[expected_file_size_in_bytes];
    
    // Loop over shares, taking 'B' bytes at a tinme, reconstructing them, and adding each output to list
    unsigned char ** big_shares = new unsigned char * [2];
    unsigned char ** small_shares = new unsigned char * [2];
    
    int indices[2];
    int index = 0;
    
    for (int i = 1; i <= 4; i++)
    {
        map<int, vector<unsigned char> >::iterator it = shares.find(i);
	if (it != shares.end())
	{
	    indices[index] = i;
	    index++;
	    if (index == 2)
	        break;
	}
    }
    
    big_shares[0] = new unsigned char[shares[indices[0]].size()];
    big_shares[1] = new unsigned char[shares[indices[1]].size()];
    
    memcpy (big_shares[0], shares[indices[0]].data(), shares[indices[0]].size());
    memcpy (big_shares[1], shares[indices[1]].data(), shares[indices[1]].size());
    
    unsigned char * next_block_index_first = big_shares[0];
    unsigned char * next_block_index_second = big_shares[1];
    unsigned char * next_block_index_recon = reconstructed_secret;

    bool end_reached = false;
    int actual_size = 0;
    int block_slice = B;
    int share_slice = B;
    while (!end_reached)
    {
        // Copy blocks from first and second share
        small_shares[0] = next_block_index_first;
        small_shares[1] = next_block_index_second;
        
	// Get size of next small-share
	if (next_block_index_first + B > big_shares[0] + share_size)
	    share_slice = big_shares[0] + share_size - next_block_index_first;

	// Reconstruct block from small-shares
        unsigned char * reconstructed_block = _array_bp_xor_matrix->ReconstructSecret(small_shares, indices, 2, share_slice);
	
        // Add reconstructed block to reconstructed secret
	if (actual_size + B > expected_file_size_in_bytes)
	    block_slice = expected_file_size_in_bytes - actual_size;
	std::copy(reconstructed_block, reconstructed_block + block_slice, next_block_index_recon);
	
	delete[] reconstructed_block;
	
        next_block_index_first += B;
        next_block_index_second += B;
        next_block_index_recon += B;
        
        actual_size += B;
        
        end_reached = (actual_size >= expected_file_size_in_bytes);
    }
    
    small_shares[0] = NULL;
    small_shares[1] = NULL;
    delete[] small_shares;

    delete[] big_shares[0];
    delete[] big_shares[1];
    delete[] big_shares;

    return reconstructed_secret;
}
