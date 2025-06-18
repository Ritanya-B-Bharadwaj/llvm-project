int whitelisted() {
  int *p = 0;
  return *p;
}

int not_whitelisted() {
  int *p = 0;
  return *p;
}

int main() {
  whitelisted();
  not_whitelisted();
  return 0;
}
