#ifndef MATH_UTIL_GUARD
#define MATH_UTIL_GUARD


#include <vector>
#include <math.h>       /* exp */
#include <string>
#include <float.h>

#include <Eigen/Eigenvalues>
#include <Eigen/Dense>

// TESTING
#include <type_traits>
#include <typeinfo>

#define HARDCODE_N 228

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> laplacian;

using namespace std;

float v3p_mean(vector<vector<vector<vector<float>>>> matrix) {
  float sum = 0;
  long unsigned int num = 0;
  for       (auto a = 0; a < matrix      .size(); a++) {
    for     (auto b = 0; b < matrix[a]   .size(); b++) {
      for   (auto c = 0; c < matrix[a][b].size(); c++) {
        sum += matrix[a][b][c][0];
        num++;
      }
    }
  }
  float ret = sum/num;
  return ret;

}
float v3p_std(vector<vector<vector<vector<float>>>> matrix, float mean) {
  float sum = 0;
  float pow;
  size_t num;
  for       (auto a = 0; a < matrix         .size(); a++) {
    for     (auto b = 0; b < matrix[a]      .size(); b++) {
      for   (auto c = 0; c < matrix[a][b]   .size(); c++) {
        pow = matrix[a][b][c][0] - mean;
        sum += pow*pow;
        num++;
      }
    }
  }
  sum /= num-1;
}
float v4_mean(vector<vector<vector<vector<float>>>> matrix) {
  float sum = 0;
  size_t num = 0;
  for       (auto a = 0; a < matrix         .size(); a++) {
    for     (auto b = 0; b < matrix[a]      .size(); b++) {
      for   (auto c = 0; c < matrix[a][b]   .size(); c++) {
        for (auto d = 0; d < matrix[a][b][c].size(); d++) {
          sum += matrix[a][b][c][d];
          num++;
        }
      }
    }
  }
  return sum/num;
}
float v4_std(vector<vector<vector<vector<float>>>> matrix, float mean) {
  float sum = 0;
  float pow;
  size_t num;
  for       (auto a = 0; a < matrix         .size(); a++) {
    for     (auto b = 0; b < matrix[a]      .size(); b++) {
      for   (auto c = 0; c < matrix[a][b]   .size(); c++) {
        for (auto d = 0; d < matrix[a][b][c].size(); d++) {
          pow = matrix[a][b][c][d] - mean;
          sum += pow*pow;
          num++;
        }
      }
    }
  }
  sum /= num-1;
}
int z_score(vector<vector<vector<vector<float>>>> &matrix, float mean, float std) {
  for       (auto a = 0; a < matrix         .size(); a++) {
    for     (auto b = 0; b < matrix[a]      .size(); b++) {
      for   (auto c = 0; c < matrix[a][b]   .size(); c++) {
        for (auto d = 0; d < matrix[a][b][c].size(); d++) {
          matrix[a][b][c][d] = (matrix[a][b][c][d] - mean) / std;
        }
      }
    }
  }
}

float sum_Vector(vector<float> input) {
  float sum = 0;
  for (auto i = 0; i < input.size(); i++) sum += input[i];
  return sum;
}
//int max_real(Eigen::Matrix<std::complex<float>>, rows, cols> matrix) {
//  return 0;
//}
bool non_bool(vector<vector<float>> &data) {
  for   (auto i = 0; i < data.size(); i++) {
    for (auto j = 0; j < data[i].size(); j++) {
      if (data[i][j]!=0 || data[i][j]!=1) return true;
    }
  }
  return false;
}
int weight_matrix(vector<vector<float>> &data, float sigma2=0.1, float epsilon=0.5, bool scaling=true) {
  if (!scaling) return 0;
  if (!non_bool(data)) return 0;

  size_t n = data.size();

  float mask;   // ones - identity
  float w;      // ones - identity
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      mask = (i==j?0:1);
      w = data[i][j] / 10000;     // base
      w *= w;                     // ^2
      w = (w * -1) / sigma2;      // prep for exp
      w = exp(w);                 // exp
      data[i][j] = w * (w>=epsilon) * mask;
    }
  }

  return 1;
}


int scaled_laplacian(vector<vector<float>> &dst, vector<vector<float>> &src) {
  // py: src == W -> true

  auto n = dst.size();
  float d[n];
  for (auto i = 0; i < n; i++) d[i] = sum_Vector(src[i]);

  laplacian L(src.size(), src[0].size());

  for (auto i = 0; i < n; i++) {
    for (auto j = 0; j < n; j++) {
      L(i, j) = -src[i][j];
      if (d[i] <= 0 || d[j] <= 0) continue;
      L(i, j) /= sqrt(d[i] * d[j]);
    }
  }

  Eigen::EigenSolver<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> eigen(L, false);

  Eigen::Matrix<std::complex<float>, Eigen::Dynamic, 1> e_values = eigen.eigenvalues();

  float lambda_max = e_values(0, 0).real();
  for (auto i = 0; i < e_values.cols(); i++) {
    auto curr = e_values(0, i).real();
    lambda_max = (curr > lambda_max) ? curr : lambda_max;
  }

  for (auto i = 0; i < n; i++){
    for (auto j = 0; j < n; j++) {
      dst[i][j] = 2 * L(i, j) / lambda_max;
    }
    dst[i][i] -= 1;
  }

  return 1;
}

int cheb_poly_approx_2D(vector<vector<float>> L, int ks, int n, vector<vector<float>> &Lk) {
  float L0[HARDCODE_N][HARDCODE_N];
  float L1[HARDCODE_N][HARDCODE_N];
  float LN[HARDCODE_N][HARDCODE_N];

  for   (auto i = 0; i < n; i++)
    for (auto j = 0; j < n; j++) {
      L0[i][j] = i==j;
      L1[i][j] = L[i][j];
    }
  // Handle special cases of KS
  if (ks==1) {
    for   (auto i = 0; i < n; i++) {
        Lk[i] = vector<float>(i==0?HARDCODE_N*HARDCODE_N:0);
        for (auto j = 0; j < n; j++) Lk[0][i*HARDCODE_N + j] = L0[i][j];
    }
  } else if (ks<1) {
    std::cout << "You dummy, the Spatial Kernel was given an invalid size: " << ks << '\n';
    return -1;
  }
  float L_list[ks][n][n];
  for   (auto i = 0; i < n; i++)
    for (auto j = 0; j < n; j++) {
      L_list[0][i][j]  = L0[i][j];
      L_list[1][i][j]  = L1[i][j];
    }
  for (auto list_slot = 2; list_slot < ks; list_slot++) {
    for   (auto i = 0; i < n; i++) {
      for (auto j = 0; j < n; j++) {
        LN[i][j] = 2 * L[i][j] * L1[i][j] - L0[i][j];
        L_list[list_slot][i][j] = LN[i][j];
        L0[i][j] = L1[i][j];
        L1[i][j] = LN[i][j];
      }
    }
  }
  // CONCATENATE
  //      Prep return vector:
  size_t size = ks*n;
  for (auto i = 0; i < n; i++) Lk[i] = vector<float>(size);

  for (auto ks_n = 0; ks_n < ks; ks_n++) {
    for   (auto i = 0; i < n; i++) {
      for (auto j = 0; j < n; j++) {
        Lk[i][ks_n*n+j] = L_list[ks_n][i][j];
      }
    }
  }
  return 1;
}


#endif //MATH_UTIL
