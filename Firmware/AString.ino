
const int MAX_SPLIT_PARTS = 7;
char * splitted[MAX_SPLIT_PARTS];

int split_to_array(char * str) {
  char * pch;
  byte iFound = 0;
  pch = strchr(str, '|');
  while (pch != NULL)
  {
    splitted[iFound++] = str;
    *pch = 0;
    str = pch + 1;
    pch = strchr(str , '|');
  }
  splitted[iFound++] = str;
  return iFound;
}
