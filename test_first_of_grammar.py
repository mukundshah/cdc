import ctypes
import os
import pytest

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./libfirst_of_grammar.so"))

# Define function signatures
lib.add_production.argtypes = [ctypes.c_char, ctypes.c_char_p]
lib.clear_grammar.argtypes = []
lib.compute_first_sets.argtypes = []
lib.get_first_set.argtypes = [ctypes.c_char]
lib.get_first_set.restype = ctypes.c_char_p

def add_production(lhs, rhs):
    lib.add_production(lhs.encode('utf-8'), rhs.encode('utf-8'))

def compute_first_sets():
    lib.compute_first_sets()

def get_first_set(symbol):
    result = lib.get_first_set(symbol.encode('utf-8')).decode('utf-8')
    return set(result.split(',')) if result else set()

def setup_grammar(productions):
    lib.clear_grammar()
    for lhs, rhs in productions:
        add_production(lhs, rhs)
    compute_first_sets()

# Test cases
def test_example_grammar():
    productions = [
        ('S', 'ABC'),
        ('A', 'aA'),
        ('A', '#'),
        ('B', 'bB'),
        ('B', '#'),
        ('C', 'c')
    ]
    setup_grammar(productions)

    assert get_first_set('S') == {'a', 'b', 'c', '#'}
    assert get_first_set('A') == {'a', '#'}
    assert get_first_set('B') == {'b', '#'}
    assert get_first_set('C') == {'c'}

def test_left_recursion():
    productions = [
        ('E', 'E+T'),
        ('E', 'T'),
        ('T', 'id')
    ]
    setup_grammar(productions)

    assert get_first_set('E') == {'id'}
    assert get_first_set('T') == {'id'}

def test_epsilon_propagation():
    productions = [
        ('S', 'ABC'),
        ('A', 'aD'),
        ('A', '#'),
        ('B', 'b'),
        ('B', '#'),
        ('C', 'c'),
        ('D', 'd'),
        ('D', '#')
    ]
    setup_grammar(productions)

    assert get_first_set('S') == {'a', 'b', 'c', '#'}
    assert get_first_set('A') == {'a', '#'}
    assert get_first_set('B') == {'b', '#'}
    assert get_first_set('C') == {'c'}
    assert get_first_set('D') == {'d', '#'}

def test_complex_grammar():
    productions = [
        ('S', 'ABC'),
        ('A', 'aA'),
        ('A', 'BC'),
        ('B', 'bB'),
        ('B', 'C'),
        ('C', 'cC'),
        ('C', '#')
    ]
    setup_grammar(productions)

    assert get_first_set('S') == {'a', 'b', 'c'}
    assert get_first_set('A') == {'a', 'b', 'c'}
    assert get_first_set('B') == {'b', 'c', '#'}
    assert get_first_set('C') == {'c', '#'}

# Run the tests
if __name__ == "__main__":
    pytest.main([__file__])
