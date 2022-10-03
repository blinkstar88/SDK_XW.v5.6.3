/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf  */
/* Computed positions: -k'1,3' */

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
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

struct kconf_id;
/* maximum key range = 40, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
kconf_id_hash (register const char *str, register unsigned int len)
{
  static unsigned char asso_values[] =
    {
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 35,  0, 20,
       5,  0,  5, 42,  0, 25, 42, 42,  5,  5,
      10,  0, 25, 15,  0,  0,  0, 10, 42, 42,
       0, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
      42, 42, 42, 42, 42, 42
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct kconf_id_strings_t
  {
    char kconf_id_strings_str2[sizeof("on")];
    char kconf_id_strings_str3[sizeof("hex")];
    char kconf_id_strings_str4[sizeof("bool")];
    char kconf_id_strings_str5[sizeof("reset")];
    char kconf_id_strings_str6[sizeof("string")];
    char kconf_id_strings_str7[sizeof("boolean")];
    char kconf_id_strings_str8[sizeof("optional")];
    char kconf_id_strings_str9[sizeof("help")];
    char kconf_id_strings_str10[sizeof("endif")];
    char kconf_id_strings_str11[sizeof("select")];
    char kconf_id_strings_str12[sizeof("endmenu")];
    char kconf_id_strings_str13[sizeof("deselect")];
    char kconf_id_strings_str14[sizeof("endchoice")];
    char kconf_id_strings_str15[sizeof("range")];
    char kconf_id_strings_str16[sizeof("source")];
    char kconf_id_strings_str17[sizeof("default")];
    char kconf_id_strings_str18[sizeof("def_bool")];
    char kconf_id_strings_str19[sizeof("menu")];
    char kconf_id_strings_str21[sizeof("def_boolean")];
    char kconf_id_strings_str22[sizeof("def_tristate")];
    char kconf_id_strings_str23[sizeof("requires")];
    char kconf_id_strings_str25[sizeof("menuconfig")];
    char kconf_id_strings_str26[sizeof("choice")];
    char kconf_id_strings_str27[sizeof("if")];
    char kconf_id_strings_str28[sizeof("int")];
    char kconf_id_strings_str31[sizeof("prompt")];
    char kconf_id_strings_str32[sizeof("comment")];
    char kconf_id_strings_str33[sizeof("tristate")];
    char kconf_id_strings_str36[sizeof("config")];
    char kconf_id_strings_str37[sizeof("depends")];
    char kconf_id_strings_str38[sizeof("mainmenu")];
    char kconf_id_strings_str41[sizeof("enable")];
  };
static struct kconf_id_strings_t kconf_id_strings_contents =
  {
    "on",
    "hex",
    "bool",
    "reset",
    "string",
    "boolean",
    "optional",
    "help",
    "endif",
    "select",
    "endmenu",
    "deselect",
    "endchoice",
    "range",
    "source",
    "default",
    "def_bool",
    "menu",
    "def_boolean",
    "def_tristate",
    "requires",
    "menuconfig",
    "choice",
    "if",
    "int",
    "prompt",
    "comment",
    "tristate",
    "config",
    "depends",
    "mainmenu",
    "enable"
  };
#define kconf_id_strings ((const char *) &kconf_id_strings_contents)
struct kconf_id *
kconf_id_lookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 32,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 12,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 41
    };

  static struct kconf_id wordlist[] =
    {
      {-1}, {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str2,		T_ON,		TF_PARAM},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str3,		T_TYPE,		TF_COMMAND, S_HEX},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str4,		T_TYPE,		TF_COMMAND, S_BOOLEAN},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str5,		T_RESET,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str6,		T_TYPE,		TF_COMMAND, S_STRING},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str7,	T_TYPE,		TF_COMMAND, S_BOOLEAN},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str8,	T_OPTIONAL,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str9,		T_HELP,		TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str10,		T_ENDIF,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str11,		T_SELECT,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str12,	T_ENDMENU,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str13,	T_DESELECT,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str14,	T_ENDCHOICE,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str15,		T_RANGE,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str16,		T_SOURCE,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str17,	T_DEFAULT,	TF_COMMAND, S_UNKNOWN},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str18,	T_DEFAULT,	TF_COMMAND, S_BOOLEAN},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str19,		T_MENU,		TF_COMMAND},
      {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str21,	T_DEFAULT,	TF_COMMAND, S_BOOLEAN},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str22,	T_DEFAULT,	TF_COMMAND, S_TRISTATE},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str23,	T_REQUIRES,	TF_COMMAND},
      {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str25,	T_MENUCONFIG,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str26,		T_CHOICE,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str27,		T_IF,		TF_COMMAND|TF_PARAM},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str28,		T_TYPE,		TF_COMMAND, S_INT},
      {-1}, {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str31,		T_PROMPT,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str32,	T_COMMENT,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str33,	T_TYPE,		TF_COMMAND, S_TRISTATE},
      {-1}, {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str36,		T_CONFIG,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str37,	T_DEPENDS,	TF_COMMAND},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str38,	T_MAINMENU,	TF_COMMAND},
      {-1}, {-1},
      {(int)(long)&((struct kconf_id_strings_t *)0)->kconf_id_strings_str41,		T_SELECT,	TF_COMMAND}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = kconf_id_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + kconf_id_strings;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist[key];
            }
        }
    }
  return 0;
}

