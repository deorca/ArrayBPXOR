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

#ifndef RESTRICTEDBNMATRIX_H
#define RESTRICTEDBNMATRIX_H

#include "icodematrix.hh"
#include <vector>
#include <map>

using namespace std;

class RestrictedBNMatrix : public ICodeMatrix
{
private:
    static const int VN_SURROGATE = -1;
    static const int DEFAULT_B = 251;
    static const int DEFAULT_N = 503;
    static const int REQUIRED_COLUMNS = 4;
    static const int ONE_BASE_OFFSET = 1;
    
    const int _b, _n;

    unsigned char * _key;
    unsigned char * _full_input_values;

    int *** _template_matrix;
	
    // Helpers
    void ConstructionTasks();
	void InitialiseTemplate();
    int ArrayContainsValue(int indices[], int number_of_indices, int value);

	// Reconstructors
    void ReconstructJ1(unsigned char * column1);
    void ReconstructWithJ2ForJ1(unsigned char * column2);
    void ReconstructWithJ3ForJ1(unsigned char * column3);
    void ReconstructWithJ4ForJ1(unsigned char * column4);

    void ReconstructJ2FirstRound(unsigned char * column2);
    void ReconstructWithJ3ForJ2(unsigned char * column3);
    void ReconstructJ2SecondRound(unsigned char * column2);
    void ReconstructWithJ4ForJ2(unsigned char * column4);
    
    void ReconstructJ3FirstRound(unsigned char * column3);
    void ReconstructWithJ4ForJ3FirstRound(unsigned char * column4);
    void ReconstructJ3SecondRound(unsigned char * column3);
    void ReconstructWithJ4ForJ3SecondRound(unsigned char * column4);
    void ReconstructJ3ThirdRound(unsigned char * column3);

public:
	RestrictedBNMatrix();
    RestrictedBNMatrix(int b, int n);
	~RestrictedBNMatrix();

	// ICodeMatrix
	int Rows();
	int Columns();
	int ColumnsRequired();

    unsigned char * Key();
    void Initialise(unsigned char key[]);

    unsigned char ** CreateShares(unsigned char * secret);
    unsigned char * ReconstructSecret(unsigned char ** shares, int indices[], int number_of_indices);
};

#endif // RESTRICTEDBNMATRIX_H
