#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "TStiva.h"
#include "TLista.h"
#include "TCoada.h"

typedef struct TNodAttr {
	char *name;
	char *value;
	struct TNodAttr *next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
	char *type;
	char *id;
	TAttr style;
	TAttr otherAttributes;
	int isSelfClosing;
	char *contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
	TInfo info;
	struct TNodArb *nextSibling;
	struct TNodArb *firstChild;
} TNodArb, *TArb;

typedef enum
{
    PARSE_ERROR             = 0,
    PARSE_CONTENTS          = 1,
    PARSE_OPENING_BRACKET   = 2,
    PARSE_TAG_TYPE          = 3,
    PARSE_CLOSING_TAG       = 4,
    PARSE_REST_OF_TAG       = 5,
    PARSE_ATTRIBUTE_NAME    = 6,
    PARSE_ATTRIBUTE_EQ      = 7,
    PARSE_ATTRIBUTE_VALUE   = 8,
    PARSE_SELF_CLOSING      = 9,
} TParseState;
