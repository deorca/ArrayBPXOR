//
//  isecretsharer.h
//
//  Created by James Sinclair on 03/02/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//
//  This file is part of Array-BPXOR Secret-sharer.
//
//    Array-BPXOR Secret-sharer is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as published
//    by the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Array-BPXOR Secret-sharer is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Array-BPXOR Secret-sharer.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SmcsClient_ArrayBpXorEncoding_isecretsharer_h
#define SmcsClient_ArrayBpXorEncoding_isecretsharer_h

#include <map>
#include <vector>

using namespace std;

class ISecretSharer
{
    
public:
    
	virtual ~ISecretSharer() {};
	
	virtual unsigned char * Key() = 0;
    
	virtual map<int, vector<unsigned char> > CreateShares(unsigned char * secret, int length, int max_number_of_shares) = 0;
	virtual unsigned char * ReconstructSecret(map<int, vector<unsigned char> > shares, int share_size, int expected_file_size_in_bytes) = 0;
};


#endif
