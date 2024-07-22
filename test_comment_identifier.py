import ctypes
import os
import pytest
from enum import IntEnum

# Load the shared library
lib = ctypes.CDLL(os.path.abspath("./libcomment_identifier.so"))

# Define function signature
lib.identify_comment_wrapper.argtypes = [ctypes.c_char_p]
lib.identify_comment_wrapper.restype = ctypes.c_int

class CommentType(IntEnum):
    SINGLE_LINE = 9  # SINGLE_LINE_COMMENT_COMPLETE
    MULTI_LINE = 10  # MULTI_LINE_COMMENT_COMPLETE
    DOCUMENTATION = 11  # DOCUMENTATION_COMMENT_COMPLETE
    INVALID = 12  # INVALID

    @classmethod
    def to_string(cls, value):
        return {
            cls.SINGLE_LINE: "Single Line Comment",
            cls.MULTI_LINE: "Multi Line Comment",
            cls.DOCUMENTATION: "Documentation Comment",
            cls.INVALID: "Not a valid comment"
        }.get(value, "Not a valid comment")

@pytest.fixture
def comment_identifier():
    return lambda input_str: CommentType.to_string(lib.identify_comment_wrapper(input_str.encode('utf-8')))


test_cases = [
    ("/* This is a multi-line comment */", CommentType.MULTI_LINE),
    ("/* This is a multi-line comment", CommentType.INVALID),
    ("This is not a comment", CommentType.INVALID),
    ("// This is a single line comment", CommentType.SINGLE_LINE),
    ("This is not a comment // This is a single line comment", CommentType.INVALID),
    ("This is not a comment /* This is a multi-line comment */", CommentType.INVALID),
    ("This is not a comment /* This is a multi-line comment", CommentType.INVALID),
    ("/** This is a documentation comment */", CommentType.DOCUMENTATION),
    ("/** This is a documentation comment", CommentType.INVALID),
    ("This is not a comment /** This is a documentation comment */", CommentType.INVALID),
    ("This is not a comment /** This is a documentation comment", CommentType.INVALID),
]

@pytest.mark.parametrize("input_str, expected", test_cases)
def test_identify_comment(comment_identifier, input_str, expected):
    assert comment_identifier(input_str) == CommentType.to_string(expected)

def test_unknown_comment_type(comment_identifier):
    assert comment_identifier("Some random string") == "Not a valid comment"

def test_empty_string(comment_identifier):
    assert comment_identifier("") == "Not a valid comment"

def test_non_ascii_characters(comment_identifier):
    assert comment_identifier("// コメント") == "Single Line Comment"
    assert comment_identifier("/* マルチライン コメント */") == "Multi Line Comment"

def test_nested_comments(comment_identifier):
    assert comment_identifier("/* outer /* inner */ */") == "Multi Line Comment"

def test_mixed_comment_types(comment_identifier):
    assert comment_identifier("// Single line /* Multi-line */") == "Single Line Comment"
    assert comment_identifier("/* Multi-line // Single line */") == "Multi Line Comment"

def test_whitespace_handling(comment_identifier):
    assert comment_identifier("   //   Padded comment   ") == "Single Line Comment"
    assert comment_identifier("   /*   Padded multi-line comment   */   ") == "Multi Line Comment"


if __name__ == "__main__":
    pytest.main([__file__])
