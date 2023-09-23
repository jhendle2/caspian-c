#ifndef CASPIAN_UTILS_H
#define CASPIAN_UTILS_H

char* lstrip(char* s);
void  replace(char* s, const char original, const char replacement);
bool  empty(const char* s);
bool  isWhitespace(const char c);

#endif /* CASPIAN_UTILS_H */