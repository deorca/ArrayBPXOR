//
//  isecretsharer.h
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 03/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#ifndef SmcsClient_ArrayBpXorEncoding_isecretsharer_h
#define SmcsClient_ArrayBpXorEncoding_isecretsharer_h

#include <map>
#include <vector>

using namespace std;

class ISecretSharer
{
    
public:
    
	virtual unsigned char * Key() = 0;
    
	virtual map<int, vector<unsigned char> > CreateShares(unsigned char * secret, int length, int max_number_of_shares) = 0;
	virtual unsigned char * ReconstructSecret(unsigned char ** shares, int num_shares, int share_size, int expected_file_size_in_bytes) = 0;
};


#endif
