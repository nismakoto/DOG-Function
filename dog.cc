#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <fstream>
using namespace std;

// Peak value
#define Alpha 2.0
#define Beta 1.0
// Variance
#define Sigma_e 0.1
#define Sigma_l 1.5
// Coordinate width
#define K1 1
#define K2 1

list<string> split(string str, string delim) {
  list<string> result;
  int cut_at;
  while ((cut_at = str.find_first_of(delim)) != str.npos) {
    if (cut_at > 0) {
      result.push_back(str.substr(0, cut_at));
    }
    str = str.substr(cut_at + 1);
  }
  if (str.length() > 0) {
    result.push_back(str);
  }
  return result;
}

// DOG function
double dog(int k1, int k2) {
  return ((Alpha / sqrt(2.0 * M_PI * pow(Sigma_e, 2.0))) *
          exp(-(pow(k1, 2.0) + pow(k2, 2.0)) / (2.0 * pow(Sigma_e, 2.0)))) -
         ((Beta / sqrt(2.0 * M_PI * pow(Sigma_l, 2.0))) *
          exp(-(pow(k1, 2.0) + pow(k2, 2.0)) / (2.0 * pow(Sigma_l, 2.0))));
}

// Ganglion cell
// rbc:Bipolar cell
double rgc(int n1, int n2, int k1, int k2, int **rbc) {
  return dog(k1, k2) * ((rbc[n1 - k1][n2 - k2]) / 4);
}

// Make PGM image
void make_rbc() {
  ofstream ofs("image01.pgm");
  ofs << "P2" << endl;
  ofs << "50"
      << " "
      << "50" << endl;
  ofs << "255" << endl;

  for (int i = 0; i < 50; ++i) {
    for (int j = 0; j < 50; ++j) {
      if (j < 25)
        ofs << "128"
            << " ";
      if (j >= 25)
        ofs << "64"
            << " ";
    }
    ofs << endl;
  }

  ofs.close();
}

int main() {
  make_rbc();
  ifstream ifs1("image01.pgm");
  string buf;
  int width = 0;
  int height = 0;
  int pixel_max = 0;
  int k1, k2, n1, n2;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;

  while (ifs1 && getline(ifs1, buf)) {
    if (i >= 0 && i < 3) {
      // Split
      list<string> strList = split(buf, " ");
      // Iterator
      list<string>::iterator iter1 = strList.begin();

      j = 0;
      while (iter1 != strList.end()) {
        if (i == 1 && j == 0) {
          // PGM width
          width = atoi(string(*iter1).c_str());
        } else if (i == 1 && j == 1) {
          // PGM height
          height = atoi(string(*iter1).c_str());
        } else if (i == 2) {
          // Max value
          pixel_max = atoi(string(*iter1).c_str());
        }
        ++j;
        ++iter1;
      }
    }
    ++i;
  }

  ifs1.close();

  int **input = new int *[height];
  for (i = 0; i < height; ++i) {
    input[i] = new int[width];
  }

  double **output = new double *[height - K1 * 2];
  for (i = 0; i < height - K1 * 2; i++) {
    output[i] = new double[width - K2 * 2];
  }

  ifstream ifs2("image01.pgm");
  i = 0;
  j = 0;
  k = 0;
  l = 0;

  while (ifs2 && getline(ifs2, buf)) {
    if (i > 2) {
      // Split
      list<string> strList = split(buf, " ");
      // Iterator
      list<string>::iterator iter2 = strList.begin();

      k = 0;
      while (iter2 != strList.end()) {
        if (j < height && k < width) {
          input[j][k] = atoi(string(*iter2).c_str());
        }
        ++k;
        // １つ後方に進む
        ++iter2;
      }
      ++j;
    }
    ++i;
  }

  ifs2.close();

  // Calculation of the ganglion cell output.
  for (i = K1; i < height - K1; ++i) {
    for (j = K2; j < width - K2; ++j) {
      for (k = -K1; k <= K1; ++k) {
        for (l = -K2; l <= K2; ++l) {
          output[i - K1][j - K2] += rgc(i, j, k, l, input);
        }
      }
    }
  }

  ofstream ofs("dog.pgm");
  ofs << "P2" << endl;
  ofs << width - K2 * 2 << " " << height - K2 * 2 << endl;
  ofs << pixel_max << endl;

  for (i = 0; i < height - K1 * 2; i++) {
    for (j = 0; j < width - K2 * 2; j++) {
      ofs << (int)output[i][j];
      if (j < width - K2 * 2) {
        ofs << " ";
      }
    }
    ofs << endl;
  }

  ofs.close();

  for (i = 0; i < height; i++) {
    delete[] input[i];
  }
  delete[] input;

  for (i = 0; i < height - K1 * 2; i++) {
    delete[] output[i];
  }
  delete[] output;

  return 0;
}
