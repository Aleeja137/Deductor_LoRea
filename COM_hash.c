/* C code produced by gperf version 3.1 */
/* Command-line: gperf COM123+1.signature.gperf  */
/* Computed positions: -k'2,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "COM123+1.signature.gperf"

#include <string.h>
#line 12 "COM123+1.signature.gperf"
struct Symbol {
    const char* name;
    int value;
};

#define TOTAL_KEYWORDS 27
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 11
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 42
/* maximum key range = 42, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register size_t len;
{
  static const unsigned char asso_values[] =
    {
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 19, 43, 43, 43, 43, 28, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 30, 43,
      43, 20, 15, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43,  0, 43, 43, 43, 10,  5, 43,
       5,  0, 43, 10, 43,  0, 43, 15, 10,  0,
       5, 43,  5, 43,  0,  0,  5, 43, 20,  5,
      43,  0, 43, 43,  0, 43,  0, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
      43, 43, 43, 43, 43, 43
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

const struct Symbol *
get_value (str, len)
     register const char *str;
     register size_t len;
{
  static const struct Symbol wordlist[] =
    {
      {"",0},
#line 44 "COM123+1.signature.gperf"
      {"~",27},
#line 18 "COM123+1.signature.gperf"
      {"[]",1},
#line 24 "COM123+1.signature.gperf"
      {"[|]",7},
#line 25 "COM123+1.signature.gperf"
      {"true",8},
#line 30 "COM123+1.signature.gperf"
      {"veabs",13},
#line 31 "COM123+1.signature.gperf"
      {"vempty",14},
#line 38 "COM123+1.signature.gperf"
      {"vreduce",21},
      {"",0},
#line 40 "COM123+1.signature.gperf"
      {"vsomeType",23},
#line 34 "COM123+1.signature.gperf"
      {"visFreeVar",17},
#line 41 "COM123+1.signature.gperf"
      {"vsubst",24},
#line 37 "COM123+1.signature.gperf"
      {"vnoType",20},
#line 39 "COM123+1.signature.gperf"
      {"vsomeExp",22},
#line 26 "COM123+1.signature.gperf"
      {"vabs",9},
#line 29 "COM123+1.signature.gperf"
      {"vbind",12},
#line 36 "COM123+1.signature.gperf"
      {"vnoExp",19},
#line 32 "COM123+1.signature.gperf"
      {"vgensym",15},
      {"",0},
#line 27 "COM123+1.signature.gperf"
      {"vapp",10},
#line 19 "COM123+1.signature.gperf"
      {"!",2},
#line 28 "COM123+1.signature.gperf"
      {"varrow",11},
#line 35 "COM123+1.signature.gperf"
      {"vlookup",18},
      {"",0},
#line 43 "COM123+1.signature.gperf"
      {"vvar",26},
      {"",0},
#line 33 "COM123+1.signature.gperf"
      {"vgetSomeExp",16},
#line 42 "COM123+1.signature.gperf"
      {"vtcheck",25},
      {"",0},
#line 21 "COM123+1.signature.gperf"
      {"&",4},
      {"",0},
#line 22 "COM123+1.signature.gperf"
      {":",5},
#line 23 "COM123+1.signature.gperf"
      {"=>",6},
      {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      {"",0}, {"",0},
#line 20 "COM123+1.signature.gperf"
      {"!=",3}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
