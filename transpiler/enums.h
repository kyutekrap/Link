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
