// X
#ifndef DATA_LOADER_GUARD
#define DATA_LOADER_GUARD

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../code/math_util.cpp"
#include "../code/arguments.cpp"
using namespace std;

int read_csv(string path, vector<vector<float>> &data) {
  string line;
  string delim = ",";

  string token;
  size_t pos_start, pos_end, delim_len = delim.length();

  std::ifstream csv (path);
  size_t counter = 0, index;
  if (csv.is_open())
  {
    while (getline(csv, line)) {
      pos_start = 0;
      index = 0;
      while ((pos_end = line.find (delim, pos_start)) != string::npos) {
        token = line.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        data[counter][index] = stof(token);
        index++;
      }
      data[counter][index] = stof(line.substr(pos_start));

      counter++;
    }
    csv.close();
    return 0;
  } else {
    return 1;
  }
}

int seq_gen_MONO_CHAN(int len_seq, vector<vector<float>> &data_seq, vector<vector<vector<vector<float>>>> &dst, int offset, int n_frame, int n_route, int day_slot) {
  int n_slot = day_slot - n_frame + 1;
  int sta, end, write;
  int idx_nf, idx_nr, idx_cw;
  for   (auto i = 0; i < len_seq; i++) {
    for (auto j = 0; j < n_slot; j++) {
      sta = (i + offset) * day_slot + j;
      end = sta + n_frame;
      write = i * n_slot + j;
      idx_nf = 0; idx_nr = 0;
      for (auto data_ref = sta; data_ref < end; data_ref++) {
        for (auto index = 0; index < data_seq[data_ref].size(); index++) {

          dst[write][idx_nf][idx_nr][0] = data_seq[data_ref][index];
          idx_nr++;
          if (idx_nr==n_route) {idx_nr=0; idx_nf++;}
          //if (idx_nf==n_frame) break;
        }
      }
    }
  }
}
int seq_gen(int len_seq, vector<vector<float>> &data_seq, vector<vector<vector<vector<float>>>> &dst, int offset, int n_frame, int n_route, int day_slot, int C_W=1) {
  if (C_W == 1) {return seq_gen_MONO_CHAN(len_seq, data_seq, dst, offset, n_frame, n_route, day_slot);}
  int n_slot = day_slot - n_frame + 1;
  int sta, end, write;
  int idx_nf, idx_nr, idx_cw;
  for   (auto i = 0; i < len_seq; i++) {
    for (auto j = 0; j < n_slot; j++) {
      sta = (i + offset) * day_slot + j;
      end = sta + n_frame;
      write = i * n_slot + j;
      idx_nf = 0; idx_nr = 0; idx_cw = 0;
      for (auto data_ref = sta; data_ref < end; data_ref++) {
        for (auto index = 0; index < data_seq[data_ref].size(); index++) {
          dst[write][idx_nf][idx_nr][0] = data_seq[data_ref][index];
          idx_cw++;
          if (idx_cw==C_W)     {idx_cw=0; idx_nr++;}
          if (idx_nr==n_route) {idx_nr=0; idx_nf++;}
        }
      }
    }
  }
}
int data_gen(string path, int n_route, int n_frame, int n_slot,
    int config[3],
    int inner_shape[3],
    Dataset_Proxy &data,
    int day_slot=288, int C_W=1) {

  int n_train = config[0];
  int n_val   = config[1];
  int n_test  = config[2];

  string line;
  string delim = ",";

  string token;
  size_t pos_start, pos_end, delim_len = delim.length();

  // START: PREP DATA SEQ
  vector<vector<float>> data_seq(1);
  std::ifstream csv (path);
  size_t counter = 0, index, size=1;
  if (csv.is_open()) {
    while (getline(csv, line)) {
      pos_start = 0;
      index = 0;

      if (counter==0) {// FIND SIZE for initiations
        while ((pos_end = line.find (delim, pos_start)) != string::npos) {
          pos_start = pos_end + delim_len;
          size++;
        }
        pos_start = 0;
        data_seq[0] = vector<float>(size);
      }
      while (data_seq.size() <= counter) data_seq.push_back(vector<float>(size));
      while ((pos_end = line.find (delim, pos_start)) != string::npos) {
        token = line.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        data_seq[counter][index] = stof(token);
        index++;
      }
      data_seq[counter][index] = stof(line.substr(pos_start));

      counter++;
    }
    csv.close();
  } else return 1;
  // DONE: PREP DATA SEQ

  seq_gen(n_train, data_seq, data.train, 0,               n_frame, n_route, day_slot, C_W);
  seq_gen(n_val  , data_seq, data.val  , n_train,         n_frame, n_route, day_slot, C_W);
  seq_gen(n_test , data_seq, data.test , n_train + n_val, n_frame, n_route, day_slot, C_W);

  if (C_W==1) {
    data.mean = v3p_mean(data.train);
    data.std  = v3p_std(data.train, data.mean);
  } else {
    data.mean = v4_mean(data.train);
    data.std  = v4_std(data.train, data.mean);
  }
  z_score(data.train, data.mean, data.std);
  z_score(data.val, data.mean, data.std);
  z_score(data.test, data.mean, data.std);
  return 1;
}

#endif //DATA_LOADER
