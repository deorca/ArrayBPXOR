//
//  testcases.cpp
//  SmcsClient.ArrayBpXorEncoding
//
//  Created by James Sinclair on 29/01/2014.
//  Copyright (c) 2014 James Sinclair. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include "restrictedbnmatrix.hh"
#include <map>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    int b = 4, n = 9;
    RestrictedBNMatrix *matrix2 = new RestrictedBNMatrix(b, n);
    unsigned char key[b];
    for (int i = 0; i < b; i++)
        key[i] = rand() % 230;
    matrix2->Initialise(key);

    int rows2 = matrix2->Rows();
    int columns2 = matrix2->Columns();
    
    unsigned char file[b];
    for (int i = 0; i < b; i++)
        file[i] = (10 * i) + (i + 1);
    unsigned char ** shares = matrix2->CreateShares(file);
    
    cout << "\nStart -\n\n";
    
    cout << "Key:         ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(key[i]) << "' ";
    }

    cout << "\nInput:       ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(file[i]) << "' ";
    }

    cout << "\n\n";
    
    for (int j = 0; j < 4; j++)
    {
        cout << "\n" << "Share j" << j+1 << ":     ";
        for (int i = 0; i < b; i++)
        {
            cout << "'" << int(shares[j][i]) << "' ";
        }
    }
    
    int indices[2];
    int one_base = 1;
    unsigned char ** recon_shares = new unsigned char * [2];

    cout << "\n\n" << "****Test-case 1: indices = j1 & j2 ";
    indices[0] = 1 - one_base;
    indices[1] = 2 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile1 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile1[i]) << "' ";
    }


    cout << "\n\n" << "****Test-case 2: indices = j1 & j3 ";
    indices[0] = 1 - one_base;
    indices[1] = 3 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile2 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile2[i]) << "' ";
    }


    cout << "\n\n" << "****Test-case 3: indices = j1 & j4 ";
    indices[0] = 1 - one_base;
    indices[1] = 4 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile3 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile3[i]) << "' ";
    }


    cout << "\n\n" << "****Test-case 4: indices = j2 & j3 ";
    indices[0] = 2 - one_base;
    indices[1] = 3 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile4 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile4[i]) << "' ";
    }


    cout << "\n\n" << "****Test-case 5: indices = j2 & j4 ";
    indices[0] = 2 - one_base;
    indices[1] = 4 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile5 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile5[i]) << "' ";
    }



    cout << "\n\n" << "****Test-case 6: indices = j3 & j4 ";
    indices[0] = 3 - one_base;
    indices[1] = 4 - one_base;

    recon_shares[0] = shares[indices[0]];
    recon_shares[1] = shares[indices[1]];
    
    // unsigned char ** shares, int indices[], int number_of_indices
    unsigned char * reconfile6 = matrix2->ReconstructSecret(recon_shares, indices, sizeof(indices)/sizeof(int));

    cout << "\n" << "Output:      ";
    for (int i = 0; i < b; i++)
    {
        cout << "'" << int(reconfile6[i]) << "' ";
    }


    cout << "\n\n- End.\n\n";

    delete[] shares;
    delete[] recon_shares;
    delete[] reconfile1;
    delete[] reconfile2;
    delete[] reconfile3;
    delete[] reconfile4;
    delete[] reconfile5;
    delete[] reconfile6;
    
    delete matrix2;
    
    return (0);
}
