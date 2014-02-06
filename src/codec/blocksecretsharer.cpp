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
using namespace std;

BlockSecretSharer::BlockSecretSharer()
{
    unsigned char key[B];

    srand ( time(NULL));
    for (int i = 0; i < B; i++)
    {
        key[i] = rand() % 255;
    }
    Initialise(key);
}

BlockSecretSharer::BlockSecretSharer(unsigned char * key)
{
    Initialise(key);
}
BlockSecretSharer::~BlockSecretSharer()
{
    //delete _array_bp_xor_matrix;
    //delete[] _key;
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
        }
        
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
    }
    
    delete[] array_shares;
    
    return shares;
}
unsigned char * BlockSecretSharer::ReconstructSecret(unsigned char ** shares, int num_shares, int share_size, int expected_file_size_in_bytes)
{
    if (num_shares < 2 || num_shares > N-1)
        throw "Number of shares from which to reconstruct secret should be at least 2";
    
    // Create & initialise a return-value
    unsigned char * reconstructed_secret = new unsigned char[expected_file_size_in_bytes];
    
    // Loop over shares, taking 'B' bytes at a tinme, reconstructing them, and adding each output to list
    unsigned char ** small_shares = new unsigned char * [2];
    
    int indices[] = { 0, 1 };
    
/*
    cout << "\n\nRecevied share 1:\n";
    for (int i = 0; i < share_size; i++)
    {
        cout << "'" << int(shares[0][i]) << "' ";
    }
    
    cout << "\nRecevied share 2:\n";
    for (int i = 0; i < share_size; i++)
    {
        cout << "'" << int(shares[1][i]) << "' ";
    }
    
    cout << "\n\n";
*/
    
    unsigned char * next_block_index_first = shares[0];
    unsigned char * next_block_index_second = shares[1];
    unsigned char * next_block_index_recon = reconstructed_secret;

    bool end_reached = false;
    int actual_size = 0;
    while (!end_reached)
    {
        // Copy blocks from first and second share
        small_shares[0] = next_block_index_first;
        small_shares[1] = next_block_index_second;
        
        // Reconstruct block from small-shares
        unsigned char * reconstructed_block = _array_bp_xor_matrix->ReconstructSecret(small_shares, indices, 2);
        // Add reconstructed block to reconstructed secret
        std::copy(reconstructed_block, reconstructed_block + B, next_block_index_recon);
        
        next_block_index_first += B;
        next_block_index_second += B;
        next_block_index_recon += B;
        
        actual_size += B;
        
        end_reached = (actual_size >= expected_file_size_in_bytes);
    }
    delete[] small_shares;

    return reconstructed_secret;
}
