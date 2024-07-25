import ctypes
import pytest

# Load the C library
lib = ctypes.CDLL('./libll1_parser.so')

# Define function prototypes
lib.add_production.argtypes = [ctypes.c_char, ctypes.c_char_p]
lib.compute_first_sets.argtypes = []
lib.compute_follow_sets.argtypes = []
lib.create_parsing_table.argtypes = []
lib.ll1_parse.argtypes = [ctypes.c_char_p]
lib.ll1_parse.restype = ctypes.c_bool
lib.clear_grammar.argtypes = []

def setup_grammar(productions):
    lib.clear_grammar()
    for lhs, rhs in productions:
        lib.add_production(lhs.encode(), rhs.encode())
    lib.compute_first_sets()
    lib.compute_follow_sets()
    lib.create_parsing_table()

def test_simple_grammar():
    productions = [
        ('S', 'aAB'),
        ('A', 'bC'),
        ('B', 'c'),
        ('C', 'd')
    ]
    setup_grammar(productions)

    assert not lib.ll1_parse(b"abcd$")
    assert not lib.ll1_parse(b"abc$")
    assert not lib.ll1_parse(b"abcde$")

def test_grammar_with_epsilon():
    productions = [
        ('S', 'aA'),
        ('A', 'bA'),
        ('A', '#')
    ]
    setup_grammar(productions)

    assert lib.ll1_parse(b"a$")
    assert lib.ll1_parse(b"ab$")
    assert lib.ll1_parse(b"abb$")
    assert not lib.ll1_parse(b"b$")
    assert not lib.ll1_parse(b"aa$")

def test_complex_grammar():
    productions = [
        ('E', 'TR'),
        ('R', '+TR'),
        ('R', '#'),
        ('T', 'FY'),
        ('Y', '*FY'),
        ('Y', '#'),
        ('F', '(E)'),
        ('F', 'id')
    ]
    setup_grammar(productions)

    assert lib.ll1_parse(b"i+i*i$")
    assert lib.ll1_parse(b"(i+i)*i$")
    assert lib.ll1_parse(b"i$")
    assert not lib.ll1_parse(b"i+$")
    assert not lib.ll1_parse(b"i*+i$")
