import ctypes
import os
import pytest

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./liblexical_analyzer.so"))

# Define function signature
lib.analyze_input.argtypes = [ctypes.c_char_p]
lib.analyze_input.restype = ctypes.c_char_p

def analyze(input_str):
    result = lib.analyze_input(input_str.encode('utf-8'))
    return result.decode('utf-8')

def test_lexical_analyzer():
    input_str = "int main() { int x = 5 + 3; return 0; }"
    result = analyze(input_str)
    expected_output = """Token: int        Type: KEYWORD
Token: main       Type: IDENTIFIER
Token: (          Type: OPERATOR
Token: )          Type: OPERATOR
Token: {          Type: OPERATOR
Token: int        Type: KEYWORD
Token: x          Type: IDENTIFIER
Token: =          Type: OPERATOR
Token: 5          Type: NUMBER
Token: +          Type: OPERATOR
Token: 3          Type: NUMBER
Token: ;          Type: OPERATOR
Token: return     Type: KEYWORD
Token: 0          Type: NUMBER
Token: ;          Type: OPERATOR
Token: }          Type: OPERATOR
"""
    assert result == expected_output

def test_keywords():
    keywords = ["int", "float", "char", "if", "else", "while", "for", "return"]
    for keyword in keywords:
        result = analyze(keyword)
        assert result == f"Token: {keyword:<10} Type: KEYWORD\n"

def test_identifiers():
    identifiers = ["x", "y", "variable_name", "_underscore", "camelCase", "PascalCase"]
    for identifier in identifiers:
        result = analyze(identifier)
        assert result == f"Token: {identifier:<10} Type: IDENTIFIER\n"

def test_numbers():
    numbers = ["0", "123", "3.14", "0.123", "1000000"]
    for number in numbers:
        result = analyze(number)
        assert result == f"Token: {number:<10} Type: NUMBER\n"

def test_operators():
    operators = ["+", "-", "*", "/", "=", "<", ">", "(", ")", "{", "}"]
    for operator in operators:
        result = analyze(operator)
        assert result == f"Token: {operator:<10} Type: OPERATOR\n"

def test_mixed_input():
    input_str = "if (x < 10) { x = x + 1; }"
    result = analyze(input_str)
    expected_output = """Token: if         Type: KEYWORD
Token: (          Type: OPERATOR
Token: x          Type: IDENTIFIER
Token: <          Type: OPERATOR
Token: 10         Type: NUMBER
Token: )          Type: OPERATOR
Token: {          Type: OPERATOR
Token: x          Type: IDENTIFIER
Token: =          Type: OPERATOR
Token: x          Type: IDENTIFIER
Token: +          Type: OPERATOR
Token: 1          Type: NUMBER
Token: ;          Type: OPERATOR
Token: }          Type: OPERATOR
"""
    assert result == expected_output

def test_unknown_characters():
    input_str = "!@#$%^&*"
    result = analyze(input_str)
    expected_output = "".join([f"Token: {c:<10} Type: UNKNOWN\n" for c in "!@#$%^&"])
    expected_output += "Token: *          Type: OPERATOR\n"
    assert result == expected_output

def test_empty_input():
    result = analyze("")
    assert result == ""

def test_long_input():
    input_str = "a" * 1000
    result = analyze(input_str)

    # Calculate the number of full-length tokens
    full_tokens = 1000 // 99
    remainder = 1000 % 99

    expected_output = ""
    for _ in range(full_tokens):
        expected_output += f"Token: {'a' * 99:<10} Type: IDENTIFIER\n"

    if remainder > 0:
        expected_output += f"Token: {'a' * remainder:<10} Type: IDENTIFIER\n"

    assert result == expected_output

if __name__ == "__main__":
    pytest.main([__file__])
