#ifndef TRASHTALKER_H_
#define TRASHTALKER_H_

int main(int argc, char **argv);
#ifdef POW
int proofofwork(int difficulty);
#endif // POW
char* introduceGame();
char* printDots();
char* playGame();

#endif // TRASHTALKER_H_
