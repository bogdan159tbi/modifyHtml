/*
 *  Pentru a ușura citirea codului HTML din fișier, vi se pune la dispoziție
 *  următoarea funcție. Aceasta primeste un singur caracter (fișierul va fi
 *  citit caracter cu caracter în afara acestei funcții, iar ea va fi apelată
 *  de fiecare dată) și, în funcție de caracter, trece dintr-o stare în alta.
 *  Comentariile din cod arată o sugestie (neobligatorie) despre cum ar trebui
 *  completată funcția pentru a construi arborele. 
 * 
 *  Este, cu alte cuvinte, un automat de stări, unde starea inițială este PARSE_CONTENTS.
 */

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

TParseState Interpret(TParseState currentState, char c, TArb *a)
{
    TParseState nextState;

    switch (currentState)
    {
    case PARSE_CONTENTS:
        if (c == '<')           {   nextState = PARSE_OPENING_BRACKET;  }
        else                    {   nextState = PARSE_CONTENTS; 
                                    /* TODO append to `info->contents` string */ }
        break;
    case PARSE_OPENING_BRACKET:
        if (isspace(c))         {   nextState = PARSE_OPENING_BRACKET;  }
        else if (c == '>')      {   nextState = PARSE_ERROR;            }
        else if (c == '/')      {   nextState = PARSE_CLOSING_TAG;      }
        else                    {   nextState = PARSE_TAG_TYPE; 
                                    /* TODO create tag node */ }
        break;
    case PARSE_TAG_TYPE:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else                    {   nextState = PARSE_TAG_TYPE; 
                                    /* TODO append to `info->type` string */ }
        break;
    case PARSE_CLOSING_TAG:
        if (c == '>')           {   nextState = PARSE_CONTENTS; 
                                    /* TODO insert tag node in `children` list */ }
        else                    {   nextState = PARSE_CLOSING_TAG;      }
        break;
    case PARSE_REST_OF_TAG:
        if (isspace(c))         {   nextState = PARSE_REST_OF_TAG;      }
        else if (c == '>')      {   nextState = PARSE_CONTENTS;         }
        else if (c == '/')      {   nextState = PARSE_SELF_CLOSING;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME;   }
        break;
    case PARSE_ATTRIBUTE_NAME:
        if (c == '=')           {   nextState = PARSE_ATTRIBUTE_EQ;     }
        else                    {   nextState = PARSE_ATTRIBUTE_NAME; 
                                    /* TODO append to `info->temp->name` string */ }
        break;
    case PARSE_ATTRIBUTE_EQ:
        if (c == '\"')          {   nextState = PARSE_ATTRIBUTE_VALUE;  }
        break;
    case PARSE_ATTRIBUTE_VALUE:
        if (c == '\"')          {   nextState = PARSE_REST_OF_TAG; 
                                    /* TODO create attribute from the `info->temp->*` strings*/ }
        else                    {   nextState = PARSE_ATTRIBUTE_VALUE; 
                                    /* TODO append to `info->temp->value` string */ }
        break;
    case PARSE_SELF_CLOSING:
        if (c == '>')           {   nextState = PARSE_CONTENTS; 
                                    /* TODO insert self-closing tag node in `children` list*/ }
        else                    {   nextState = PARSE_ERROR;            }
        break;
    default:
        break;
    }

    /* For debugging */
    printf("%d", nextState);
    if (c == '\n') printf("\n");

    return nextState;
}