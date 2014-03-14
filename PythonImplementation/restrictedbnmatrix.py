#!/usr/bin/python
"""Defines class RestrictedBNMatrix"""

from random import seed
from random import randint


class RestrictedBNMatrix(object):
    """Defines an implementation of a BN matrix used in sharing and
    reconstructing by the array BPXOR codec """

    #Public interface
    def __init__(self, b, n):
        self.__b = b
        self.__n = n
        self.__initialise_template()

    def __del__(self):
        pass

    def initialise(self, key=None):
        """Initialises the object with either a supplied key or, if none is
        supplied, it generates one"""

        if key is None:
            seed()
            self.__key = [randint(0, 255) for x in range(self.__b)]
        else:
            self.__key[:] = key[:]
        self.__full_input_values = [0 * x for x in range(self.__n)]
        self.__full_input_values[self.__b:] = self.__key[:]
        #print "Initialised key to", self.__key
        #print "Initialised input-vector to", self.__full_input_values

    def get_key(self):
        """Returns the locally held key"""
        return self.__key

    def get_required_columns(self):
        """Returns the expected number of columns to output"""
        return self.__required_columns

    def create_shares(self, secret):
        """Creates a number of shares based on an input secret."""

        #Create hashed part of the secret to complete the input-values
        #Wang (2013) denotes this as Vi
        #pdb.set_trace()
        secret_length = len(secret)
        if secret_length < self.__b:
            secret.extend([0 * x for x in range(self.__b - secret_length)])
        self.__full_input_values[:self.__b] = [
            secret[i] ^ self.__key[i] for i in range(self.__b)]

        # Create dictionary for shares to be returned
        return_val = {}
        for j in range(1, self.__required_columns + 1):

            share = []

            # Create each element-pair in turn
            for i in range(1, self.__b + 1):

                # Get next values for left- & right-operands from template.
                # These values should have one subtracted from them as they are
                # - 1-based whereas our lists are zero-based
                left_operand = self.__template_matrix[j][i][0] - 1
                right_operand = self.__template_matrix[j][i][1] - 1

                # Eliminate operands where Vn has been identified
                if right_operand <= self.__vn_surrogate:
                    share.append(self.__full_input_values[left_operand])
                    continue

                # Both operands should be used
                share.append(self.__full_input_values[left_operand] ^
                             self.__full_input_values[right_operand])

            # Add the share to the return value
            return_val[j] = share

        # Delete original data, but not the latter half as these represent
        # the key
        self.__full_input_values[:self.__b] = [0 for x in range(self.__b)]

        return return_val

    def reconstruct_secret(self, shares):
        """Reconstructs a given dictionary of shares."""

        reconstructed_secret = []
        share_length = 0

        if 1 in shares:
            share_length = len(shares[1])
            self.__reconstruct_j1(shares[1])
            if 2 in shares:
                self.__reconstruct_withj2_forj1(shares[2])
            elif 3 in shares:
                self.__reconstruct_withj3_forj1(shares[3])
            elif 4 in shares:
                self.__reconstruct_withj4_forj1(shares[4])
        elif 2 in shares:
            share_length = len(shares[2])
            self.__reconstruct_j2_first_round(shares[2])
            if 3 in shares:
                self.__reconstruct_withj3_forj2(shares[3])
                self.__reconstruct_j2_second_round(shares[2])
            elif 4 in shares:
                self.__reconstruct_withj4_forj2(shares[4])
        elif 3 in shares and 4 in shares:
            share_length = len(shares[3])
            self.__reconstruct_j3_first_round(shares[3])
            self.__reconstruct_withj4_forj3_1(shares[4])
            self.__reconstruct_j3_second_round(shares[3])
            self.__reconstruct_withj4_forj3_2(shares[4])
            self.__reconstruct_j3_third_round(shares[3])
        else:
            print "None of the required shares are present.", \
                "No reconstruction possible"

        reconstructed_secret = [
            self.__full_input_values[i] ^
            self.__full_input_values[i + self.__b]
            for i in range(0, share_length)]

        # Delete original data, but not the latter half as these represent
        # the key
        self.__full_input_values[:self.__b] = [0 * x for x in range(self.__b)]

        return reconstructed_secret

    #Private
    __one_base_offset = 1
    __required_columns = 4
    __vn_surrogate = -1
    __b = 251
    __n = 503
    __key = []
    __full_input_values = []
    __template_matrix = {}

    def __surrogate(self, value):
        """Checks an input value and, if zero, returns the surrogate value """
        if value == 0:
            return self.__vn_surrogate
        else:
            return value

    def __initialise_template(self):
        """Creates the matrix template"""

        #Build each column in turn
        for j in range(1, self.__required_columns + 1):

            column = [0]

            #Create each element-pair in turn
            for i in range(1, self.__b + 1):

                #The formula for creating the left-hand element of the pair is
                #    (i + j) mod n
                #Where the element would equal 0 (indicating Vn as we are doing
                #mod n), however, we put in a value of -1 to indicate that this
                #half of the pair is not to be used
                left = (i + j) % self.__n
                left = self.__surrogate(left)

                #The formula for creating the right-hand element of the pair is
                #    (n - i + j) mod n
                #Where the element would equal 0 (indicating Vn as we are
                #doing mod n), however, we put in a value of -1 to indicate
                #that this half of the pair is not to be used
                right = (self.__n - i + j) % self.__n
                right = self.__surrogate(right)

                #If the left operatnd is to be ignored, swap with the right
                #operand. This means that we only have to search one operand
                #when finding an anchor-value
                if  left == self.__vn_surrogate:
                    left = right
                    right = self.__vn_surrogate

                column.append([left, right])

            #Add new list (template_matrix[j][i]
            self.__template_matrix[j] = column

    def __reconstruct_j1(self, share):
        """Does first phase of reconstruction where the first share is
        present."""

        share_size = len(share)

        # Reminder:    Vi  = Vi' ^ Si
        #              Vi' = V(i+b)

        # Can regain V2 immediately from i1j1
        i_index = 1 - self.__one_base_offset
        v_index = 2 - self.__one_base_offset
        k_index = self.__n - 1 - self.__one_base_offset

        # Regaining V2:    setting V, index v=2
        #                  referencing column-index i=1
        self.__full_input_values[v_index] = share[i_index]
        v_index += 1
        i_index += 1

        while i_index < share_size:
            # Regaining V3:    setting v, index v=3
            #                  referencing column-index i=2
            #                  referencing key-index a=6
            self.__full_input_values[v_index] = share[i_index] ^ \
                self.__full_input_values[k_index]

            k_index -= 1
            i_index += 1
            v_index += 1

    def __reconstruct_withj2_forj1(self, share):
        """Does second phase of reconstruction where the first and second
        shares are present."""

        v3_index = 2
        v1_index = 0
        v3_xor_v1_index = 0

        self.__full_input_values[v1_index] = share[v3_xor_v1_index] ^ \
            self.__full_input_values[v3_index]

    def __reconstruct_withj3_forj1(self, share):
        """Does second phase of reconstruction where the first and third shares
        are present."""

        # Reconstruct V1 from j3: i1j3 = V5 ^ V1, so V1 = i1j3 ^ 5
        v5_index = 4
        v5_xor_v1_index = 1
        v1_index = 0

        self.__full_input_values[v1_index] = share[v5_xor_v1_index] ^ \
            self.__full_input_values[v5_index]

    def __reconstruct_withj4_forj1(self, share):
        """Does second phase of reconstruction where the first and fourth
        shares are present."""

        # Reconstruct V1 from j4; i3j4 = V7 ^ V1, so V1 = i3j4 ^ V7
        k_index = 7 - self.__one_base_offset
        i_index = 3 - self.__one_base_offset
        v_index = 1 - self.__one_base_offset

        if self.__b * 2 + 1 > 7:
            self.__full_input_values[v_index] = share[i_index] ^ \
                self.__full_input_values[k_index]
        else:
            self.__full_input_values[v_index] = share[i_index]

    def __reconstruct_j2_first_round(self, share):
        """Does first phase of reconstruction where the second share is
        present."""

        share_size = len(share)

        # Can regain V4 immediately from i2j2 (or i1j2 with zero-base on i)
        i_index = 2 - self.__one_base_offset
        v_index = 4 - self.__one_base_offset
        k_index = self.__n - 1 - self.__one_base_offset

        # Regaining V4;    setting V, index v=4 (3 in zero-base)
        #                  regerence column-index i=1 (0 in zero-base)

        self.__full_input_values[v_index] = share[i_index]
        v_index += 1
        i_index += 1

        while i_index < share_size:
            # Regaining Vi+2:    setting v, index v = 3 (2 in zero-base)
            #                    referencing column-index i=2 (1 in zero-base)
            #                    referencing key-index a=6 (5 in zero-base)
            self.__full_input_values[v_index] = share[i_index] ^ \
                self.__full_input_values[k_index]

            k_index -= 1
            i_index += 1
            v_index += 1

    def __reconstruct_withj3_forj2(self, share):
        """Does second phase of reconstruction where the second and third
        shares are present."""

        # Get V2 from j3:    i2j3 = V4^V2, V2 = i2j3 ^ V4
        v_index = 2 - self.__one_base_offset
        i_index = 1 - self.__one_base_offset
        k_index = 4 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

        # Get V1 from j3:    i2j3 = V5^V1, V1 = i2j3 ^ V5
        v_index = 1 - self.__one_base_offset
        i_index = 2 - self.__one_base_offset
        k_index = 5 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

    def __reconstruct_withj4_forj2(self, share):
        """Does second phase of reconstruction where the second and fourth
        shares are present."""

        # Get V3 from j4:    i1j4 = V5^V3, V3 = i1j4 ^ V5
        v_index = 3 - self.__one_base_offset
        i_index = 1 - self.__one_base_offset
        k_index = 5 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

        # Get V2 from j4:    i2j4 = V6^V2, V2 = i2j4 ^ V6
        v_index = 2 - self.__one_base_offset
        i_index = 2 - self.__one_base_offset
        k_index = 6 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

        # Get V1 from j4:    i3j4 = V7^V1, V1 = i3j4 ^ V7
        v_index = 1 - self.__one_base_offset
        i_index = 3 - self.__one_base_offset
        k_index = 7 - self.__one_base_offset

        if k_index < len(self.__full_input_values):
            self.__full_input_values[v_index] = share[i_index] ^ \
                self.__full_input_values[k_index]
        else:
            self.__full_input_values[v_index] = share[i_index]

    def __reconstruct_j2_second_round(self, share):
        """Does second round of reconstruction where the second and third
        shares are present."""

        # Get V3 from j2:    i1j2 = V3^V1, V3 = i1j3 ^ V1
        v_index = 3 - self.__one_base_offset
        i_index = 1 - self.__one_base_offset
        k_index = 1 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

    def __reconstruct_j3_first_round(self, share):
        """Does first phase of reconstruction where the third share is
        present."""

        # Reminder:    Vi  = Vi' ^ Si
        #              Vi' = V(i+b)

        # Can regain V6 immediately from i3j3 (or i2j3 with zero-base on i)
        v_index = 6 - self.__one_base_offset
        i_index = 3 - self.__one_base_offset
        k_index = self.__n - 1 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index]
        v_index += 1
        i_index += 1

        share_size = len(share)
        while i_index < share_size:
            # Regaining V3:    setting v, index v=3 (2 in zero-base)
            #                  referencing column-index i=2 (1 in zero-base)
            #                  referencing key-index a=6 (5 in zero-base)
            self.__full_input_values[v_index] = share[i_index] ^ \
                self.__full_input_values[k_index]

            k_index -= 1
            i_index += 1
            v_index += 1

    def __reconstruct_withj4_forj3_1(self, share):
        """Does second phase of reconstruction where the third and fourth
        shares are present."""

        # Get V1 from j4:  i3j4 = V7^V1, V1 = i3j4 ^ V7
        v_index = 1 - self.__one_base_offset
        i_index = 3 - self.__one_base_offset
        k_index = 7 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

    def __reconstruct_j3_second_round(self, share):
        """Does first phase of the second round of reconstruction where the
        second and third shares are present."""

        # Get V5 from j3:  i2j3 = V5^V1, V5 = i2j3 ^ V1
        v_index = 5 - self.__one_base_offset
        i_index = 2 - self.__one_base_offset
        k_index = 1 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

    def __reconstruct_withj4_forj3_2(self, share):
        """Does second phase of the second round of reconstruction where the
        third and fourth shares are present."""

        # Get V3 from j4:  i1j4 = V5^V3, V3 = i1j4 ^ V5
        v_index = 3 - self.__one_base_offset
        i_index = 1 - self.__one_base_offset
        k_index = 5 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

        # Get V2 from j4:  i2j4 = V6^V2, V2 = i2j4 ^ V6
        v_index = 2 - self.__one_base_offset
        i_index = 2 - self.__one_base_offset
        k_index = 6 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]

    def __reconstruct_j3_third_round(self, share):
        """Does third round of reconstruction where the third and fourth shares
        are present."""

        # Get V4 from j3:  i1j3 = V4^V2, V4 = i1j3 ^ V2
        v_index = 4 - self.__one_base_offset
        i_index = 1 - self.__one_base_offset
        k_index = 2 - self.__one_base_offset

        self.__full_input_values[v_index] = share[i_index] ^ \
            self.__full_input_values[k_index]
