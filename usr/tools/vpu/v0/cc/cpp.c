#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zero/trix.h>
#include <cc/cc.h>

extern struct ccmach ccmach;
extern long          typesztab[32];

#if (CCPRINT)
static void ccprinttoken(struct cctoken *token);
#endif
static struct cctoken * ccgetfunc(struct cctoken *token,
                                  struct cctoken **nextret,
                                  struct cctoken **lastret);

typedef struct cctoken * cctokenfunc_t(struct cctoken *,
                                       struct cctoken **,
                                       struct cctoken **);

struct cctoken *ccprocif(struct cctoken *, struct cctoken **,
                         struct cctoken **);
struct cctoken *ccprocifdef(struct cctoken *, struct cctoken **,
                            struct cctoken **);
struct cctoken *ccprocifndef(struct cctoken *, struct cctoken **,
                             struct cctoken **);
struct cctoken *ccprocdefine(struct cctoken *, struct cctoken **,
                             struct cctoken **);

#if (CCPRINT) || (CCDEBUG)
static char *toktypetab[256] =
{
    NULL,
    "CC_TYPE_TOKEN",
    "CC_TYPEDEF_TOKEN",
    "CC_VAR_TOKEN",
    "CC_STRUCT_TOKEN",
    "CC_UNION_TOKEN",
    "CC_OPER_TOKEN",
    "CC_DOT_TOKEN",
    "CC_INDIR_TOKEN",
    "CC_ASTERISK_TOKEN",
    "CC_COMMA_TOKEN",
    "CC_SEMICOLON_TOKEN",
    "CC_COLON_TOKEN",
    "CC_EXCLAMATION_TOKEN",
    "CC_LEFT_PAREN_TOKEN",
    "CC_RIGHT_PAREN_TOKEN",
    "CC_INDEX_TOKEN",
    "CC_END_INDEX_TOKEN",
    "CC_BLOCK_TOKEN",
    "CC_END_BLOCK_TOKEN",
    "CC_QUOTE_TOKEN",
    "CC_DOUBLE_QUOTE_TOKEN",
    "CC_BACKSLASH_TOKEN",
    "CC_NEWLINE_TOKEN",
    "CC_VALUE_TOKEN",
    "CC_STRING_TOKEN",
    "CC_LITERAL_TOKEN",
    "CC_QUAL_TOKEN",
    "CC_ATR_TOKEN",
    "CC_FUNC_TOKEN",
    "CC_LABEL_TOKEN",
    "CC_ADR_TOKEN",
    "CC_MACRO_TOKEN",
    "CC_PREPROC_TOKEN",
    "CC_CONCAT_TOKEN",
    "CC_STRINGIFY_TOKEN"
#if 0
    "CC_LATIN1_TOKEN",
    "CC_UTF8_TOKEN",
    "CC_UCS16_TOKEN",
    "CC_UCS32_TOKEN"
#endif
    };
static char *tokparmtab[256] =
{
    "NONE",
    "CC_EXTERN_QUAL",
    "CC_STATIC_QUAL",
    "CC_CONST_QUAL",
    "CC_VOLATILE_QUAL",
    "CC_IF_DIR",
    "CC_ELIF_DIR",
    "CC_ELSE_DIR",
    "CC_ENDIF_DIR",
    "CC_IFDEF_DIR",
    "CC_IFNDEF_DIR",
    "CC_DEFINE_DIR"
    };
static char *typetab[32] =
{
    "NONE",
    "CC_CHAR",
    "CC_UCHAR",
    "CC_SHORT",
    "CC_USHORT",
    "CC_INT",
    "CC_UINT",
    "CC_LONG",
    "CC_ULONG",
    "CC_LONGLONG",
    "CC_ULONGLONG",
    "CC_FLOAT",
    "CC_DOUBLE",
    "CC_LDOUBLE",
    "CC_VOID"
    };
#endif /* CCPRINT */
static cctokenfunc_t *dirfunctab[16] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ccprocif,           // CC_IF_DIR
    NULL,               // CC_ELIF_DIR
    NULL,               // CC_ELSE_DIR
    NULL,               // CC_ENDIF_DIR
    ccprocifdef,        // CC_IFDEF_DIR
    ccprocifndef,       // CC_IFNDEF_DIR
    ccprocdefine        // CC_DEFINE_DIR
    };

#if (CCPRINT) || (CCDEBUG)

void
ccprintval(struct ccval *val)
{
    switch (val->type) {
        case CC_CHAR:
            fprintf(stderr, "CHAR: %x\n", val->ival.c);

            break;
        case CC_UCHAR:
            fprintf(stderr, "UCHAR: %x\n", val->ival.uc);

            break;
        case CC_SHORT:
            fprintf(stderr, "SHORT: %x\n", val->ival.s);

            break;
        case CC_USHORT:
            fprintf(stderr, "USHORT: %x\n", val->ival.us);

            break;
        case CC_INT:
            fprintf(stderr, "INT: %x\n", val->ival.i);

            break;
        case CC_UINT:
            fprintf(stderr, "UINT: %x\n", val->ival.ui);

            break;
        case CC_LONG:
            fprintf(stderr, "LONG: %lx\n", val->ival.l);

            break;
        case CC_ULONG:
            fprintf(stderr, "ULONG: %lx\n", val->ival.ul);

            break;
        case CC_LONGLONG:
            fprintf(stderr, "LONGLONG: %llx\n", val->ival.ll);

        case CC_ULONGLONG:
            fprintf(stderr, "ULONGLONG: %llx\n", val->ival.ull);

            break;

        default:

            break;
    }
    fprintf(stderr, "SZ: %ld\n", val->sz);

    return;
}

