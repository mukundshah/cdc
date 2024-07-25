import ctypes
import os
import random
import string

import pytest

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./libidentifier_checker.so"))

# Define function signature
lib.check_identifier.argtypes = [ctypes.c_char_p]
lib.check_identifier.restype = ctypes.c_int


def is_valid_identifier(identifier):
    result = lib.check_identifier(identifier.encode("utf-8"))
    return "Valid" if result == 1 else "Invalid"


# Test cases
test_cases = [
    ("validIdentifier", "Valid"),
    ("_validIdentifier", "Valid"),
    ("valid_identifier_123", "Valid"),
    ("1invalidIdentifier", "Invalid"),
    ("invalid-identifier", "Invalid"),
    ("a", "Valid"),
    ("_", "Valid"),
    ("", "Invalid"),
    ("__identifier__", "Valid"),
    ("abc123", "Valid"),
    ("123abc", "Invalid"),
    ("abc_123", "Valid"),
    ("_123", "Valid"),
    ("abc!", "Invalid"),
    ("a" * 1000, "Valid"),  # Very long identifier
]


@pytest.mark.parametrize("identifier, expected", test_cases)
def test_identifier(identifier, expected):
    assert is_valid_identifier(identifier) == expected


def test_random_valid_identifiers():
    for _ in range(100):
        length = random.randint(1, 50)
        first_char = random.choice(string.ascii_letters + "_")
        rest_chars = "".join(
            random.choices(string.ascii_letters + string.digits + "_", k=length - 1)
        )
        identifier = first_char + rest_chars
        assert is_valid_identifier(identifier) == "Valid"


def test_random_invalid_identifiers():
    for _ in range(100):
        length = random.randint(1, 50)
        identifier = "".join(
            random.choices(string.punctuation.replace("_", ""), k=length)
        )
        assert is_valid_identifier(identifier) == "Invalid"


def test_edge_cases():
    assert is_valid_identifier("_" * 1000) == "Valid"
    assert is_valid_identifier("a" * 1000 + "1" * 1000) == "Valid"
    assert is_valid_identifier("_" + "1" * 1000) == "Valid"
    assert is_valid_identifier("1" * 1000) == "Invalid"
    assert is_valid_identifier("_" + "@" * 1000) == "Invalid"


def test_unicode_characters():
    assert is_valid_identifier("válid") == "Invalid"
    assert is_valid_identifier("invalid¡") == "Invalid"
    assert is_valid_identifier("_válid") == "Invalid"
