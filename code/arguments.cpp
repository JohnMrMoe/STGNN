#ifndef ARGUMENTS_GUARD
#define ARGUMENTS_GUARD

#include <string.h>
using namespace std;

class Arguments {
public:
  int n_route   = 228,
      n_his     = 12,
      n_pred    = 9,
      batch_size= 50,
      epoch     = 50,
      save      = 10,
      ks        = 3,
      kt        = 3,
      c_w       = 1,
      d_slot    = 288;    // DAY SLOT
  float lr      = 1e-3;
  string opt    = "RMSProp",
      graph     = "default",
      inf_mode  = "merge";
  void set(string s, string v) {
    if (0==s.compare("--n_route")   )  n_route     = stoi(v);
    if (0==s.compare("--n_his")     )  n_his       = stoi(v);
    if (0==s.compare("--n_pred")    )  n_pred      = stoi(v);
    if (0==s.compare("--batch_size"))  batch_size  = stoi(v);
    if (0==s.compare("--epoch")     )  epoch       = stoi(v);
    if (0==s.compare("--save")      )  save        = stoi(v);
    if (0==s.compare("--ks")        )  ks          = stoi(v);
    if (0==s.compare("--kt")        )  kt          = stoi(v);
    if (0==s.compare("--c_w")       )  c_w         = stoi(v);
    if (0==s.compare("--lr")        )  lr          = stof(v);
    if (0==s.compare("--opt")       )  opt         = v;
    if (0==s.compare("--graph")     )  graph       = v;
    if (0==s.compare("--inf_mode")  )  inf_mode    = v;
  }
  Arguments(int argc, char const *argv[]) {
    for (int i = 1; i < argc; i+=2) {
      set(argv[i], argv[i+1]);
      printf("Reading argument: %s %s\n", argv[i], argv[i+1]);
    }
  }
};

class Dataset_Proxy {
public:

  vector<vector<vector<vector<float>>>> train;
  vector<vector<vector<vector<float>>>> val;
  vector<vector<vector<vector<float>>>> test;

  float mean, std;
  int initialize_quad_vec(vector<vector<vector<vector<float>>>> &vec, int shape[3]) {
    for (auto v = 0; v < vec.size(); v++) {
      vec[v] = vector<vector<vector<float>>>(shape[0]);
      for (auto i = 0; i < shape[0]; i++) {
        vec[v][i] =   vector<vector<float>> (shape[1]);
        for (auto j = 0; j < shape[1]; j++) {
          vec[v][i][j] =       vector<float>  (shape[2]);
        }
      }
    }
  }
  Dataset_Proxy(size_t n_train, size_t n_val, size_t n_test, int shape[3]) {
    train = vector<vector<vector<vector<float>>>>(n_train); initialize_quad_vec(train, shape);
    val   = vector<vector<vector<vector<float>>>>(n_val);   initialize_quad_vec(val,   shape);
    test  = vector<vector<vector<vector<float>>>>(n_test);  initialize_quad_vec(test,  shape);
  }
};

#endif
