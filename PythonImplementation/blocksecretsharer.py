#!/usr/bin/python
"""Defines BlockSecretSharer class"""

from restrictedbnmatrix import RestrictedBNMatrix
#import pdb


class BlockSecretSharer(object):
    """Extends the RestrictedBNMatrix class and applies it to data-blocks of
    any size"""

    #Public interface
    def __init__(self, key=None):
        self.__matrix = RestrictedBNMatrix(self.__b, self.__n)
        self.__matrix.initialise(key)

    def __del__(self):
        del self.__matrix

    def get_key(self):
        """Returns encryption-key"""
        return self.__matrix.get_key()

    def get_key_length(self):
        """Returns length of encryption-key"""
        return len(self.get_key())

    def get_number_of_shares(self):
        return self.__matrix.get_required_columns()

    def create_shares(self, secret):
        """Creates a number of shares based on an input secret."""

        length = len(secret)
        total_shares = self.__matrix.get_required_columns()
        shares = {}
        for i in range(1, total_shares+1):
            shares[i] = []

        # Loop over secret, taking B bytes at a time, sharing them, and adding
        # each output to map
        next_block = 0
        end_reached = (next_block >= length)
        while not end_reached:
            # Take next block

            # Check for where the last block is shorter than the nominal
            # block-length
            if length - next_block < self.__b:
                # Take last bytes
                block_to_pass = secret[next_block:length]
            else:
                # Take next block
                block_to_pass = secret[next_block:next_block + self.__b]

            # Share bytes
            block_shares = self.__matrix.create_shares(block_to_pass)

            for i in range(1, total_shares+1):
                shares[i][len(shares[i]):] = block_shares[i][:]

            next_block += self.__b
            end_reached = (next_block >= length)

        return shares

    def reconstruct_secret(self, shares, expected_file_size=0):
        """Reconstructs a given dictionary of shares."""

        number_of_shares = len(shares)
        if number_of_shares < 2 or number_of_shares > self.__n - 1:
            raise ValueError("Incorrect number of shares: between 2 and " +
                             self.__matrix.get_required_columns() +
                             " required")

        reconstructed_secret = []

        # Take from 2 shares only
        i = 0
        small_shares = {}
        first_share = second_share = 0
        for k in shares.iterkeys():
            small_shares[k] = []
            i += 1
            if i == 1:
                first_share = k
            if i == 2:
                second_share = k
            if i >= 2:
                break

        if expected_file_size == 0:
            expected_file_size = len(shares[first_share])

        next_block = 0
        end_reached = False
        while not end_reached:
            # Copy block from first- and second-shares
            small_shares[first_share][:] = \
                shares[first_share][next_block:next_block + self.__b]
            small_shares[second_share][:] = \
                shares[second_share][next_block:next_block + self.__b]

            # Reconstruct block from small-shares
            reconstructed_block = \
                self.__matrix.reconstruct_secret(small_shares)

            # Add reconstructed block to reconstructed secret
            reconstructed_secret[len(reconstructed_secret):] = \
                reconstructed_block[:]

            next_block += self.__b
            end_reached = next_block >= expected_file_size

        return reconstructed_secret[:expected_file_size]

    # Private interface
    __matrix = None
    __b = 251
    __n = 503
    __r = 3
    #__b = 15
    #__n = 31
    #__r = 3