static void
ccprinttoken(struct cctoken *token)
{
    fprintf(stderr, "TYPE %s\n", toktypetab[token->type]);
    if (token->type == CC_TYPE_TOKEN) {
        fprintf(stderr, "PARM: %s\n", typetab[token->parm]);
    } else {
        fprintf(stderr, "PARM: %s\n", tokparmtab[token->parm]);
    }
    fprintf(stderr, "STR: %s\n", token->str);
    if (token->type == CC_VALUE_TOKEN) {
        fprintf(stderr, "VALUE\n");
        fprintf(stderr, "-----\n");
        ccprintval((void *)token->data);
    } else {
        fprintf(stderr, "DATA: %lx\n", (unsigned long)token->data);
    }
}

void
ccprintqueue(struct cctokenq *queue)
{
    struct cctoken *token = queue->head;

    while (token) {
        ccprinttoken(token);
        token = token->next;
    }
}

#endif /* CCPRINT */

#if 0
struct cctoken *
ccprocif(struct cctoken *token, struct cctoken **nextret,
         struct cctoken **tailret)
{
    struct cctoken *head = NULL;
    struct cctoken *tail = NULL;
    long            block = 0;
    long            skip = 0;
    long            loop = 1;

    token = token->next;
    skip = !cceval(token, &token);
    while (loop) {
        while (token) {
            if (!skip) {
                ccqueuetoken(token, &head, &tail);
            }
            token = token->next;
            if (token->type == CC_ELSE_TOKEN) {
                if (skip) {
                    skip = !skip;
                    token = token->next;
                } else {
                    while (token->type != CC_ENDIF_TOKEN) {
                        token = token->next;
                    }
                    loop = 0;
                }
            } else if (token->type == CC_ELIF_TOKEN) {
                if (skip) {
                    skip = !cceval(token, &token);
                } else {
                    while (token->type != CC_ENDIF_TOKEN) {
                        token = token->next;
                    }
                    loop = 0;
                }
            } else if (token->type == CC_ENDIF_TOKEN) {
                loop = 0;
            }
        }
    }
    if (head) {
        *nextret = token;
    }

    return head;
}
#endif
struct cctoken *
ccprocif(struct cctoken *token, struct cctoken **nextret,
         struct cctoken **tailret)
{
    return NULL;
}

struct cctoken *
ccprocifdef(struct cctoken *token, struct cctoken **nextret,
            struct cctoken **tailret)
{
    return NULL;
}

struct cctoken *
ccprocifndef(struct cctoken *token, struct cctoken **nextret,
             struct cctoken **tailret)
{
    return NULL;
}

struct cctoken *
ccprocdefine(struct cctoken *token, struct cctoken **nextret,
             struct cctoken **tailret)
{
    return NULL;
}

static struct cctoken *
ccgetblock(struct cctoken *token, struct cctoken **nextret,
           struct cctoken **lastret)
{
    return NULL;
}

static struct cctoken *
ccgetfunc(struct cctoken *token, struct cctoken **nextret,
          struct cctoken **lastret)
{
    return NULL;
}

void
ccqueuetoken(struct cctoken *token, struct cctoken *last,
             struct cctoken **queue, struct cctoken **tail)
{
    if (token) {
        token->next = NULL;
        if (!*queue) {
            token->prev = NULL;
            *queue = token;
            *tail = last;
        } else if (*tail) {
            token->prev = *tail;
            (*tail)->next = token;
            if (last) {
                last->next = NULL;
                *tail = last;
            } else {
                *tail = token;
            }
        } else {
            token->prev = *queue;
            (*queue)->next = token;
            if (last) {
                *tail = last;
            } else {
                *tail = token;
            }
        }
    }

    return;
}

struct cctoken *
ccpreproc(struct cctoken *token, struct cctoken **tailret)
{
    struct cctoken *head = NULL;
    struct cctoken *tail = NULL;
    struct cctoken *tok = NULL;
    struct cctoken *last = NULL;
    long            parm;
    cctokenfunc_t  *func;

    while (token) {
        if (token->type == CC_TYPEDEF_TOKEN
           || token->type == CC_TYPE_TOKEN) {
            tok = ccmktype(token, &token, &last);
            if (tok) {
                ccqueuetoken(tok, last, &head, &tail);
            }
            if (token->type == CC_FUNC_TOKEN) {
                tok = ccgetfunc(token, &token, &last);
            }
        } else if (token->type == CC_PREPROC_TOKEN) {
            parm = token->parm;
            func = dirfunctab[parm];
            if (func) {
                tok = func(token, &token, &last);
                if (!tok) {
                    fprintf(stderr, "invalid token %s\n", token->str);
                }
            }
        }
        if (tok) {
            ccqueuetoken(tok, last, &head, &tail);
        } else {
            fprintf(stderr, "unexpected token\n");
#if (CCDEBUG)
            ccprinttoken(token);
#endif

            exit(1);
        }
    }
    if (head) {
        *tailret = tail;
    }

    return head;
}

