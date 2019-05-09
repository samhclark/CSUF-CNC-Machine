#!/usr/bin/env python3
"""Utilities used for parsing G-Code files

California State University, Fresno
Lyle's College of Engineering
Electrical and Computer Engineering Department
ECE 186(A/B) - Senior Design(I/II)
Fall 2018 - Spring 2019
Plasma CNC Machine Group - Computer Engineering Team

Instructor:    Dr. Aaron Stillmaker
Author:        Sam Clark
Group Members: Jarrod Smith (Computer)
               Zachary Stewart (Computer)
               Chris Doda (Mechanical)
               Bryant Pennebaker (Mechanical)
               Elvis Pizano (Mechanical)
"""
#---------------------------------------------------------------------------------------#
# Import Statements                                                                     #
#---------------------------------------------------------------------------------------#
from itertools import tee, islice, chain  # For parsing lines
import re   # For stripping out parentheses


#---------------------------------------------------------------------------------------#
# Function definitions                                                                  #
#---------------------------------------------------------------------------------------#
def strip_parens(s):
    """Strips out the parentheses and contents from a string

    Uses regular expressions to strip any parentheticals out of a
    string.  Requires the re package.

    Arguments:
    s -- string type

    Returns:  string without the parentheses and contents
    """
    return re.sub(r" ?\([^)]+\)", "", s)

def prev_next(some_iterable):
    """Helps to iterate while keeping track of prev & next elements

    Returns the iterable with previous and next items for each element.
    Used for parsing the lines.  From https://stackoverflow.com/a/1012089
    Requires the itertools package

    Arguments:
    some_iterable -- Any iterable object

    Returns:  a zip of the prev, current, and next items
    """
    prevs, items, nexts = tee(some_iterable, 3)
    prevs = chain([None], prevs)
    nexts = chain(islice(nexts, 1, None), [None])
    return zip(prevs, items, nexts)

def four_bit_wrap(num):
    """Handles overflow for packet and command sequence"""
    while num < 0:
        num += 16
    while num >= 16:
        num -= 16
    return num

def int_to_signed_word(x):
    """Converts an integer to a 32-bit 2's complement bytes object"""
    if x >= 0:
        data = x.to_bytes(4,byteorder="big")
    else:
        twos_comp_str = hex((x + (1 << 32)) % (1 << 32))
        if len(twos_comp_str) % 2: # if there are an odd number of characters
            # Pretty confident this branch is never taken, but I'm not 100%
            # sure and it really doesn't take that much time, so I'm leaving it
            fixed_str = 'F' + twos_comp_str[2:]
        else:
            fixed_str = twos_comp_str[2:]
        data = bytes.fromhex(fixed_str).rjust(4,b'\xFF')
    return data
