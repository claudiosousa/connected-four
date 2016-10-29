typedef void (*splitStrCb)(int, char *);

void splitStr(char * str, void (*splitStrCb)(int, char *)) {
  char * pch;
  byte iFound = 0;
  pch = strchr(str, '|');

  while (pch != NULL)
  {
    *pch = 0;
    (*splitStrCb)(iFound++, str);
    str = pch + 1;
    pch = strchr(str , '|');
  }
  (*splitStrCb)(iFound, str);
}
