#ifndef enums_H
#define enums_H

typedef enum {
    INVALID_FILE,
    FILE_NOT_FOUND,
    UNKNOWN_IDENTIFIER,
    UNKNOWN_PROPERTY,
    C_COMPILE_ERROR,
    UNKNOWN_FUNCTION,
    UNKNOWN_DATATYPE,
    INVALID_FILENAME,
    PREDEFINED_VARIABLE,
    INVALID_IMPORT,
    NO_ERROR,
    UNDEFINED_VARIABLE,
    INVALID_VARIABLE,
    ILLEGAL_DATATYPE,
    REPEATED_KEY,
    UNMATCHED_ELEMENTS
} ErrorCode;

typedef enum {
    CommentText,
    IdentifierText,
    PropertyText,
    FreeLineText
} TextType;

typedef enum {
    OneLineComment,
    MultiLineComment
} CommentType;

typedef enum {
    Flow,
    Step,
    Data,
    UnknownIdentifier
} IdentifierType;

typedef enum {
    Global,
    Debug,
    Import,
    UnknownProperty
} PropertyType;

typedef enum {
    String,
    Integer,
    Double,
    List,
    StringList,
    IntegerList,
    DoubleList,
    Map,
    StringMap,
    IntegerMap,
    DoubleMap,
    UnknownDataType,
    None
} DataType;

typedef enum {
    EqualTo,
    GreaterThan,
    LesserThan,
    GreaterOrEqual,
    LesserOrEqual,
    UnknownComparator
} ComparatorType;

typedef enum {
    StringLiteral,
    IntegerStatic,
    DoubleStatic,
    StringVar,
    IntegerVar,
    DoubleVar,
    UnknownComparatorDataType
} ComparatorDataType;

#endif