import ctypes
import os
import pytest

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./libfirst_follow_of_grammar.so"))

# Define function signatures
lib.add_production.argtypes = [ctypes.c_char, ctypes.c_char_p]
lib.clear_grammar.argtypes = []

lib.compute_first_sets.argtypes = []
lib.get_first_set.argtypes = [ctypes.c_char]
lib.get_first_set.restype = ctypes.c_char_p

lib.compute_follow_sets.argtypes = []
lib.get_follow_set.argtypes = [ctypes.c_char]
lib.get_follow_set.restype = ctypes.c_char_p

def add_production(lhs, rhs):
    lib.add_production(lhs.encode('utf-8'), rhs.encode('utf-8'))

def compute_first_sets():
    lib.compute_first_sets()

def compute_follow_sets():
    lib.compute_follow_sets()

def get_first_set(symbol):
    result = lib.get_first_set(symbol.encode('utf-8')).decode('utf-8')
    return set(result.split(',')) if result else set()

def get_follow_set(symbol):
    result = lib.get_follow_set(symbol.encode('utf-8')).decode('utf-8')
    return set(result.split(',')) if result else set()

def setup_grammar(productions):
    lib.clear_grammar()
    for lhs, rhs in productions:
        add_production(lhs, rhs)
    compute_first_sets()
    compute_follow_sets()

def debug_print_follow_sets():
    print("First sets:")
    for symbol in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
        follow_set = get_follow_set(symbol)
        if follow_set:
            print(f"FOLLOW({symbol}) = {follow_set}")


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

    assert get_first_set('S') == {'a', 'b', 'c'}
    assert get_first_set('A') == {'a', '#'}
    assert get_first_set('B') == {'b', '#'}
    assert get_first_set('C') == {'c'}

    assert get_follow_set('S') == {'$'}
    assert get_follow_set('A') == {'b', 'c'}
    assert get_follow_set('B') == {'c'}
    assert get_follow_set('C') == {'$'}



def test_left_recursion():
    productions = [
        ('E', 'E+T'),
        ('E', 'T'),
        ('T', 'id')
    ]
    setup_grammar(productions)

    assert get_first_set('E') == {'id'}
    assert get_first_set('T') == {'id'}

    assert get_follow_set('E') == {'$', '+'}
    assert get_follow_set('T') == {'$', '+'}


def test_indirect_left_recursion():
    productions = [
        ('E', 'T'),
        ('E', 'E+T'),
        ('T', 'F'),
        ('T', 'T*F'),
        ('F', 'id')
    ]
    setup_grammar(productions)

    assert get_first_set('E') == {'id'}
    assert get_first_set('T') == {'id'}
    assert get_first_set('F') == {'id'}

    assert get_follow_set('E') == {'$', '+'}
    assert get_follow_set('T') == {'$', '+', '*'}
    assert get_follow_set('F') == {'$', '+', '*'}

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

    assert get_first_set('S') == {'a', 'b', 'c'}
    assert get_first_set('A') == {'a', '#'}
    assert get_first_set('B') == {'b', '#'}
    assert get_first_set('C') == {'c'}
    assert get_first_set('D') == {'d', '#'}

    assert get_follow_set('S') == {'$'}
    assert get_follow_set('A') == {'b', 'c'}
    assert get_follow_set('B') == {'c'}
    assert get_follow_set('C') == {'$'}
    assert get_follow_set('D') == {'b', 'c'}

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

    assert get_first_set('S') == {'a', 'b', 'c', '#'}
    assert get_first_set('A') == {'a', 'b', 'c', '#'}
    assert get_first_set('B') == {'b', 'c', '#'}
    assert get_first_set('C') == {'c', '#'}

    assert get_follow_set('S') == {'$'}
    assert get_follow_set('A') == {'$', 'b', 'c'}
    assert get_follow_set('B') == {'$', 'b', 'c'}
    assert get_follow_set('C') == {'$', 'b', 'c'}

def test_multi_character_terminals():
    productions = [
        ('S', 'AB'),
        ('A', 'id'),
        ('A', 'num'),
        ('B', 'while'),
        ('B', 'for')
    ]
    setup_grammar(productions)

    assert get_first_set('S') == {'id', 'num'}
    assert get_first_set('A') == {'id', 'num'}
    assert get_first_set('B') == {'while', 'for'}

    assert get_follow_set('S') == {'$'}
    assert get_follow_set('A') == {'while', 'for'}
    assert get_follow_set('B') == {'$'}

def test_mixed_terminals():
    productions = [
        ('S', 'AaBbC'),
        ('A', 'id'),
        ('A', '#'),
        ('B', 'num'),
        ('B', 'for'),
        ('C', 'c')
    ]
    setup_grammar(productions)

    assert get_first_set('S') == {'id', 'a'}
    assert get_first_set('A') == {'id', '#'}
    assert get_first_set('B') == {'num', 'for'}
    assert get_first_set('C') == {'c'}

    assert get_follow_set('S') == {'$'}
    assert get_follow_set('A') == {'a'}
    assert get_follow_set('B') == {'b'}
    assert get_follow_set('C') == {'$'}


# Run the tests
if __name__ == "__main__":
    pytest.main([__file__])
