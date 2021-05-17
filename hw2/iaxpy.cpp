  #include <cstdio>
  #include <random>

  #include <gem5/m5ops.h>

  int main()
  {
    const int N = 10000;
    int X[N], Y[N], alpha;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis(1, 100);
    alpha = dis(gen)/2;
    for (int i = 0; i < N; ++i)
    {
      X[i] = dis(gen);
      Y[i] = dis(gen);
    }

    // Start of iaxpy loop
    m5_dump_reset_stats(0, 0);
    for (int i = 0; i < N; i++)
    {
      Y[i] = X[i] / alpha + Y[i];
    }
    m5_dump_reset_stats(0, 0);
    // End of iaxpy loop

    int sum = 0;
    for (int i = 0; i < N; ++i)
    {
      sum += Y[i];
    }
    printf("%d\n", sum);
    return 0;
  }

