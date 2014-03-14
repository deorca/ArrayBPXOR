#!/usr/bin/python2
#
# test_blocksecretsharer.py
#
# Copyright 2014 James Sinclair
# This file is part of Array-BPXOR Secret-sharer.
#
#    Array-BPXOR Secret-sharer is free software: you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Array-BPXOR Secret-sharer is distributed in the hope that it will be
#    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Array-BPXOR Secret-sharer.  If not, 
#    see <http://www.gnu.org/licenses/>.

from blocksecretsharer import BlockSecretSharer
from random import seed
from random import randint
from operator import eq

#import pdb

def test_class(share1=1, share2=2, secret_size=20):

    sharer = BlockSecretSharer()
    print ""
    print ""
    print "Sharer initialised with key:", sharer.getKey()[:10], "..."

    seed()
    secret = [randint(0,255) for x in range(0,secret_size)]
    print "Secret to be shared:", secret[:10], "..."

    shares = sharer.create_shares(secret)

    shares12 = { share1 : shares[share1], share2 : shares[share2] }
    reconstructed_secret12 = sharer.reconstruct_secret(shares12, len(secret))

    print ""
    print "Reconstructed secret from shares ", share1, " & ", share2, ":", reconstructed_secret12[:10], "..."
    print "Reconstructed secret matches original:", all(map(eq, secret, reconstructed_secret12))
    print ""

if __name__ == "__main__":
    import sys

    secret_size = 20

    test_class(1, 2, secret_size)
    test_class(1, 3, secret_size)
    test_class(1, 4, secret_size)
    test_class(2, 3, secret_size)
    test_class(2, 4, secret_size)
    test_class(3, 4, secret_size)

