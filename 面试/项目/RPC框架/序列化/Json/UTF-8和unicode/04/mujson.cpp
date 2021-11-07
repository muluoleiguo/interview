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

static int parseString(Context* c, JsonValue* v) {
    size_t head = c->top, len;
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
                    /*case 'u':
                        if (!(p = parseHex4(p, &u)))
                            STRING_ERROR(STATE::PARSE_INVALID_UNICODE_HEX);
                         TODO surrogate handling 
                        encodeUtf8(c, u);
                        break; */
                    default:
                        c->top = head;
                        return STATE::PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            
            case '\0':
                c->top = head;
                return STATE::PARSE_MISS_QUOTATION_MARK;
            default:
                if ((unsigned char)ch < 0x20) { 
                    c->top = head;
                    return STATE::PARSE_INVALID_STRING_CHAR;
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
