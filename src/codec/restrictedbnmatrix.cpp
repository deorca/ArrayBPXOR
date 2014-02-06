/* This file is part of SmcsClient.ArrayBpXorEncoding library
 *
 *  SmcsClient.ArrayBpXorEncoding is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SmcsClient.ArrayBpXorEncoding is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the SmcsClient.ArrayBpXorEncoding library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "restrictedbnmatrix.hh"
#include <iostream>


RestrictedBNMatrix::RestrictedBNMatrix() : _b(DEFAULT_B), _n(DEFAULT_N)
{
    cout << "RestrictedBNMatrix::RestrictedBNMatrix() called.\n";
    
    _key = new unsigned char[_b]();
    _full_input_values = new unsigned char[2 * _b]();
    
    InitialiseTemplate();
}

RestrictedBNMatrix::RestrictedBNMatrix(int b, int n) : _b(b), _n(n)
{
    cout << "RestrictedBNMatrix::RestrictedBNMatrix(int b, int n) called.\n";
    
    _key = new unsigned char[_b]();
    _full_input_values = new unsigned char[2 * _b]();
    
    InitialiseTemplate();
}
RestrictedBNMatrix::~RestrictedBNMatrix()
{
    cout << "RestrictedBNMatrix::~RestrictedBNMatrix() called.\n";

    delete _key;
    delete _full_input_values;
    delete _template_matrix;
}

// ICodeMatrix
int RestrictedBNMatrix::Rows()
{
    return _b;
}
int RestrictedBNMatrix::Columns()
{
    return _n;
}
int RestrictedBNMatrix::ColumnsRequired()
{
    return REQUIRED_COLUMNS;
}

unsigned char * RestrictedBNMatrix::Key()
{
    return _key;
}
void RestrictedBNMatrix::Initialise(unsigned char key[])
{
    for (int i = 0; i < _b; i++)
    {
        _key[i] = key[i];
    }
    
    // Add the key to the input-values
    // Wang (2013) denote this as Vi´
    int upper_limit = _n - 1; // We make the upper limit of V here as n-1 since we never use Vn
    for (int i = _b; i < upper_limit; i++)
    {
        _full_input_values[i] = _key[i - _b];
    }
}

unsigned char ** RestrictedBNMatrix::CreateShares(unsigned char * secret)
{
    // Create hashed part of the secret to complete the input-values
    // Wang (2013) denote this as Vi
    for (int i = 0; i < _b; i++)
    {
        _full_input_values[i] = secret[i] ^ _key[i];
    }
    
    unsigned char ** return_val = new (nothrow) unsigned char * [REQUIRED_COLUMNS];
    // Assign a value to the first element even though it will never be used
    // Arrays are always zero-based, but we use one-base here to preserve the
    // -protocol
    return_val[0] = new (nothrow) unsigned char[_b];

    for (int j = 1; j <= REQUIRED_COLUMNS; j++)
    {
        return_val[j] = new (nothrow) unsigned char[_b + 1];
        
        // Create each element-pair in turn
        for (int i = 1; i <= _b; i++)
        {
            // Get next values for left- & right-operands from template.
            // These values should have one subtracted from them as they are 1-based,
            // - whereas our arrays are zero-based
            int left_operand = _template_matrix[j][i][0] - 1;
            int right_operand = _template_matrix[j][i][1] - 1;
            
            // Eliminate operands where Vn has been identified
            if (right_operand <= VN_SURROGATE)
            {
                // Only one operand should be used
                return_val[j-1][i-1] = _full_input_values[left_operand];
                continue;
            }
            // Both operands should be used
            return_val[j-1][i-1] = _full_input_values[left_operand] ^ _full_input_values[right_operand];
        }
    }
    
    // Delete original data, but not the latter half as these represent the key
    for (int i = 0; i < _b; i++)
    {
        _full_input_values[i] = 0;
    }
    
    return return_val;
}
unsigned char * RestrictedBNMatrix::ReconstructSecret(unsigned char ** shares, int indices[], int number_of_indices)
{
    unsigned char *reconstructed_secret = new (nothrow) unsigned char [_b];
    
    int one_index = ArrayContainsValue(indices, number_of_indices, 1);
    int two_index = ArrayContainsValue(indices, number_of_indices, 2);
    int three_index = ArrayContainsValue(indices, number_of_indices, 3);
    int four_index = ArrayContainsValue(indices, number_of_indices, 4);
    
    if (one_index >= 0) // Key = 1 exists
    {
        ReconstructJ1(shares[indices[one_index]]);
        if (two_index >= 0)
        {
            ReconstructWithJ2ForJ1(shares[two_index]);
        }
        else if (three_index >= 0)
        {
            ReconstructWithJ3ForJ1(shares[three_index]);
        }
        else if (four_index >= 0)
        {
            ReconstructWithJ4ForJ1(shares[four_index]);
        }
    }
    else if (two_index >= 0)
    {
        ReconstructJ2FirstRound(shares[two_index]);
        if (three_index >= 0)
        {
            ReconstructWithJ3ForJ2(shares[three_index]);
            ReconstructJ2SecondRound(shares[two_index]);
        }
        else if (four_index >= 0)
        {
            ReconstructWithJ4ForJ2(shares[four_index]);
        }
    }
    else if (three_index >= 0)
    {
        ReconstructJ3FirstRound(shares[three_index]);
        ReconstructWithJ4ForJ3FirstRound(shares[four_index]);
        ReconstructJ3SecondRound(shares[three_index]);
        ReconstructWithJ4ForJ3SecondRound(shares[four_index]);
        ReconstructJ3ThirdRound(shares[three_index]);
    }
    
    // Reconstruct secret from reconstructed input-values
    for (int i = 0; i < _b; i++)
    {
        reconstructed_secret[i] = _full_input_values[i] ^ _full_input_values[i + _b];
    }
    
    return reconstructed_secret;
}

// Helpers
void RestrictedBNMatrix::InitialiseTemplate()
{
    // The first dimension of the template signifys the number of rows, the second the number of columns,
    // the third should always be = 2 and specifies a value-pair
    // Build each column in turn
    
    int *** template_matrix = new (nothrow) int ** [REQUIRED_COLUMNS + 1]();
    
    if (template_matrix == NULL) return;
    
    int ** firstrow = new (nothrow) int * [_b + 1]();
    template_matrix[0] = firstrow;
    
    // Build each column in turn
    for (int j = 1; j <= REQUIRED_COLUMNS; j++)
    {
        int ** row = new (nothrow) int * [_b + 1]();
        
        if (row == NULL)
        {
            delete[] template_matrix;
            return;
        }
        
        // Create each element-pair in turn
        for (int i = 1; i <= _b; i++)
        {
            // The formula for creating the left-hand element of the pair is: (i + j) mod n
            // Where the element would equal 0 (indicating Vn as we are doing mod n),
            // however we, put in a value of -1 to indicate that this half of the pair is not to be used
            int left = (i + j) % _n;
            left = (left == 0) ? VN_SURROGATE : left;
            
            // The formula for creating the right-hand element of the pair is: (n - i + j) mod n
            // Where the element would equal 0 (indicating Vn as we are doing mod n),
            // however, we put in a value of -1 to indicate that this half of the pair is not to be used
            int right = (_n - i + j) % _n;
            right = (right == 0) ? VN_SURROGATE : right;
            
            // If the left operand is to be ignored, swap with the right operand
            // - This means that we only have to search one operand when finding an anchor-value
            if (left == VN_SURROGATE)
            {
                left = right;
                right = VN_SURROGATE;
            }
            
            int * left_and_right = new (nothrow) int [2]();
            
            if (left_and_right == NULL)
            {
                delete[] row;
                delete[] template_matrix;
                return;
            }
            
            left_and_right[0] = left;
            left_and_right[1] = right;

            row[i] = left_and_right;
        }
        template_matrix[j] = row;
    }
    
    _template_matrix = template_matrix;
}
int RestrictedBNMatrix::ArrayContainsValue(int indices[], int number_of_indices, int value)
{
    int local_value = value - ONE_BASE_OFFSET;
    
    for (int i = 0; i < number_of_indices; i++)
    {
        if (indices[i] == local_value)
        {
            return i;
        }
    }
    return -1;
}


// Reconstructors

// + BEGIN J1
void RestrictedBNMatrix::ReconstructJ1(unsigned char * column1)
{
    // Reminder:    Vi  = Vi´ ^ Si
    //              Vi´ = V(i+b)
    
    // Can regain V2 immediately from i1j1 (or i0j1)
    int i = 1 - ONE_BASE_OFFSET;
    int v = 2 - ONE_BASE_OFFSET;
    int k = _n - 1 - ONE_BASE_OFFSET;
    
    // Regaining V2:    setting V, index v=2 (1 in zero-base)
    //                  referencing column-index i=1 (0 in zero-base)
    _full_input_values[v] = column1[i];
    v++;
    i++;
    
    while (i < _b)
    {
        // Ragaining V3:    setting v, index v=3 (2 in zero-base)
        //                  referencing column-index i=2 (1 in zero-base)
        //                  referencing key-index a=6 (5 in zero-base)
        _full_input_values[v] = column1[i] ^ _full_input_values[k];
        
        k--;
        i++;
        v++;
    }
    
}
void RestrictedBNMatrix::ReconstructWithJ2ForJ1(unsigned char * column2)
{
    int v3_index = 2;
    int v1_index = 0;
    int v3_xor_v1_index = 0;
    
    _full_input_values[v1_index] = column2[v3_xor_v1_index] ^ _full_input_values[v3_index];
}
void RestrictedBNMatrix::ReconstructWithJ3ForJ1(unsigned char * column3)
{
    // Reconstruct V1 from j3: i1j3 = V5 ^ V1, so V1 = i2j3 ^ V5
    int v5_index = 4;
    int v5_xor_v1_index = 1;
    int v1_index = 0;
    
    _full_input_values[v1_index] = column3[v5_xor_v1_index] ^ _full_input_values[v5_index];
}
void RestrictedBNMatrix::ReconstructWithJ4ForJ1(unsigned char * column4)
{
    // Reconstruct V1 from j4: i3j4 = V7 ^ V1, so V1 = i3j4 ^ V7
    int k = 7 - ONE_BASE_OFFSET;
    int i = 3 - ONE_BASE_OFFSET;
    int v = 1 - ONE_BASE_OFFSET;
    
    if (_b * 2 + 1 > 7)
    {
        _full_input_values[v] = column4[i] ^ _full_input_values[k];
    }
    else
    {
        // Special case for 3x7 matrix
        _full_input_values[v] = column4[i];
    }
    
}
// - END J1

// + BEGIN J2
void RestrictedBNMatrix::ReconstructJ2FirstRound(unsigned char * column2)
{
    // Reminder:    Vi  = Vi´ ^ Si
    //              Vi´ = V(i+b)
    
    // Can regain V4 immediately from i2j2 (or i1j2 with zero-base on i)
    int i = 2 - ONE_BASE_OFFSET;
    int v = 4 - ONE_BASE_OFFSET;
    int k = _n - 1 - ONE_BASE_OFFSET;
    
    // Regaining V4:    setting V, index v=4 (3 in zero-base)
    //                  reference column-index i=1 (0 in zero-base)
    _full_input_values[v] = column2[i];
    v++;
    i++;
    
    while (i < _b)
    {
        // Regaining Vi+2:  setting v, index v=3 (2 in zero-base)
        //                  referencing column-index i=2 (1 in zero-base)
        //                  referencing key-index a=6 (5 in zero-base)
        _full_input_values[v] = column2[i] ^ _full_input_values[k];
        
        k--;
        i++;
        v++;
    }
}
void RestrictedBNMatrix::ReconstructWithJ3ForJ2(unsigned char * column3)
{
    // Get V2 from j3:  i2j3 = V4^V2, V2 = i1j3 ^ V4
    int v = 2 - ONE_BASE_OFFSET;
    int i = 1 - ONE_BASE_OFFSET;
    int k = 4 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column3[i] ^ _full_input_values[k];
    
    // Get V1 from j3: i2j3 = V5^V1, V1 = i2j3 ^ V5
    v = 1 - ONE_BASE_OFFSET;
    i = 2 - ONE_BASE_OFFSET;
    k = 5 - ONE_BASE_OFFSET;

    _full_input_values[v] = column3[i] ^ _full_input_values[k];
}
void RestrictedBNMatrix::ReconstructJ2SecondRound(unsigned char * column2)
{
    // Get V3 from j2: i1j2 = V3^V1, V3 = i1j3 ^ V1
    int v = 3 - ONE_BASE_OFFSET;
    int i = 1 - ONE_BASE_OFFSET;
    int k = 1 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column2[i] ^ _full_input_values[k];
}
void RestrictedBNMatrix::ReconstructWithJ4ForJ2(unsigned char * column4)
{
    // Get V3 from j4:  i1j4 = V5^V3, V3 = i1j4 ^ V5
    int v = 3 - ONE_BASE_OFFSET;
    int i = 1 - ONE_BASE_OFFSET;
    int k = 5 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
    
    // Get V2 from j4:  i2j4 = V6^V2, V2 = i2j4 ^ V6
    v = 2 - ONE_BASE_OFFSET;
    i = 2 - ONE_BASE_OFFSET;
    k = 6 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
    
    // Get V1 from j4:  i3j4 = V7^V1, V1 = i3j4 ^ V7
    v = 1 - ONE_BASE_OFFSET;
    i = 3 - ONE_BASE_OFFSET;
    k = 7 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
}
// - END J2

// + BEGIN J3
void RestrictedBNMatrix::ReconstructJ3FirstRound(unsigned char * column3)
{
    // Reminder:    Vi  = Vi´ ^ Si
    //              Vi´ = V(i+b)
    
    // Can regain V6 immediately from i3j3 (or i2j3 with zero-base on i)
    int i = 3 - ONE_BASE_OFFSET;
    int v = 6 - ONE_BASE_OFFSET;
    int k = _n - 1 - ONE_BASE_OFFSET;
    
    // Regaining V7:    setting V, index v=7 (1 in zero-base)
    //                  reference column-index i=1 (0 in zero-base)
    _full_input_values[v] = column3[i];
    v++;
    i++;
    
    while (i < _b)
    {
        // Regaining V3:    setting v, index v=3 (2 in zero-base)
        //                  referencing column-index i=2 (1 in zero-base)
        //                  referencing key-index a=6 (5 in zero-base)
        _full_input_values[v] = column3[i] ^ _full_input_values[k];
        
        k--;
        i++;
        v++;
    }
}
void RestrictedBNMatrix::ReconstructWithJ4ForJ3FirstRound(unsigned char * column4)
{
    // Get V1 from j4: i3j4 = V7^V1, V1 = i3j4 ^ V7
    int v = 1 - ONE_BASE_OFFSET;
    int i = 3 - ONE_BASE_OFFSET;
    int k = 7 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
}
void RestrictedBNMatrix::ReconstructJ3SecondRound(unsigned char * column3)
{
    // Get V5 from j3: i2j3 = V5^V1, V5 = i2j3 ^ V1
    int v = 5 - ONE_BASE_OFFSET;
    int i = 2 - ONE_BASE_OFFSET;
    int k = 1 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column3[i] ^ _full_input_values[k];
}
void RestrictedBNMatrix::ReconstructWithJ4ForJ3SecondRound(unsigned char * column4)
{
    // Get V3 from j4: i1j4 = V5^V3, V3 = i3j4 ^ V5
    int v = 3 - ONE_BASE_OFFSET;
    int i = 1 - ONE_BASE_OFFSET;
    int k = 5 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
    
    // Get V2 from j4: i2j4 = V6^V2, V2 = i2j4 ^ V6
    v = 2 - ONE_BASE_OFFSET;
    i = 2 - ONE_BASE_OFFSET;
    k = 6 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column4[i] ^ _full_input_values[k];
}
void RestrictedBNMatrix::ReconstructJ3ThirdRound(unsigned char * column3)
{
    // Get V4 from j3: i1j3 = V4^V2, V4 = i1j3 ^ V2
    int v = 4 - ONE_BASE_OFFSET;
    int i = 1 - ONE_BASE_OFFSET;
    int k = 2 - ONE_BASE_OFFSET;
    
    _full_input_values[v] = column3[i] ^ _full_input_values[k];
}
// - END J3
