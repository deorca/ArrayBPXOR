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

#ifndef ICODEMATRIX_H
#define ICODEMATRIX_H

#include <vector>
#include <map>
using namespace std;

class ICodeMatrix
{

public:

	virtual ~ICodeMatrix() {};
  
	virtual int Rows() = 0;
	virtual int Columns() = 0;
	virtual int ColumnsRequired() = 0;
	virtual unsigned char * Key() = 0;
	virtual void Initialise(unsigned char key[]) = 0;

	virtual unsigned char ** CreateShares(unsigned char * secret) = 0;
	virtual unsigned char * ReconstructSecret(unsigned char ** shares, int indices[], int number_of_indices, int share_size) = 0;
};

#endif // ICODEMATRIX_H
