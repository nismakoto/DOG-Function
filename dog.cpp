#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <stdio.h>
using namespace std;

// DOG関数の正負それぞれのピーク値を決めるパラメータ
#define Alfa 2.0
#define Beta 1.0
// 正負のガウシアン分布の分散
#define Sigma_e 0.1
#define Sigma_l 1.5
// DOG関数に使う画像の座標幅
#define K1 1
#define K2 1

// split関数
// string str 分割したい文字列
// string delim デリミタ
// list<string> 分割された文字列
list<string> split(string str, string delim)
{
        list<string> result;
        int cutAt;
        while((cutAt = str.find_first_of(delim)) != str.npos)
        {
                if(cutAt > 0)
                {
                        result.push_back(str.substr(0, cutAt));
                }
                str = str.substr(cutAt+1);
        }
        if(str.length() > 0)
        {
                result.push_back(str);
        }
        return result;
}

// DOG関数
double dog(int k1, int k2)
{
        return ((Alfa/sqrt(2.0*M_PI*pow(Sigma_e,2.0)))*exp(-(pow(k1,2.0)+pow(k2,2.0))/(2.0*pow(Sigma_e,2.0))))-((Beta/sqrt(2.0*M_PI*pow(Sigma_l,2.0)))*exp(-(pow(k1,2.0)+pow(k2,2.0))/(2.0*pow(Sigma_l,2.0))));
}

// 神経節細胞の出力
// int **rbc 双極細胞の出力
double rgc(int n1, int n2, int k1, int k2, int **rbc)
{
        return dog(k1, k2)*((rbc[n1-k1][n2-k2])/4);
}

// PGM画像を作る関数
// このプログラムは,画像を読み込めるが,外部で画像を作るのが手間なので,
// このプログラム内でわざと作るようにしている.
void make_rbc()
{
        ofstream ofs("image01.pgm");
        ofs << "P2" << endl;
        ofs << "50" << " " << "50" << endl;
        ofs << "255" << endl;

        for(int i=0; i < 50; i++){
                for(int j=0; j < 50; j++)
                {
                        if(j < 25) ofs << "128" << " ";
                        if(j >= 25) ofs << "64" << " ";
                }
                ofs << endl;
        }

        // ファイルを閉じる
        ofs.close();
}

int main()
{
        make_rbc();
        // ファイルを開く
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

        // PGMパーサ
        while(ifs1 && getline(ifs1, buf))
        {
                if(i >= 0 && i < 3)
                {
                        // splitを実行
                        list<string> strList = split(buf, " ");

                        // イテレータを取得
                        list<string>::iterator iter1 = strList.begin();

                        j = 0;
                        // パースする
                        // 最後まで
                        while(iter1 != strList.end())
                        {
                                if(i == 1 && j == 0)
                                {
                                        // PGM画像の横幅を取得
                                        width = atoi(string(*iter1).c_str());
                                }
                                else if(i == 1 && j == 1)
                                {
                                        // PGM画像の縦幅を取得
                                        height = atoi(string(*iter1).c_str());
                                }
                                else if(i == 2)
                                {
                                        // 1画素の最大値を取得
                                        pixel_max = atoi(string(*iter1).c_str());
                                }
                                j++;
                                // １つ後方に進む
                                ++iter1; 
                        }
                }
                i++;
        }

        // ファイルを閉じる
        ifs1.close();

        // input画像を格納する領域の動的確保
        // 行を作る
        int **input = new int*[height];
        // 列を作る
        for(i=0; i < height; i++)
        {
                input[i] = new int[width];
        }

        // output画像を格納する領域の動的確保
        // 行を作る
        double **output = new double*[height-K1*2];
        // 列を作る
        for(i=0; i < height-K1*2; i++)
        {
                output[i] = new double[width-K2*2];
        }

        // ファイルを開く
        ifstream ifs2("image01.pgm");
        i = 0;
        j = 0;
        k = 0;
        l = 0;

        // PGMパーサ
        while(ifs2 && getline(ifs2, buf))
        {
                if(i > 2)
                {
                        // splitを実行
                        list<string> strList = split(buf, " ");

                        // イテレータを取得
                        list<string>::iterator iter2 = strList.begin(); 

                        k = 0;
                        // パースする
                        // 最後まで
                        while(iter2 != strList.end())
                        {
                                if(j < height && k < width)
                                {
                                        // 1画素辺りの数値を入力用配列に格納する
                                        input[j][k] = atoi(string(*iter2).c_str());
                                }
                                k++;
                                // １つ後方に進む
                                ++iter2;
                        }
                        j++;
                }
                i++;
        }

        // ファイルを閉じる
        ifs2.close();

        // 神経節細胞の出力を計算する
        for(i=K1; i < height-K1; i++)
        {
                for(j=K2; j < width-K2; j++)
                {
                        for(k=-K1; k <= K1; k++)
                        {
                                for(l=-K2; l <= K2; l++)
                                {
                                        // 計算結果を出力用配列に格納
                                        output[i-K1][j-K2] += rgc(i, j, k, l, input);
                                }
                        }
                }
        }

        // ファイルを開く
        ofstream ofs("dog.pgm");
        // PGMファイルの各設定を出力
        ofs << "P2" << endl;
        // 出力画像は,縦横ともにK1*2,K2*2ずつ小さくなる
        // そのために横幅と縦幅を調整する
        ofs << width-K2*2 << " " << height-K2*2 << endl;
        ofs << pixel_max << endl;

        // 小さくなった画像を考慮しつつ,ファイルに出力する
        for(i=0; i < height-K1*2; i++)
        {
                for(j=0; j < width-K2*2; j++)
                {
                        ofs << (int)output[i][j];
                        // 半角スペースを入れる
                        if(j < width-K2*2){
                                ofs << " ";
                        }
                }
                ofs << endl;
        }

        // ファイルを閉じる
        ofs.close();

        // 領域の解放
        // 列を解放
        for(i=0; i < height; i++)
        {
                delete [] input[i];
        }
        // 行を解放
        delete [] input;

        // 領域の解放
        // 列を解放
        for(i=0; i < height-K1*2; i++)
        {
                delete [] output[i];
        }
        // 行を解放
        delete [] output;

        return 0;
}
