void work() {
  int A[10][5];
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 5; ++j)
      A[i][j] = A[i][j] + 1;
}