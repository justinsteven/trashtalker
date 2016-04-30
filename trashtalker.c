/*
   Copyright 2016 @justinsteven

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <openssl/sha.h>
#include "trashtalker.h"

#define GUESSSIZE 64          // keeps stack buffers nicely aligned
#define CHALSIZE 16
#define CHALRESPONSESIZE 66   // 64B plus '\n\0'

#ifndef FLAG
#define FLAG "could_have_been_a_flag{you should define a flag!}"
#endif // FLAG

int main(int argc, char **argv) {
  const char *flag = FLAG;

  #ifdef POW
  // POW challenge discourages brute-forcing
  // could also be achieved with iptables rate limiting?
  if (argc != 2 || atoi(argv[1]) < 0 || atoi(argv[1]) > SHA256_DIGEST_LENGTH*2) {
    printf("Usage: %s <pow-difficulty>\n", argv[0]);
    puts("  pow-difficulty of 0 virtually disables the POW challenge");
    printf("  pow-difficulty must be >=0 and <=%d\n", SHA256_DIGEST_LENGTH*2);
    return -1;
  }
  #endif // POW

  // set STDOUT to unbuffered
  setbuf(stdout, NULL);

  // prep random number generator for POW challenge generation and dot-sleeps
  // seed = time XOR pid
  // an insecure hack to frustrate parallel connections in the same second
  // if someone wants to find instances of time XOR pid that match, I'll allow it
  srand(time(NULL) ^ getpid());

  #ifdef POW
  // do POW challenge
  if (proofofwork(atoi(argv[1]))) {
    // user failed the challenge
    return -1;
  }
  #endif // POW

  // kick off the chain that ends with asking for the user's guess
  char *guess = introduceGame();

  // on return we leak heap memory for the guess, but we're basically done
  if (!strcmp(flag,guess)) {
    puts("Yeah that's it.");
    return 0;
  }
  else {
    // filthy hack to try to mitigate timing attack
    usleep(rand() % 1500000 + 100000);
    puts("Nah that's not it.");
    return -1;
  }
}

#ifdef POW
int proofofwork(int difficulty) {
  const char *chars = "0123456789ABCDEF";
  const char *zeroes = "0000000000000000000000000000000000000000000000000000000000000000";

  // generate challenge
  char challenge[CHALSIZE+CHALRESPONSESIZE];
  for (int i=0; i<CHALSIZE; i++) {
    challenge[i] = chars[rand() % strlen(chars)];
  }
  challenge[CHALSIZE] = 0;

  // send challenge
  printf("%d:%s\n", difficulty, challenge);

  // get response, tack it on to the end of challenge
  fgets(challenge+CHALSIZE, CHALRESPONSESIZE, stdin);
  char *newlinepos;
  if ((newlinepos=strchr(challenge, '\n')) != NULL) {
    *newlinepos = '\0';
  }

  // hash it
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char*)challenge, strlen(challenge), hash);

  // hexdump the hash
  char hashasc[SHA256_DIGEST_LENGTH*2+1];
  for (int i=0; i < SHA256_DIGEST_LENGTH; i++) {
    snprintf(hashasc+(i*2), sizeof(hashasc), "%02x", hash[i]);
  }
  hashasc[SHA256_DIGEST_LENGTH*2+1] = 0;

  // check first <difficulty> chars of hashasc against zeroes
  if (strncmp(hashasc, zeroes, difficulty)) {
    // we failed the check
    puts("NO");
    return -1;
  }

  // we passed the check!
  puts("OK");
  return 0;
}
#endif // POW

char* introduceGame() {
  // determines how "deep" our stack frames will go
  int depth = rand() % 99 + 20;

  puts("Loading game");
  return printDots(depth);
}

char* printDots(int depth) {
  const char *dot = ".";

  // print a dot
  // the local var "*dot" is a bit of stack frame padding, makes things more interesting
  printf("%s", dot);

  // wait a bit
  usleep(rand() % 150000 + 10000);

  if (depth > 0) {
    // PRINT MOAR DOTS
    return printDots(depth-1);
  } else {
    // ok that's enough dots
    printf("\n");
    return playGame();
  }
}

char* playGame() {
  puts("Bet you don't know the flag.");

  char guess[GUESSSIZE];
  fgets(guess, GUESSSIZE, stdin);

  // null-terminate at the newline
  char *newlinepos;
  if ((newlinepos=strchr(guess, '\n')) != NULL) {
    *newlinepos = '\0';
  }

  // check for "%n" (to protect against "%n") and "$n" (to protect against "%<digits>$n")
  // update: @mongobug pointed out I should have also checked for "%hn", "%hhn", "$hn" and "$hhn").
  // And so we're still vulnerable to unintentional memory corruption. Derp.
  if (strstr(guess, "%n") || strstr(guess, "$n")) {
    puts("Not that sort of challenge ;)");
    exit(-1);
  }

  // here be the bug
  printf(guess);
  puts("?");

  // pass a copy of the guess back up the chain
  return strdup(guess);
}
