#!/usr/bin/python

from restrictedbnmatrix import RestrictedBNMatrix
from random import seed
from random import randint
from time import sleep
from operator import eq

def test_class(share1=1, share2=2, secret_size=20):
    """Test function which tests sharing and then reconstruction with one good
    share and one bad share"""

    print ""
    print ""
    b = 15
    n = 31

    matrix = RestrictedBNMatrix(b, n)
    matrix.initialise()

    sleep(0.45)
    bad_matrix = RestrictedBNMatrix(b, n)
    bad_matrix.initialise()

    sleep(0.73)
    seed()
    print ""
    secret = [randint(0,255) for x in range(1, b+1)]
    print "Secret to be shared:", secret[:4]

    shares = matrix.create_shares(secret)

    share_indices = range(1, matrix.get_required_columns() + 1)
    share_indices.remove(share1)
    share_indices.remove(share2)
    bad_share = share_indices[0]

    bad_matrix_shares = {}
    bad_matrix_shares[share1] = shares[share1]
    seed()
    bad_matrix_shares[bad_share] = [randint(0,255) for x in range(1, b+1)]

    print ""
    print ""
    bad_recon_secret = bad_matrix.reconstruct_secret(bad_matrix_shares)
    print "Reconstructed secret (from good share", str(share1), \
        ", & a bad share):", bad_recon_secret[:4]
    print "Reconstructed bad secret matches original:", \
        all(map(eq, secret, bad_recon_secret))

    good_matrix_shares = {}
    good_matrix_shares[share1] = shares[share1]
    good_matrix_shares[share2] = shares[share2]

    recon_secret = matrix.reconstruct_secret(good_matrix_shares)    
    print ""
    print "Reconstructed secret (from shares", str(share1), " & ", \
        str(share2), "):", recon_secret[:4]
    print "Reconstructed good secret matches original:", \
        all(map(eq, secret, recon_secret))

    del matrix
    del bad_matrix


if __name__ == "__main__":
    import sys

    secret_size = 20

    test_class(1, 2, secret_size)
    test_class(1, 3, secret_size)
    test_class(1, 4, secret_size)
    test_class(2, 3, secret_size)
    test_class(2, 4, secret_size)
    test_class(3, 4, secret_size)
