#include "mujson.h"
#include <cassert>
#include <cstdlib>  /* NULL, malloc(), realloc(), free(), strtod() */
#include <cmath>    /* HUGE_VAL */
#include <cerrno>   /* errno, ERANGE */
#include <cstring>  /* memcpy() */

#ifndef PARSE_STACK_INIT_SIZE
#define PARSE_STACK_INIT_SIZE 256
#endif

namespace mujson {

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)         do { *(char*)ContextPush(c, sizeof(char)) = (ch); } while(0)


struct Context{
    const char* json;
    char* stack;
    size_t size, top;
};


static void* ContextPush(Context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* ContextPop(Context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

static void parseWhiteSpace(Context* c) {
    const char* p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    c->json = p;
}

static int parseLiteral(Context* c, JsonValue* v,
    const char* literal, TYPE type)
{
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; i++)
        if (c->json[i] != literal[i + 1])
            return STATE::PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return STATE::PARSE_OK;
}

static int parseNumber(Context* c, JsonValue* v) {
    const char* p = c->json;
    /* 负号 ... */
    if (*p == '-') p++;
    /* 整数 ... */
    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return STATE::PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    /* 小数 ... */
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return STATE::PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    /* 指数 ... */
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return STATE::PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return STATE::PARSE_NUMBER_TOO_BIG;
    c->json = p;
    v->type = TYPE::TYPE_NUMBER;
    return STATE::PARSE_OK;
}

static const char* parseHex4(const char* p, unsigned* u) {
    int i;
    *u = 0;
    for (i = 0; i < 4; i++) {
        char ch = *p++;
        *u <<= 4;
        if      (ch >= '0' && ch <= '9')  *u |= ch - '0';
        else if (ch >= 'A' && ch <= 'F')  *u |= ch - ('A' - 10);
        else if (ch >= 'a' && ch <= 'f')  *u |= ch - ('a' - 10);
        else return NULL;
    }
    return p;
}
// 等效于
// static const char* parseHex4(const char* p, unsigned* u) {
//     char* end;
//     *u = (unsigned)strtol(p, &end, 16);
//     return end == p + 4 ? end : NULL;
// }


static void encodeUtf8(Context* c, unsigned u) {
    if (u <= 0x7F) 
        PUTC(c, u & 0xFF);
    else if (u <= 0x7FF) {
        PUTC(c, 0xC0 | ((u >> 6) & 0xFF));
        PUTC(c, 0x80 | ( u       & 0x3F));
    }
    else if (u <= 0xFFFF) {
        PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
        PUTC(c, 0x80 | ((u >>  6) & 0x3F));
        PUTC(c, 0x80 | ( u        & 0x3F));
    }
    else {
        assert(u <= 0x10FFFF);
        PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
        PUTC(c, 0x80 | ((u >> 12) & 0x3F));
        PUTC(c, 0x80 | ((u >>  6) & 0x3F));
        PUTC(c, 0x80 | ( u        & 0x3F));
    }
}

#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)

static int parseString(Context* c, JsonValue* v) {
    size_t head = c->top, len;
    unsigned u, u2;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    for (;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                len = c->top - head;
                setString(v, (const char*)ContextPop(c, len), len);
                c->json = p;
                return STATE::PARSE_OK;
            case '\\':
                switch (*p++) {
                    case '\"': PUTC(c, '\"'); break;
                    case '\\': PUTC(c, '\\'); break;
                    case '/':  PUTC(c, '/' ); break;
                    case 'b':  PUTC(c, '\b'); break;
                    case 'f':  PUTC(c, '\f'); break;
                    case 'n':  PUTC(c, '\n'); break;
                    case 'r':  PUTC(c, '\r'); break;
                    case 't':  PUTC(c, '\t'); break;
                    case 'u':
                        if (!(p = parseHex4(p, &u)))
                            STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                        if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
                            if (*p++ != '\\')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (*p++ != 'u')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (!(p = parseHex4(p, &u2)))
                                STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                            if (u2 < 0xDC00 || u2 > 0xDFFF)
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                        }
                        encodeUtf8(c, u);
                        break;
                    default:
                        STRING_ERROR(STATE::PARSE_INVALID_STRING_ESCAPE);
                }
                break;
            
            case '\0':
                STRING_ERROR(STATE::PARSE_MISS_QUOTATION_MARK);
            default:
                if ((unsigned char)ch < 0x20) { 
                    STRING_ERROR(STATE::PARSE_INVALID_STRING_CHAR);
                }
                PUTC(c, ch);
        }
    }
}


static int parseValue(Context* c, JsonValue* v) {
    switch (*c->json) {
        case 't': return parseLiteral(c, v, "true", TYPE::TYPE_TRUE);
        case 'f': return parseLiteral(c, v, "false", TYPE::TYPE_FALSE);
        case 'n': return parseLiteral(c, v, "null", TYPE::TYPE_NULL);
        default:   return parseNumber(c, v);
        case '"':  return parseString(c, v);
        case '\0': return STATE::PARSE_EXPECT_VALUE;
    }
}

int parse(JsonValue* v, const char* json) {
    Context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;        /* <- */
    c.size = c.top = 0;    /* <- */
    valueInit(v);
    parseWhiteSpace(&c);
    if ((ret = parseValue(&c, v)) == STATE::PARSE_OK) {
        parseWhiteSpace(&c);
        if (*c.json != '\0') {
            v->type = TYPE::TYPE_NULL;
            ret = STATE::PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);    /* <- */
    free(c.stack);         /* <- */
    return ret;
}

void valueFree(JsonValue* v) {
    assert(v != NULL);
    if (v->type == TYPE::TYPE_STRING)
        free(v->u.s.s);
    v->type = TYPE::TYPE_NULL;
}

TYPE getType(const JsonValue* v) {
    assert(v != nullptr);
    return v->type;
}


int getBoolean(const JsonValue* v) {
    assert(v != NULL && (v->type == TYPE::TYPE_TRUE || v->type == TYPE::TYPE_FALSE));
    return v->type == TYPE::TYPE_TRUE;
}

void setBoolean(JsonValue* v, int b) {
    valueFree(v);
    v->type = b ? TYPE::TYPE_TRUE : TYPE::TYPE_FALSE;
}

double getNumber(const JsonValue* v) {
    assert(v != NULL && v->type == TYPE::TYPE_NUMBER);
    return v->u.n;
}

void setNumber(JsonValue* v, double n) {
    valueFree(v);
    v->u.n = n;
    v->type = TYPE::TYPE_NUMBER;
}


const char* getString(const JsonValue* v) {
    assert(v != NULL && v->type == TYPE::TYPE_STRING);
    return v->u.s.s;
}
size_t getStringLength(const JsonValue* v) {
    assert(v != NULL && v->type == TYPE::TYPE_STRING);
    return v->u.s.len;
}

void setString(JsonValue* v, const char* s, size_t len) {
    assert(v != NULL && (s != NULL || len == 0));
    valueFree(v);
    v->u.s.s = (char*)malloc(len + 1);
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = TYPE::TYPE_STRING;
}



}// namespace mujson
