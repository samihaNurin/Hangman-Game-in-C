#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 50
// You get 6 wrong tries: head(1), body(2), left arm(3), right arm(4), left leg(5), right leg(6)
#define MAX_TRIES 6     // wrong tries allowed (0..6 stages shown; stage 6 = lose)

typedef struct {
    char word[MAX_WORD_LENGTH];
    char hint[MAX_WORD_LENGTH];
} WordWithHint;

/* Multi-line ASCII pictures for each stage (0..6) */
static void drawHangman(int wrongTries) {
    static const char* STAGES[7] = {
        /* 0: scaffold only */
        "  _________\n"
        "  |       |\n"
        "  |        \n"
        "  |        \n"
        "  |        \n"
        "  |        \n"
        "__|__      \n",

        /* 1: head */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |        \n"
        "  |        \n"
        "  |        \n"
        "__|__      \n",

        /* 2: head + body */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |       |\n"
        "  |       |\n"
        "  |        \n"
        "__|__      \n",

        /* 3: head + body + left arm */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |      /|\n"
        "  |       |\n"
        "  |        \n"
        "__|__      \n",

        /* 4: head + body + both arms */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |      /|\\\n"
        "  |       |\n"
        "  |        \n"
        "__|__      \n",

        /* 5: add left leg */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |      /|\\\n"
        "  |       |\n"
        "  |      / \n"
        "__|__      \n",

        /* 6: both legs (final/lose) */
        "  _________\n"
        "  |       |\n"
        "  |       O\n"
        "  |      /|\\\n"
        "  |       |\n"
        "  |      / \\\n"
        "__|__      \n"
    };

    if (wrongTries < 0) wrongTries = 0;
    if (wrongTries > 6) wrongTries = 6;
    printf("%s", STAGES[wrongTries]);
}

static void showState(const char guessedWord[],
                      const bool guessedLetters[26],
                      int wrongTries,
                      const char* hint) {
    printf("\nHint: %s\n", hint);

    printf("Word: ");
    for (int i = 0; guessedWord[i] != '\0'; i++) {
        printf("%c ", guessedWord[i]);
    }
    printf("\n");

    printf("Tried letters: ");
    for (int i = 0; i < 26; i++) {
        if (guessedLetters[i]) printf("%c ", 'a' + i);
    }
    printf("\n");

    drawHangman(wrongTries);
    printf("Tries left: %d\n", MAX_TRIES - wrongTries);
}

/* Safely read a single alphabetic character from a line of input. */
static char readGuess(void) {
    char buffer[128];
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return '\0';
    }
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (isalpha((unsigned char)buffer[i])) {
            return (char)tolower((unsigned char)buffer[i]);
        }
    }
    return '\0'; // no alphabetic character found
}

int main(void) {
    srand((unsigned)time(NULL));

    // Word list with hints (easy)
    WordWithHint words[] = {
        {"cat",   "A small domesticated animal"},
        {"dog",   "A loyal companion animal"},
        {"apple", "A popular fruit"},
        {"sun",   "The star at the center of our solar system"},
        {"car",   "A four-wheeled vehicle"},
        {"ball",  "A round object used in many games"},
        {"book",  "A set of written or printed pages"},
        {"house", "A building for human habitation"},
        {"fish",  "A cold-blooded aquatic animal"}
    };
    int totalWords = (int)(sizeof(words) / sizeof(words[0]));
    int chosen = rand() % totalWords;

    const char* secret = words[chosen].word;
    const char* hint   = words[chosen].hint;

    size_t n = strlen(secret);

    char guessedWord[MAX_WORD_LENGTH];
    bool guessedLetters[26] = {false};

    // Initialize the guessed word with underscores
    for (size_t i = 0; i < n; i++) guessedWord[i] = '_';
    guessedWord[n] = '\0';

    printf("=== Welcome to Hangman! ===\n");

    int wrongTries = 0;
    while (wrongTries <= MAX_TRIES) {
        showState(guessedWord, guessedLetters, wrongTries, hint);

        // Win check before asking (covers case of zero-length or already guessed)
        if (strcmp(secret, guessedWord) == 0) {
            printf("\n Congratulations! You guessed the word: %s\n", secret);
            return 0;
        }

        if (wrongTries == MAX_TRIES) break; // reached final stage

        printf("Enter a letter (A-Z): ");
        char guess = readGuess();
        if (guess == '\0') {
            printf("Please enter a valid letter A-Z.\n");
            continue;
        }

        int idx = guess - 'a';
        if (idx < 0 || idx >= 26) {
            printf("Please enter a letter A-Z.\n");
            continue;
        }

        if (guessedLetters[idx]) {
            printf("You've already guessed '%c'. Try another letter.\n", guess);
            continue;
        }
        guessedLetters[idx] = true;

        // Reveal the letter if present
        bool found = false;
        for (size_t i = 0; i < n; i++) {
            if (secret[i] == guess) {
                guessedWord[i] = guess;
                found = true;
            }
        }

        if (found) {
            printf("Good guess!\n");
        } else {
            printf("Sorry, '%c' is not in the word.\n", guess);
            wrongTries++;
        }
    }

    // Lose condition
    showState(guessedWord, guessedLetters, wrongTries, hint);
    printf("\n Out of tries! The word was: %s\n", secret);
    return 0;
}
