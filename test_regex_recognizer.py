import ctypes
import os
import pytest
import random
import string

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./libregex_recognizer.so"))

# Define function signature
lib.recognize_pattern_wrapper.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.recognize_pattern_wrapper.restype = ctypes.c_int

def recognize_pattern(input_str, pattern):
    result = lib.recognize_pattern_wrapper(input_str.encode('utf-8'), pattern.encode('utf-8'))
    return "Accepted" if result == 1 else "Rejected"

# Test cases
test_cases = [
    # a* pattern tests
    ("", "a*", "Accepted"),
    ("a", "a*", "Accepted"),
    ("aa", "a*", "Accepted"),
    ("aaa", "a*", "Accepted"),
    ("aaaaaaaaaa", "a*", "Accepted"),
    ("b", "a*", "Rejected"),
    ("ab", "a*", "Rejected"),
    ("ba", "a*", "Rejected"),
    ("aab", "a*", "Rejected"),

    # a*b+ pattern tests
    ("b", "a*b+", "Accepted"),
    ("ab", "a*b+", "Accepted"),
    ("aab", "a*b+", "Accepted"),
    ("aaabbb", "a*b+", "Accepted"),
    ("bbb", "a*b+", "Accepted"),
    ("", "a*b+", "Rejected"),
    ("a", "a*b+", "Rejected"),
    ("ba", "a*b+", "Rejected"),
    ("aba", "a*b+", "Rejected"),
    ("aabbc", "a*b+", "Rejected"),

    # abb pattern tests
    ("abb", "abb", "Accepted"),
    ("", "abb", "Rejected"),
    ("ab", "abb", "Rejected"),
    ("abbb", "abb", "Rejected"),
    ("aabb", "abb", "Rejected"),
    ("babb", "abb", "Rejected"),
    ("abbab", "abb", "Rejected"),

    # Edge cases and mixed inputs
    ("aaaaaaaaaaaaaab", "a*b+", "Accepted"),
    ("abbbbbbbbbbbbbb", "a*b+", "Accepted"),
    ("aaaaaaaaaaaaaaaabbbbbbbbbbbbbbbb", "a*b+", "Accepted"),
    ("aaaaaaaaaaaaaaa", "a*", "Accepted"),
    ("abba", "abb", "Rejected"),
]

@pytest.mark.parametrize("input_str, pattern, expected", test_cases)
def test_recognize_pattern(input_str, pattern, expected):
    assert recognize_pattern(input_str, pattern) == expected

def test_unsupported_pattern():
    assert recognize_pattern("abc", "abc+") == "Rejected"

# Generate random strings for a* pattern
@pytest.mark.parametrize("_", range(20))
def test_random_a_star(_):
    length = random.randint(0, 100)
    input_str = ''.join(random.choice('a') for _ in range(length))
    assert recognize_pattern(input_str, "a*") == "Accepted"

    # Add a non-'a' character to make it reject
    if length > 0:
        pos = random.randint(0, length-1)
        input_str = input_str[:pos] + random.choice('bcdefghijklmnopqrstuvwxyz') + input_str[pos+1:]
    else:
        input_str = random.choice('bcdefghijklmnopqrstuvwxyz')
    assert recognize_pattern(input_str, "a*") == "Rejected"

# Generate random strings for a*b+ pattern
@pytest.mark.parametrize("_", range(20))
def test_random_a_star_b_plus(_):
    a_length = random.randint(0, 50)
    b_length = random.randint(1, 50)
    input_str = ''.join(random.choice('a') for _ in range(a_length)) + ''.join(random.choice('b') for _ in range(b_length))
    assert recognize_pattern(input_str, "a*b+") == "Accepted"

    # Modify the string to make it reject
    if random.choice([True, False]):
        # Add a non 'a' or 'b' character
        pos = random.randint(0, len(input_str))
        input_str = input_str[:pos] + random.choice('cdefghijklmnopqrstuvwxyz') + input_str[pos:]
    else:
        # Remove all 'b's
        input_str = ''.join(c for c in input_str if c != 'b')
    assert recognize_pattern(input_str, "a*b+") == "Rejected"

# Generate random strings for abb pattern
@pytest.mark.parametrize("_", range(20))
def test_random_abb(_):
    input_str = "abb"
    assert recognize_pattern(input_str, "abb") == "Accepted"

    # Modify the string to make it reject
    modifications = [
        lambda s: s[1:],                  # Remove first character
        lambda s: s[:-1],                 # Remove last character
        lambda s: s + 'a',                # Add 'a' at the end
        lambda s: 'a' + s,                # Add 'a' at the beginning
        lambda s: s.replace('a', 'b', 1), # Replace 'a' with 'b'
        lambda s: s.replace('b', 'a', 1), # Replace one 'b' with 'a'
    ]
    input_str = random.choice(modifications)(input_str)
    assert recognize_pattern(input_str, "abb") == "Rejected"

def test_long_input():
    long_input = 'a' * 10000 + 'b' * 10000
    assert recognize_pattern(long_input, "a*b+") == "Accepted"

def test_invalid_characters():
    invalid_chars = string.punctuation + string.digits + string.whitespace
    for char in invalid_chars:
        assert recognize_pattern(char, "a*") == "Rejected"
        assert recognize_pattern(char, "a*b+") == "Rejected"
        assert recognize_pattern(char, "abb") == "Rejected"

def test_empty_input():
    assert recognize_pattern("", "a*") == "Accepted"
    assert recognize_pattern("", "a*b+") == "Rejected"
    assert recognize_pattern("", "abb") == "Rejected"
