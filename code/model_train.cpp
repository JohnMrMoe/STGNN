
#include <vector>
#include <math.h>       /* exp */
#include <string>
#include <float.h>
#include "arguments.cpp"

int model_train(Dataset &data, Arguments &args, int blocks[2][3]) {
  int n = args.n_route, n_his = args.n_his, n_pred = args.n_pred;
  int Ks = args.ks, Kt = args.kt;
  int batch_size = args.batch_size, epoch = args.epoch;
  string inf_mode = args.inf_mode, opt = args.opt;

  


}
