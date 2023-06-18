//
// Created by iu22 on 13/06/23.
//

#ifndef ARM_PROJECT_UTILS_H
#define ARM_PROJECT_UTILS_H

extern char typeArray[33][5];
extern int labelCheck(char *word);
extern char **createMallocedStrings(int count, int size);
extern void freeMallocedStrings(char **strings, int count);


#endif //ARM_PROJECT_UTILS_H
