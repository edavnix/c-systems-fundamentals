/*
 ===============================================================================
 Exercise: 08_text_statistics.c
 Description: Text analysis tool for statistical reporting
 Platform: GNU/Linux (Arch/WSL) on x86_64
 ===============================================================================
 Features:
 - Character count (total, no spaces, punctuation)
 - Word, sentence, and line counting
 - Vowel frequency analysis
 - Pangram detection (using all alphabet letters)
 - Longest/Shortest word finding
 ===============================================================================
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_BUFFER 2048
#define MAX_WORD_LENGTH 50
#define ALPHABET_SIZE 26
#define VOWEL_COUNT 5

typedef enum { SUCCESS, ERR_BUFFER_OVERFLOW } Status;

typedef struct {
  int total_chars;
  int chars_no_space;
  int words;
  int sentences;
  int lines;
  int letters;
  int spaces;
  int punctuation;
} CharStats;

typedef struct {
  int vowel_counts[VOWEL_COUNT];
  int alpha_present[ALPHABET_SIZE];
  int unique_letters;
} AlphabetStats;

typedef struct {
  char longest[MAX_WORD_LENGTH];
  char shortest[MAX_WORD_LENGTH];
  double average_length;
} WordStats;

typedef struct {
  Status status;
  CharStats chars;
  AlphabetStats alphabet;
  WordStats words;
} TextAnalysisResult;

void show_analysis_results(const TextAnalysisResult *result);
void run_text_analysis(void);

Status read_text_input(char *buffer, int max_size);

TextAnalysisResult analyze_text_logic(const char *text);
int is_vowel(char c);
int get_vowel_index(char c);

int main(void) {
  printf("=== Text Analyzer ===\n\n");
  printf("Enter text (type END on a new line to finish):\n\n");

  run_text_analysis();

  return 0;
}

void show_analysis_results(const TextAnalysisResult *result) {
  printf("\n----- Text Statistics -----\n\n");

  printf("Character Counts:\n");
  printf("  - Total characters: %d\n", result->chars.total_chars);
  printf("  - Characters (no spaces): %d\n", result->chars.chars_no_space);
  printf("  - Words: %d\n", result->chars.words);
  printf("  - Sentences: %d\n", result->chars.sentences);
  printf("  - Lines: %d\n\n", result->chars.lines);

  if (result->chars.words > 0) {
    printf("Word Analysis:\n");
    printf("  - Average word length: %.2f characters\n",
           result->words.average_length);
    printf("  - Longest word: \"%s\" (%zu characters)\n", result->words.longest,
           strlen(result->words.longest));
    printf("  - Shortest word: \"%s\" (%zu characters)\n\n",
           result->words.shortest, strlen(result->words.shortest));
  }

  printf("Character Distribution:\n");
  printf("  - Letters: %d (%.2f%%)\n", result->chars.letters,
         (result->chars.letters * 100.0) / result->chars.total_chars);
  printf("  - Spaces: %d (%.2f%%)\n", result->chars.spaces,
         (result->chars.spaces * 100.0) / result->chars.total_chars);
  printf("  - Punctuation: %d (%.2f%%)\n\n", result->chars.punctuation,
         (result->chars.punctuation * 100.0) / result->chars.total_chars);

  printf("Vowel Frequency:\n  ");
  const char vowels[] = "aeiou";
  for (int i = 0; i < VOWEL_COUNT; i++) {
    printf("%c: %d", vowels[i], result->alphabet.vowel_counts[i]);
    if (i < VOWEL_COUNT - 1) {
      printf(", ");
    }
  }

  printf("\n\nPangram Detection:\n");
  printf("  - Unique letters used: %d/%d\n", result->alphabet.unique_letters,
         ALPHABET_SIZE);
  printf("  - Is pangram? ");
  if (result->alphabet.unique_letters == ALPHABET_SIZE) {
    printf("YES\n");
    printf("  - (Contains all 26 letters of the alphabet)\n");
  } else {
    printf("NO\n");
    printf("  - (Missing %d letters)\n",
           ALPHABET_SIZE - result->alphabet.unique_letters);
  }
}

void run_text_analysis(void) {
  char buffer[MAX_BUFFER] = "";

  Status status = read_text_input(buffer, MAX_BUFFER);
  if (status != SUCCESS) {
    printf("Error: Failed to read input.\n");
    return;
  }

  TextAnalysisResult result = analyze_text_logic(buffer);
  if (result.status == SUCCESS) {
    show_analysis_results(&result);
  }
}

Status read_text_input(char *buffer, int max_size) {
  char line[256];
  int current_length = 0;

  while (TRUE) {
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }

    if (strncmp(line, "END", 3) == 0 &&
        (line[3] == '\n' || line[3] == '\r' || line[3] == '\0')) {
      break;
    }

    int line_len = strlen(line);
    if (current_length + line_len >= max_size - 1) {
      return ERR_BUFFER_OVERFLOW;
    }

    strcat(buffer, line);
    current_length += line_len;
  }

  return SUCCESS;
}

TextAnalysisResult analyze_text_logic(const char *text) {
  TextAnalysisResult result = {SUCCESS, {0}, {{0}, {0}, 0}, {"", "", 0.0}};

  result.chars.total_chars = strlen(text);

  char current_word[MAX_WORD_LENGTH];
  size_t word_idx = 0;
  double total_word_len = 0;

  for (int i = 0; i <= result.chars.total_chars; i++) {
    char c = text[i];

    if (c == '\n') {
      result.chars.lines++;
    }

    if (c == '.' || c == '!' || c == '?') {
      result.chars.sentences++;
    }

    if (isalpha((unsigned char)c)) {
      result.chars.letters++;
      result.chars.chars_no_space++;
      result.alphabet.alpha_present[tolower(c) - 'a'] = TRUE;

      if (is_vowel(c)) {
        int idx = get_vowel_index(c);
        if (idx >= 0) {
          result.alphabet.vowel_counts[idx]++;
        }
      }

      if (word_idx < MAX_WORD_LENGTH - 1) {
        current_word[word_idx++] = c;
      }
    } else {
      if (isspace((unsigned char)c)) {
        if (c != '\n' && c != '\r') {
          result.chars.spaces++;
        }
      } else if (ispunct((unsigned char)c)) {
        result.chars.punctuation++;
        result.chars.chars_no_space++;
      }

      if (word_idx > 0) {
        current_word[word_idx] = '\0';
        result.chars.words++;
        total_word_len += word_idx;

        if (strlen(result.words.longest) == 0 ||
            word_idx > strlen(result.words.longest)) {
          strcpy(result.words.longest, current_word);
        }

        if (strlen(result.words.shortest) == 0 ||
            word_idx < strlen(result.words.shortest)) {
          strcpy(result.words.shortest, current_word);
        }

        word_idx = 0;
      }
    }
  }

  if (result.chars.words > 0) {
    result.words.average_length = total_word_len / result.chars.words;
  }

  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (result.alphabet.alpha_present[i]) {
      result.alphabet.unique_letters++;
    }
  }

  return result;
}

int is_vowel(char c) {
  c = tolower((unsigned char)c);
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

int get_vowel_index(char c) {
  switch (tolower((unsigned char)c)) {
  case 'a':
    return 0;
  case 'e':
    return 1;
  case 'i':
    return 2;
  case 'o':
    return 3;
  case 'u':
    return 4;
  default:
    return -1;
  }
}
