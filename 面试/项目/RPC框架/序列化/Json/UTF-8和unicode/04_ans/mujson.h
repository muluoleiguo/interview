#pragma once

#include <cstddef> /* size_t */

namespace mujson {

enum TYPE {
    TYPE_NULL = 0,
    TYPE_FALSE,
    TYPE_TRUE,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT
};

enum STATE {
    PARSE_OK = 0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE
};

struct JsonValue {
    union {
        struct { char* s; size_t len; }s;  /* string: null-terminated string, string length */
        double n;                          /* number */
    }u;
    TYPE type;
};

#define valueInit(v) do { (v)->type = TYPE::TYPE_NULL; } while(0)

int parse(JsonValue* v, const char* json);

void valueFree(JsonValue* v);

TYPE getType(const JsonValue* v);

#define setNull(v) valueFree(v)

int getBoolean(const JsonValue* v);
void setBoolean(JsonValue* v, int b);

double getNumber(const JsonValue* v);
void setNumber(JsonValue* v, double n);

const char* getString(const JsonValue* v);
size_t getStringLength(const JsonValue* v);
void setString(JsonValue* v, const char* s, size_t len);

} //namespace mujson
