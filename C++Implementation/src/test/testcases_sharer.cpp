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

//int main(int argc, char *argv[])
int main(void)
{
    int unsigned_char_max = 255;
#ifdef TEST_BLOCK_SECRET_SHARER
    int secret_length = 9;
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
    
    int share_size = shares[1].size();

    //unsigned char ** shares_array = new unsigned char * [4];
    
    for (int j = 1; j <= 4; j++)
    {
        //shares_array[j-1] = new unsigned char[share_size];
        
	write_elipsis = false;
        cout << "\n" << "Share j" << j << ":     ";
        for (int i = 0; i < share_size; i++)
        {
	    //shares_array[j-1][i] = shares[j].at(i);
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
    
    cout << "\n\n" << "****Test-case 1: all indices \n";

    unsigned char * reconstructed_secret1 =
        secret_sharer->ReconstructSecret(shares, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret1[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret1;
    cout << "\n\n" << "****End Test-case 1\n";
    
    
    cout << "\n\n" << "****Test-case 2: indices 1 and 3 \n";

    map<int, vector<unsigned char> > shares13;
    
    shares13[1] = shares[1];
    shares13[3] = shares[3];
    
    unsigned char * reconstructed_secret2 =
        secret_sharer->ReconstructSecret(shares13, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret2[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret2;
    cout << "\n\n" << "****End Test-case 2\n";
    

    cout << "\n\n" << "****Test-case 3: indices 1 and 4 \n";

    map<int, vector<unsigned char> > shares14;
    
    shares14[1] = shares[1];
    shares14[4] = shares[4];
    
    unsigned char * reconstructed_secret3=
        secret_sharer->ReconstructSecret(shares14, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret3[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret3;
    cout << "\n\n" << "****End Test-case 3\n";



    cout << "\n\n" << "****Test-case 4: indices 2 and 3 \n";

    map<int, vector<unsigned char> > shares23;
    
    shares23[2] = shares[2];
    shares23[3] = shares[3];
    
    unsigned char * reconstructed_secret4 =
        secret_sharer->ReconstructSecret(shares23, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret4[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret4;
    cout << "\n\n" << "****End Test-case 4\n";


    cout << "\n\n" << "****Test-case 5: indices 2 and 4 \n";

    map<int, vector<unsigned char> > shares24;
    
    shares24[2] = shares[2];
    shares24[4] = shares[4];
    
    unsigned char * reconstructed_secret5=
        secret_sharer->ReconstructSecret(shares24, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret5[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret5;
    cout << "\n\n" << "****End Test-case 5\n";

    
    cout << "\n\n" << "****Test-case 6: indices 3 and 4 \n";

    map<int, vector<unsigned char> > shares34;
    
    shares34[3] = shares[3];
    shares34[4] = shares[4];
    
    unsigned char * reconstructed_secret6=
        secret_sharer->ReconstructSecret(shares34, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret6[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret6;
    cout << "\n\n" << "****End Test-case 6\n";
    

    cout << "\n\n" << "****Test-case 7: new secret-sharer, new key, indices 1 and 2 \n";

    BlockSecretSharer *secret_sharer2 = new BlockSecretSharer();

    unsigned char * reconstructed_secret7=
        secret_sharer2->ReconstructSecret(shares, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret7[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret7;
    delete secret_sharer2;
    
    cout << "\n\n" << "****End Test-case 7\n";


    cout << "\n\n" << "****Test-case 8: new secret-sharer, same key, indices 1 and 2 \n";

    BlockSecretSharer *secret_sharer3 = new BlockSecretSharer(key);

    unsigned char * reconstructed_secret8=
        secret_sharer3->ReconstructSecret(shares, share_size, secret_length);
    write_elipsis = false;
    cout << "\nOutput:       ";
    for (int i = 0; i < secret_length; i++)
    {
	if (i < 6 || i > secret_length - 6)
	{
	    cout << "'" << int(reconstructed_secret8[i]) << "' ";
	}
	else if (!write_elipsis)
	{
	    cout << " ... ";
	    write_elipsis = true;
	}
    }
    delete[] reconstructed_secret8;
    delete secret_sharer3;
    
    cout << "\n\n" << "****End Test-case 8\n";



    cout << "\n\n- End.\n\n";
    
    delete secret_sharer;
    
    return (0);
}
