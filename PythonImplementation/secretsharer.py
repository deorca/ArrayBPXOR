#!/usr/bin/python2
"""A command-line tool for sharing file-data into obscured shares"""

from blocksecretsharer import BlockSecretSharer
import sys
import os.path
import io
#import pdb


KEY_FILE_NAME = "~/.ssk"


def print_help(unknown_command, command=None):
    """Prints the help-screen, listing all valid arguments."""
    if unknown_command is True:
        print "\nUNKNOWN COMMAND:", command

    print "\nThe format for this command-line tool is as follows -\n"

    print "  secretsharer -h                 : displays the help-message"
    print "  secretsharer -k                 : create new key. Keys will be"
    print "                                  : - stored at"
    print "                                  :    ", KEY_FILE_NAME
    print "  secretsharer -c <file-path>     : creates shares from the file"
    print "                                  : - identified by <file-path>"
    print "  secretsharer -r <file-path>     : reconstructs a file from shares"
    print "                                  : - of a file based on the format"
    print "                                  : - of <file-path>. Format will"
    print "                                  : - be simple -"
    print "                                  : -     <file-path>_%"
    print "                                  : - where % is the index of the"
    print "                                  : - share-file"


def create_key():
    """Creates a new encryption-key and stores in in home-directory."""

    #Get new key
    sharer = BlockSecretSharer()
    key = sharer.get_key()

    #Write key to file
    put_file_contents(os.path.expanduser(KEY_FILE_NAME), key)

    del sharer


def get_key():
    """Gets currently held encryption-key stored in home directory."""

    return get_file_contents(os.path.expanduser(KEY_FILE_NAME))


def create_file_shares(filepath):
    """Creates shares of a file at the given path."""

    success = False

    contents = get_file_contents(filepath)
    length = len(contents)

    if length > 0:
        #Get key and set up sharer
        key = get_key()

        if len(key) > 0:
            #Create secret-sharer with current key
            sharer = BlockSecretSharer(key)
        else:
            #Create secret-sharer with new key
            sharer = BlockSecretSharer()

        #Create shares from file-contents
        shares = sharer.create_shares(contents)

        #Write shared data to files
        number_of_shares = sharer.get_number_of_shares()
        for i in range(1, number_of_shares + 1):
            #Get share-name
            share_name = create_share_name(filepath, i)
            #Write share-data to file
            put_file_contents(share_name, shares[i])

        del sharer

        success = True

    return success


def reconstruct_file_from_shares(filepath):
    """Reconstructs a file at a given path from shares.
    The share-paths are inferred from the file-path."""

    success = False

    #Look for locally-stored key
    key = get_key()

    if len(key) > 0:
        #Key found, use for this operation
        sharer = BlockSecretSharer(key)

        shares = {}
        number_of_shares = sharer.get_number_of_shares()

        #Get at least 2 shared files from same directory as filepath
        for i in range(1, number_of_shares + 1):

            share_name = create_share_name(filepath, i)
            contents = get_file_contents(share_name)

            if len(contents) > 0:
                shares[i] = contents
                if len(shares) >= 2:
                    break

        if len(shares) >= 2:
            #Found at least 2 shared files from directory local to filepath.
            #Do reconstruction
            reconstructed_file = sharer.reconstruct_secret(shares)

            #Write reconstructed data to original filename.
            put_file_contents(filepath, reconstructed_file)

            success = True
        else:
            print "\nNo shared files found"

        del sharer

    else:
        print "\nNo key found, reconstruction would fail!"

    return success


def get_file_contents(filepath):
    """Gets the contents of a file at a given path."""

    file_contents = None

    if os.path.isfile(filepath):
        try:
            input_file = io.open(filepath, 'rb')
            file_contents = bytearray(input_file.read())
        except IOError:
            print "\nError: can\'t find file or read data from file at", \
                filepath
        except io.BlockingIOError:
            print "\nError: unsuported operation while reading data from file at", \
                filepath
        except:
            print "\nError: unknown error occurred while opening file for", \
                "reading at", filepath
        else:
            input_file.close()

    return file_contents


def put_file_contents(filepath, contents):
    """Writes given data (as a list) to a file at the given path."""

    try:
        #Convert data from list to array of bytes
        contents_bytes = bytearray(contents)
        output_file = io.open(filepath, 'wb')
        output_file.write(contents_bytes)
        output_file.flush()
    except IOError:
        print "\nError: can\'t find file or write data to file at", \
            filepath
    except io.BlockingIOError:
        print "\nError: unsuported operation while writing data to file at", \
            filepath
    except:
        print "\nError: unknown error occurred while opening file for", \
            "writing at", filepath
    else:
        output_file.close()


def create_share_name(filename, share_index):
    """Creates a standard share-name from a given filename and share-index."""

    sharename = ""

    if "." in filename:
        filename_parts = filename.split('.')
        num_parts = len(filename_parts)
        for i in range(0, num_parts - 1):
            sharename += filename_parts[i]
        sharename += "_"
        sharename += str(share_index)
        sharename += "."
        sharename += filename_parts[num_parts - 1]
    else:
        sharename = filename + "_" + share_index

    return sharename


def main_function(argv):
    """Main"""

    num_of_args = len(argv)

    #Check there are enough arguments
    if num_of_args == 2 and argv[1][0] == '-':

        if argv[1] == "-h":
            print_help(False)

        elif argv[1] == "-k":
            create_key()

        else:
            print_help(True, argv[1])

    elif num_of_args == 3 and argv[1][0] == '-' and argv[2][0] != '-':

        if argv[1] == "-c":
            create_file_shares(argv[2])

        elif argv[1] == "-r":
            reconstruct_file_from_shares(argv[2])

        elif argv[1] == "-g":
            bytes_read = get_file_contents(argv[2])
            print "get_file_contents operation read", str(len(bytes_read)), \
                "bytes from file", argv[2]

        else:
            print_help(True, argv[1])

    elif num_of_args == 4 and argv[1][0] == '-' and argv[2][0] != '-':

        if argv[1] == "-p":
            put_file_contents(argv[2], argv[3])

    else:

        print_help(False, argv[1:])


if __name__ == "__main__":

    main_function(sys.argv)
