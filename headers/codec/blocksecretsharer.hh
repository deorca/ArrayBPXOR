//
//  blocksecretsharer.h
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 03/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#ifndef SmcsClient_ArrayBpXorEncoding_blocksecretsharer_h
#define SmcsClient_ArrayBpXorEncoding_blocksecretsharer_h

//#define TEST_BLOCK_SECRET_SHARER

#include "isecretsharer.hh"
#include "icodematrix.hh"

class BlockSecretSharer : public ISecretSharer
{
private:
#ifndef TEST_BLOCK_SECRET_SHARER
    static const int B = 251;
    static const int N = 503;
#else
    static const int B = 4;
    static const int N = 9;
#endif
    static const int R = 3;
    
    ICodeMatrix * _array_bp_xor_matrix;
    unsigned char _key[B];
    
    void Initialise(unsigned char * key);
    
public:
    explicit BlockSecretSharer();
    BlockSecretSharer(unsigned char key[]);
    ~BlockSecretSharer();
    
    unsigned char * Key();
    static const int KeyLength();
    
    map<int, vector<unsigned char> > CreateShares(unsigned char * secret, int length, int max_number_of_shares);
    unsigned char * ReconstructSecret(map<int, vector<unsigned char> > shares, int share_size, int expected_file_size_in_bytes);
};


#endif
