/* C code produced by gperf version 3.1 */
/* Command-line: gperf AGT006+1.signature.gperf  */
/* Computed positions: -k'2-5,8' */

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

#line 1 "AGT006+1.signature.gperf"

#include <string.h>
#line 12 "AGT006+1.signature.gperf"
struct Symbol {
    const char* name;
    int value;
};

#ifdef AGT
      #define TOTAL_KEYWORDS 291
      #define MIN_WORD_LENGTH 2
      #define MAX_WORD_LENGTH 44
      #define MIN_HASH_VALUE 3
      #define MAX_HASH_VALUE 904
      /* maximum key range = 902, duplicates = 0 */

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
      static const unsigned short asso_values[] =
      {
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 125,   5,
            15, 415, 250, 200, 115, 100, 278,  30,   0,  65,
            468, 433, 258,   3, 485, 340, 210, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905,  15, 905,  10,  45,   0,
            55,  10,  60,   0,   5,   5,  45,   0,   0, 905,
            0,   0,  10,  50,   5,   5,   0,   0,  10,  55,
            0,  10, 905,   0,  40,  10,  10, 905,   0, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905, 905, 905, 905, 905, 905,
            905, 905, 905, 905, 905
      };
      register unsigned int hval = len;

      switch (hval)
      {
            default:
            hval += asso_values[(unsigned char)str[7]];
            /*FALLTHROUGH*/
            case 7:
            case 6:
            case 5:
            hval += asso_values[(unsigned char)str[4]+2];
            /*FALLTHROUGH*/
            case 4:
            hval += asso_values[(unsigned char)str[3]];
            /*FALLTHROUGH*/
            case 3:
            hval += asso_values[(unsigned char)str[2]+9];
            /*FALLTHROUGH*/
            case 2:
            hval += asso_values[(unsigned char)str[1]];
            break;
      }
      return hval;
      }

      const struct Symbol *
      get_value (str, len)
      register const char *str;
      register size_t len;
      {
      static const struct Symbol wordlist[] =
      {
            {"",0}, {"",0}, {"",0},
      #line 167 "AGT006+1.signature.gperf"
            {"nn1",150},
            {"",0},
      #line 306 "AGT006+1.signature.gperf"
            {"towna",289},
      #line 251 "AGT006+1.signature.gperf"
            {"nn6",234},
      #line 39 "AGT006+1.signature.gperf"
            {"n1",22},
      #line 51 "AGT006+1.signature.gperf"
            {"n11",34},
      #line 179 "AGT006+1.signature.gperf"
            {"nn11",162},
      #line 188 "AGT006+1.signature.gperf"
            {"nn118",171},
      #line 74 "AGT006+1.signature.gperf"
            {"n16",57},
      #line 253 "AGT006+1.signature.gperf"
            {"nn61",236},
            {"",0},
      #line 53 "AGT006+1.signature.gperf"
            {"n111",36},
      #line 308 "AGT006+1.signature.gperf"
            {"townc",291},
            {"",0},
      #line 78 "AGT006+1.signature.gperf"
            {"n2",61},
      #line 80 "AGT006+1.signature.gperf"
            {"n21",63},
      #line 190 "AGT006+1.signature.gperf"
            {"nn12",173},
      #line 199 "AGT006+1.signature.gperf"
            {"nn128",182},
      #line 85 "AGT006+1.signature.gperf"
            {"n26",68},
      #line 254 "AGT006+1.signature.gperf"
            {"nn62",237},
      #line 301 "AGT006+1.signature.gperf"
            {"stjosephburgh",284},
      #line 54 "AGT006+1.signature.gperf"
            {"n112",37},
      #line 180 "AGT006+1.signature.gperf"
            {"nn110",163},
      #line 18 "AGT006+1.signature.gperf"
            {"accept_team",1},
            {"",0}, {"",0},
      #line 22 "AGT006+1.signature.gperf"
            {"christian",5},
      #line 25 "AGT006+1.signature.gperf"
            {"citya",8},
      #line 166 "AGT006+1.signature.gperf"
            {"native",149},
      #line 155 "AGT006+1.signature.gperf"
            {"n9",138},
      #line 157 "AGT006+1.signature.gperf"
            {"n91",140},
      #line 206 "AGT006+1.signature.gperf"
            {"nn19",189},
      #line 191 "AGT006+1.signature.gperf"
            {"nn120",174},
      #line 162 "AGT006+1.signature.gperf"
            {"n96",145},
      #line 261 "AGT006+1.signature.gperf"
            {"nn69",244},
      #line 40 "AGT006+1.signature.gperf"
            {"n10",23},
      #line 61 "AGT006+1.signature.gperf"
            {"n119",44},
      #line 187 "AGT006+1.signature.gperf"
            {"nn117",170},
            {"",0}, {"",0}, {"",0},
      #line 42 "AGT006+1.signature.gperf"
            {"n101",25},
      #line 34 "AGT006+1.signature.gperf"
            {"countryccivilorganization",17},
      #line 36 "AGT006+1.signature.gperf"
            {"muslim",19},
      #line 35 "AGT006+1.signature.gperf"
            {"countrycmedicalorganization",18},
      #line 79 "AGT006+1.signature.gperf"
            {"n20",62},
            {"",0},
      #line 198 "AGT006+1.signature.gperf"
            {"nn127",181},
            {"",0}, {"",0}, {"",0},
      #line 43 "AGT006+1.signature.gperf"
            {"n102",26},
      #line 28 "AGT006+1.signature.gperf"
            {"countryacivilorganization",11},
            {"",0},
      #line 31 "AGT006+1.signature.gperf"
            {"countryamedicalorganization",14},
      #line 29 "AGT006+1.signature.gperf"
            {"countryafirstaidorganization",12},
      #line 295 "AGT006+1.signature.gperf"
            {"northport",278},
      #line 307 "AGT006+1.signature.gperf"
            {"townb",290},
      #line 23 "AGT006+1.signature.gperf"
            {"christiancountrychumanitarianorganization",6},
      #line 30 "AGT006+1.signature.gperf"
            {"countryahumanitarianorganization",13},
      #line 156 "AGT006+1.signature.gperf"
            {"n90",139},
      #line 24 "AGT006+1.signature.gperf"
            {"christiansufferterrahumanitarianorganization",7},
      #line 296 "AGT006+1.signature.gperf"
            {"other",279},
            {"",0},
      #line 19 "AGT006+1.signature.gperf"
            {"atheist",2},
      #line 207 "AGT006+1.signature.gperf"
            {"nn2",190},
      #line 50 "AGT006+1.signature.gperf"
            {"n109",33},
      #line 303 "AGT006+1.signature.gperf"
            {"suffertown",286},
      #line 305 "AGT006+1.signature.gperf"
            {"sunsetpoint",288},
      #line 27 "AGT006+1.signature.gperf"
            {"coastvillage",10},
      #line 62 "AGT006+1.signature.gperf"
            {"n12",45},
      #line 209 "AGT006+1.signature.gperf"
            {"nn21",192},
      #line 189 "AGT006+1.signature.gperf"
            {"nn119",172},
      #line 21 "AGT006+1.signature.gperf"
            {"centraltown",4},
            {"",0},
      #line 37 "AGT006+1.signature.gperf"
            {"muslimcountrybhumanitarianorganization",20},
      #line 64 "AGT006+1.signature.gperf"
            {"n121",47},
      #line 20 "AGT006+1.signature.gperf"
            {"centrallakecity",3},
            {"",0}, {"",0},
      #line 81 "AGT006+1.signature.gperf"
            {"n22",64},
      #line 210 "AGT006+1.signature.gperf"
            {"nn22",193},
      #line 26 "AGT006+1.signature.gperf"
            {"cityb",9},
            {"",0}, {"",0}, {"",0},
      #line 65 "AGT006+1.signature.gperf"
            {"n122",48},
      #line 32 "AGT006+1.signature.gperf"
            {"countrybcivilorganization",15},
      #line 302 "AGT006+1.signature.gperf"
            {"sufferterragovernment",285},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 33 "AGT006+1.signature.gperf"
            {"countrybhumanitarianorganization",16},
      #line 158 "AGT006+1.signature.gperf"
            {"n92",141},
      #line 217 "AGT006+1.signature.gperf"
            {"nn29",200},
            {"",0}, {"",0},
      #line 133 "AGT006+1.signature.gperf"
            {"n7",116},
      #line 135 "AGT006+1.signature.gperf"
            {"n71",118},
      #line 204 "AGT006+1.signature.gperf"
            {"nn17",187},
            {"",0},
      #line 140 "AGT006+1.signature.gperf"
            {"n76",123},
      #line 259 "AGT006+1.signature.gperf"
            {"nn67",242},
            {"",0},
      #line 59 "AGT006+1.signature.gperf"
            {"n117",42},
      #line 185 "AGT006+1.signature.gperf"
            {"nn115",168},
            {"",0}, {"",0},
      #line 297 "AGT006+1.signature.gperf"
            {"rdn",280},
      #line 300 "AGT006+1.signature.gperf"
            {"rdnn",283},
            {"",0}, {"",0},
      #line 122 "AGT006+1.signature.gperf"
            {"n6",105},
      #line 124 "AGT006+1.signature.gperf"
            {"n61",107},
      #line 203 "AGT006+1.signature.gperf"
            {"nn16",186},
      #line 196 "AGT006+1.signature.gperf"
            {"nn125",179},
      #line 129 "AGT006+1.signature.gperf"
            {"n66",112},
      #line 258 "AGT006+1.signature.gperf"
            {"nn66",241},
      #line 304 "AGT006+1.signature.gperf"
            {"sunnysideport",287},
      #line 58 "AGT006+1.signature.gperf"
            {"n116",41},
      #line 184 "AGT006+1.signature.gperf"
            {"nn114",167},
            {"",0},
      #line 38 "AGT006+1.signature.gperf"
            {"n0",21},
            {"",0},
      #line 168 "AGT006+1.signature.gperf"
            {"nn10",151},
      #line 177 "AGT006+1.signature.gperf"
            {"nn108",160},
            {"",0},
      #line 252 "AGT006+1.signature.gperf"
            {"nn60",235},
      #line 134 "AGT006+1.signature.gperf"
            {"n70",117},
      #line 52 "AGT006+1.signature.gperf"
            {"n110",35},
      #line 195 "AGT006+1.signature.gperf"
            {"nn124",178},
            {"",0},
      #line 299 "AGT006+1.signature.gperf"
            {"rdn_pos",282},
            {"",0},
      #line 48 "AGT006+1.signature.gperf"
            {"n107",31},
            {"",0}, {"",0},
      #line 298 "AGT006+1.signature.gperf"
            {"rdn_neg",281},
            {"",0}, {"",0},
      #line 169 "AGT006+1.signature.gperf"
            {"nn100",152},
            {"",0}, {"",0},
      #line 123 "AGT006+1.signature.gperf"
            {"n60",106},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 47 "AGT006+1.signature.gperf"
            {"n106",30},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 176 "AGT006+1.signature.gperf"
            {"nn107",159},
            {"",0}, {"",0}, {"",0},
      #line 41 "AGT006+1.signature.gperf"
            {"n100",24},
            {"",0}, {"",0}, {"",0},
      #line 136 "AGT006+1.signature.gperf"
            {"n72",119},
      #line 215 "AGT006+1.signature.gperf"
            {"nn27",198},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 70 "AGT006+1.signature.gperf"
            {"n127",53},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 125 "AGT006+1.signature.gperf"
            {"n62",108},
      #line 214 "AGT006+1.signature.gperf"
            {"nn26",197},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 69 "AGT006+1.signature.gperf"
            {"n126",52},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 208 "AGT006+1.signature.gperf"
            {"nn20",191},
      #line 178 "AGT006+1.signature.gperf"
            {"nn109",161},
            {"",0}, {"",0}, {"",0},
      #line 63 "AGT006+1.signature.gperf"
            {"n120",46},
            {"",0}, {"",0},
      #line 111 "AGT006+1.signature.gperf"
            {"n5",94},
      #line 113 "AGT006+1.signature.gperf"
            {"n51",96},
      #line 202 "AGT006+1.signature.gperf"
            {"nn15",185},
            {"",0},
      #line 118 "AGT006+1.signature.gperf"
            {"n56",101},
      #line 257 "AGT006+1.signature.gperf"
            {"nn65",240},
            {"",0},
      #line 57 "AGT006+1.signature.gperf"
            {"n115",40},
      #line 183 "AGT006+1.signature.gperf"
            {"nn113",166},
            {"",0}, {"",0},
      #line 284 "AGT006+1.signature.gperf"
            {"nn9",267},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 77 "AGT006+1.signature.gperf"
            {"n19",60},
      #line 286 "AGT006+1.signature.gperf"
            {"nn91",269},
      #line 194 "AGT006+1.signature.gperf"
            {"nn123",177},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 88 "AGT006+1.signature.gperf"
            {"n29",71},
      #line 287 "AGT006+1.signature.gperf"
            {"nn92",270},
      #line 174 "AGT006+1.signature.gperf"
            {"nn105",157},
            {"",0}, {"",0},
      #line 112 "AGT006+1.signature.gperf"
            {"n50",95},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 46 "AGT006+1.signature.gperf"
            {"n105",29},
            {"",0}, {"",0}, {"",0},
      #line 165 "AGT006+1.signature.gperf"
            {"n99",148},
      #line 294 "AGT006+1.signature.gperf"
            {"nn99",277},
      #line 173 "AGT006+1.signature.gperf"
            {"nn104",156},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 100 "AGT006+1.signature.gperf"
            {"n4",83},
      #line 102 "AGT006+1.signature.gperf"
            {"n41",85},
      #line 201 "AGT006+1.signature.gperf"
            {"nn14",184},
            {"",0},
      #line 107 "AGT006+1.signature.gperf"
            {"n46",90},
      #line 256 "AGT006+1.signature.gperf"
            {"nn64",239},
            {"",0},
      #line 56 "AGT006+1.signature.gperf"
            {"n114",39},
      #line 182 "AGT006+1.signature.gperf"
            {"nn112",165},
      #line 240 "AGT006+1.signature.gperf"
            {"nn5",223},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 73 "AGT006+1.signature.gperf"
            {"n15",56},
      #line 242 "AGT006+1.signature.gperf"
            {"nn51",225},
      #line 114 "AGT006+1.signature.gperf"
            {"n52",97},
      #line 213 "AGT006+1.signature.gperf"
            {"nn25",196},
      #line 193 "AGT006+1.signature.gperf"
            {"nn122",176},
            {"",0}, {"",0}, {"",0},
      #line 68 "AGT006+1.signature.gperf"
            {"n125",51},
            {"",0},
      #line 84 "AGT006+1.signature.gperf"
            {"n25",67},
      #line 243 "AGT006+1.signature.gperf"
            {"nn52",226},
            {"",0}, {"",0},
      #line 144 "AGT006+1.signature.gperf"
            {"n8",127},
      #line 146 "AGT006+1.signature.gperf"
            {"n81",129},
      #line 205 "AGT006+1.signature.gperf"
            {"nn18",188},
      #line 101 "AGT006+1.signature.gperf"
            {"n40",84},
      #line 151 "AGT006+1.signature.gperf"
            {"n86",134},
      #line 260 "AGT006+1.signature.gperf"
            {"nn68",243},
            {"",0},
      #line 60 "AGT006+1.signature.gperf"
            {"n118",43},
      #line 186 "AGT006+1.signature.gperf"
            {"nn116",169},
      #line 45 "AGT006+1.signature.gperf"
            {"n104",28},
            {"",0},
      #line 161 "AGT006+1.signature.gperf"
            {"n95",144},
      #line 250 "AGT006+1.signature.gperf"
            {"nn59",233},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 197 "AGT006+1.signature.gperf"
            {"nn126",180},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 145 "AGT006+1.signature.gperf"
            {"n80",128},
            {"",0},
      #line 143 "AGT006+1.signature.gperf"
            {"n79",126},
      #line 292 "AGT006+1.signature.gperf"
            {"nn97",275},
            {"",0}, {"",0},
      #line 49 "AGT006+1.signature.gperf"
            {"n108",32},
      #line 103 "AGT006+1.signature.gperf"
            {"n42",86},
      #line 212 "AGT006+1.signature.gperf"
            {"nn24",195},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 67 "AGT006+1.signature.gperf"
            {"n124",50},
            {"",0}, {"",0}, {"",0},
      #line 132 "AGT006+1.signature.gperf"
            {"n69",115},
      #line 291 "AGT006+1.signature.gperf"
            {"nn96",274},
      #line 172 "AGT006+1.signature.gperf"
            {"nn103",155},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 285 "AGT006+1.signature.gperf"
            {"nn90",268},
            {"",0}, {"",0}, {"",0},
      #line 273 "AGT006+1.signature.gperf"
            {"nn8",256},
            {"",0}, {"",0},
      #line 147 "AGT006+1.signature.gperf"
            {"n82",130},
      #line 216 "AGT006+1.signature.gperf"
            {"nn28",199},
      #line 76 "AGT006+1.signature.gperf"
            {"n18",59},
      #line 275 "AGT006+1.signature.gperf"
            {"nn81",258},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 87 "AGT006+1.signature.gperf"
            {"n28",70},
      #line 276 "AGT006+1.signature.gperf"
            {"nn82",259},
            {"",0},
      #line 139 "AGT006+1.signature.gperf"
            {"n75",122},
      #line 248 "AGT006+1.signature.gperf"
            {"nn57",231},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0},
      #line 164 "AGT006+1.signature.gperf"
            {"n98",147},
      #line 283 "AGT006+1.signature.gperf"
            {"nn89",266},
            {"",0},
      #line 128 "AGT006+1.signature.gperf"
            {"n65",111},
      #line 247 "AGT006+1.signature.gperf"
            {"nn56",230},
            {"",0}, {"",0},
      #line 171 "AGT006+1.signature.gperf"
            {"nn102",154},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 241 "AGT006+1.signature.gperf"
            {"nn50",224},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 175 "AGT006+1.signature.gperf"
            {"nn106",158},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 121 "AGT006+1.signature.gperf"
            {"n59",104},
      #line 290 "AGT006+1.signature.gperf"
            {"nn95",273},
            {"",0}, {"",0},
      #line 89 "AGT006+1.signature.gperf"
            {"n3",72},
      #line 91 "AGT006+1.signature.gperf"
            {"n31",74},
      #line 200 "AGT006+1.signature.gperf"
            {"nn13",183},
            {"",0},
      #line 96 "AGT006+1.signature.gperf"
            {"n36",79},
      #line 255 "AGT006+1.signature.gperf"
            {"nn63",238},
            {"",0},
      #line 55 "AGT006+1.signature.gperf"
            {"n113",38},
      #line 181 "AGT006+1.signature.gperf"
            {"nn111",164},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0},
      #line 192 "AGT006+1.signature.gperf"
            {"nn121",175},
      #line 229 "AGT006+1.signature.gperf"
            {"nn4",212},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 72 "AGT006+1.signature.gperf"
            {"n14",55},
      #line 231 "AGT006+1.signature.gperf"
            {"nn41",214},
      #line 142 "AGT006+1.signature.gperf"
            {"n78",125},
      #line 281 "AGT006+1.signature.gperf"
            {"nn87",264},
            {"",0}, {"",0}, {"",0},
      #line 90 "AGT006+1.signature.gperf"
            {"n30",73},
            {"",0}, {"",0},
      #line 83 "AGT006+1.signature.gperf"
            {"n24",66},
      #line 232 "AGT006+1.signature.gperf"
            {"nn42",215},
            {"",0},
      #line 44 "AGT006+1.signature.gperf"
            {"n103",27},
            {"",0}, {"",0}, {"",0},
      #line 131 "AGT006+1.signature.gperf"
            {"n68",114},
      #line 280 "AGT006+1.signature.gperf"
            {"nn86",263},
            {"",0},
      #line 117 "AGT006+1.signature.gperf"
            {"n55",100},
      #line 246 "AGT006+1.signature.gperf"
            {"nn55",229},
      #line 110 "AGT006+1.signature.gperf"
            {"n49",93},
      #line 289 "AGT006+1.signature.gperf"
            {"nn94",272},
            {"",0},
      #line 160 "AGT006+1.signature.gperf"
            {"n94",143},
      #line 239 "AGT006+1.signature.gperf"
            {"nn49",222},
            {"",0},
      #line 274 "AGT006+1.signature.gperf"
            {"nn80",257},
            {"",0},
      #line 218 "AGT006+1.signature.gperf"
            {"nn3",201},
            {"",0}, {"",0}, {"",0}, {"",0},
      #line 71 "AGT006+1.signature.gperf"
            {"n13",54},
      #line 220 "AGT006+1.signature.gperf"
            {"nn31",203},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 92 "AGT006+1.signature.gperf"
            {"n32",75},
      #line 211 "AGT006+1.signature.gperf"
            {"nn23",194},
            {"",0},
      #line 82 "AGT006+1.signature.gperf"
            {"n23",65},
      #line 221 "AGT006+1.signature.gperf"
            {"nn32",204},
      #line 262 "AGT006+1.signature.gperf"
            {"nn7",245},
      #line 66 "AGT006+1.signature.gperf"
            {"n123",49},
            {"",0},
      #line 154 "AGT006+1.signature.gperf"
            {"n89",137},
      #line 293 "AGT006+1.signature.gperf"
            {"nn98",276},
      #line 75 "AGT006+1.signature.gperf"
            {"n17",58},
      #line 264 "AGT006+1.signature.gperf"
            {"nn71",247},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 159 "AGT006+1.signature.gperf"
            {"n93",142},
      #line 228 "AGT006+1.signature.gperf"
            {"nn39",211},
      #line 86 "AGT006+1.signature.gperf"
            {"n27",69},
      #line 265 "AGT006+1.signature.gperf"
            {"nn72",248},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 106 "AGT006+1.signature.gperf"
            {"n45",89},
      #line 245 "AGT006+1.signature.gperf"
            {"nn54",228},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 163 "AGT006+1.signature.gperf"
            {"n97",146},
      #line 272 "AGT006+1.signature.gperf"
            {"nn79",255},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0},
      #line 138 "AGT006+1.signature.gperf"
            {"n74",121},
      #line 237 "AGT006+1.signature.gperf"
            {"nn47",220},
            {"",0},
      #line 150 "AGT006+1.signature.gperf"
            {"n85",133},
      #line 249 "AGT006+1.signature.gperf"
            {"nn58",232},
            {"",0}, {"",0},
      #line 120 "AGT006+1.signature.gperf"
            {"n58",103},
      #line 279 "AGT006+1.signature.gperf"
            {"nn85",262},
      #line 170 "AGT006+1.signature.gperf"
            {"nn101",153},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 127 "AGT006+1.signature.gperf"
            {"n64",110},
      #line 236 "AGT006+1.signature.gperf"
            {"nn46",219},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0},
      #line 230 "AGT006+1.signature.gperf"
            {"nn40",213},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 137 "AGT006+1.signature.gperf"
            {"n73",120},
      #line 226 "AGT006+1.signature.gperf"
            {"nn37",209},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 126 "AGT006+1.signature.gperf"
            {"n63",109},
      #line 225 "AGT006+1.signature.gperf"
            {"nn36",208},
      #line 141 "AGT006+1.signature.gperf"
            {"n77",124},
      #line 270 "AGT006+1.signature.gperf"
            {"nn77",253},
            {"",0}, {"",0}, {"",0},
      #line 109 "AGT006+1.signature.gperf"
            {"n48",92},
      #line 278 "AGT006+1.signature.gperf"
            {"nn84",261},
            {"",0}, {"",0},
      #line 219 "AGT006+1.signature.gperf"
            {"nn30",202},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 130 "AGT006+1.signature.gperf"
            {"n67",113},
      #line 269 "AGT006+1.signature.gperf"
            {"nn76",252},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0},
      #line 263 "AGT006+1.signature.gperf"
            {"nn70",246},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 153 "AGT006+1.signature.gperf"
            {"n88",136},
      #line 282 "AGT006+1.signature.gperf"
            {"nn88",265},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 99 "AGT006+1.signature.gperf"
            {"n39",82},
      #line 288 "AGT006+1.signature.gperf"
            {"nn93",271},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 116 "AGT006+1.signature.gperf"
            {"n54",99},
      #line 235 "AGT006+1.signature.gperf"
            {"nn45",218},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 115 "AGT006+1.signature.gperf"
            {"n53",98},
      #line 224 "AGT006+1.signature.gperf"
            {"nn35",207},
            {"",0}, {"",0}, {"",0},
      #line 95 "AGT006+1.signature.gperf"
            {"n35",78},
      #line 244 "AGT006+1.signature.gperf"
            {"nn53",227},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 105 "AGT006+1.signature.gperf"
            {"n44",88},
      #line 234 "AGT006+1.signature.gperf"
            {"nn44",217},
      #line 119 "AGT006+1.signature.gperf"
            {"n57",102},
      #line 268 "AGT006+1.signature.gperf"
            {"nn75",251},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0},
      #line 149 "AGT006+1.signature.gperf"
            {"n84",132},
      #line 238 "AGT006+1.signature.gperf"
            {"nn48",221},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 104 "AGT006+1.signature.gperf"
            {"n43",87},
      #line 223 "AGT006+1.signature.gperf"
            {"nn34",206},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 108 "AGT006+1.signature.gperf"
            {"n47",91},
      #line 267 "AGT006+1.signature.gperf"
            {"nn74",250},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0},
      #line 148 "AGT006+1.signature.gperf"
            {"n83",131},
      #line 227 "AGT006+1.signature.gperf"
            {"nn38",210},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 98 "AGT006+1.signature.gperf"
            {"n38",81},
      #line 277 "AGT006+1.signature.gperf"
            {"nn83",260},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 152 "AGT006+1.signature.gperf"
            {"n87",135},
      #line 271 "AGT006+1.signature.gperf"
            {"nn78",254},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 94 "AGT006+1.signature.gperf"
            {"n34",77},
      #line 233 "AGT006+1.signature.gperf"
            {"nn43",216},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
      #line 93 "AGT006+1.signature.gperf"
            {"n33",76},
      #line 222 "AGT006+1.signature.gperf"
            {"nn33",205},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0}, {"",0},
            {"",0},
      #line 97 "AGT006+1.signature.gperf"
            {"n37",80},
      #line 266 "AGT006+1.signature.gperf"
            {"nn73",249}
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

#elif defined(COM)

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
#endif