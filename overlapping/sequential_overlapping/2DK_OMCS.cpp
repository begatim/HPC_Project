#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include <ctime>
#include "2DK_OMCS.hpp"

using namespace std;

std::vector<double> doublewoDKmax(vector<double> M, int K,int rows, int col){

  std::cout << "Beginning" << std::endl;
  // Set of all intervals (s,t) in increasing order
  vector<int> sortedPairs = findSortedCombinations(rows);
  // Last input == 1 for left, ==0 for right
  std::cout << "Find the convex shapes" << std::endl;

  std::clock_t start;
  double duration;
  vector<double> F_w = findConvex(M,sortedPairs,K,1,rows,col);
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  printf("---------------------------------------------------------\n");
  std::cout<<"Creation of left side of the convex shapes run in: "<< duration <<'\n';


  start = 0.0;
  vector<double> F_n = findConvex(M,sortedPairs,K,0,rows,col);
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  printf("---------------------------------------------------------\n");
  std::cout<<"Creation of right side of the convex shapes run in: "<< duration <<'\n';

  // std::cout<<"hop "<<endl;
  // for (std::vector<double>::iterator it=F_n.begin(); it!=F_n.end(); ++it)
  //   std::cout << ' ' << *it;
  // std::cout << '\n';

  std::cout << "Finalize" << std::endl;
  // Finilization: adding left and right convex shapes
  start = 0.0;
  vector<double> F_wn(col*rows*rows*K,-std::numeric_limits<double>::infinity());
  for (int k=1;k<col;k++){
      for(int s=0;s<rows;s++){
          for(int t=0;t<rows;t++){
              // set a runge of values to a
              int start = k*rows*rows*K + s*rows*K + t*K;
              int end = k*rows*rows*K+s*rows*K + t*K + K;

              vector<double> sub1(&F_w[start],&F_w[end]);
              vector<double> sub2(&F_n[start],&F_n[end]);
              vector<double> temp = add(sub1,sub2);

              int tempint = summarize(M,k,s,t,rows);
              addInt(temp,summarize(M,k,s,t,rows));

              for(int j = 0;j<K;j++){
                  F_wn[k*rows*rows*K + s*rows*K + t*K + j] = temp[j];

              }

          }
      }
  }
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  printf("---------------------------------------------------------\n");
  std::cout<<"Creation of finilized shapes run in: "<< duration <<'\n';

  std::cout << "Sort the vector" << std::endl;

  // for (std::vector<double>::iterator it=F_wn.begin(); it!=F_wn.end(); ++it)
  //   std::cout << ' ' << *it;
  // std::cout << '\n';
  start = 0.0;
  std::sort(F_wn.begin(), F_wn.end());

  std::vector<double>::iterator it;
  it = std::unique(F_wn.begin(), F_wn.end());

  F_wn.resize(std::distance(F_wn.begin(),it));

  std::reverse(F_wn.begin(),F_wn.end());

  std::cout << "Extract the K maximal sums" << std::endl;

  vector<double>::const_iterator first = F_wn.begin();
  vector<double>::const_iterator last = F_wn.end();

  vector<double> K_overlap(first, last);
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  printf("---------------------------------------------------------\n");
  std::cout<<"Sorting the resulting vector and extracting the maximums run in: "<< duration <<'\n';

  std::cout << "Result" << std::endl;

  return K_overlap;

}

/* Finds all possible F_w*/
std::vector<double> findConvex(std::vector<double> M, std::vector<int> sortedPairs, int K, int type,int rows,int col){
  //int m = sizeof(M,1); //N.of. rows
  //int n = sizeof(M,2); //N.of columns
  int num_of_comb = numberOfCombinations(rows);

  std::vector<double> F_w(col*rows*rows*K,-std::numeric_limits<double>::infinity());

  // set all k=0 values to 0 for sets s<= t
  for (int i = 0; i<num_of_comb;i++){
      int s = sortedPairs[i*2];
      int t = sortedPairs[i*2+1];
      if (s<=t){
        F_w[0,s,t,0] = 0;
      }
  }

  vector<int> k_runge(K);
  if (type == 1){ //left
      int x = 0;
      std::generate(k_runge.begin(), k_runge.end(), [&]{ return x++; });
  }
  else if (type == 0){
      int x = K-1;
      std::generate(k_runge.begin(), k_runge.end(), [&]{ return x--; });
  }

  std::cout<<k_runge.size()<<std::endl;

  for(int k : k_runge){
      std::cout<<k<<std::endl;
      for(int i=0;i<num_of_comb;i++){
          int s = sortedPairs[i*2];
          int t = sortedPairs[i*2+1];

          int start = k*rows*rows*K + s*rows*K + t*K;
          int end = k*rows*rows*K + s*rows*K + t*K + K;

          int prev = (type==1) ? (k-1) : (k+1);
          // Subvectors of F_w corresponding to the three cases
          std::vector<double> L1(&F_w[prev*rows*rows*K + s*rows*K + t*K],
                                 &F_w[prev*rows*rows*K + s*rows*K + t*K + K]);
          addInt(L1,summarize(M,k,s,t,rows));
          std::vector<double> L2(&F_w[k*rows*rows*K + (s+1)*rows*K + t*K],
                                 &F_w[k*rows*rows*K + (s+1)*rows*K + t*K + K]);
          addInt(L2,M[s,k]);
          std::vector<double> L3(&F_w[k*rows*rows*K + s*rows*K + (t+1)*K],
                                 &F_w[k*rows*rows*K + s*rows*K + (t+1)*K + K]);
          addInt(L3,M[t,k]);
          vector<double> L_max = max(L1,L2,L3);
          for(int j = 0;j<K;j++){
              F_w[k*rows*rows*K + s*rows*K + t*K + j] = L_max[j];
          }
          //std::fill(F_w.begin() + start, F_w.begin() + start + end, max(L1,L2,L3));
      }
  }
  return F_w;
}


int numberOfCombinations(int m){
  int num_of_comb = m*(m+1)/2;
  return num_of_comb;
}

/**
 * Places all intervals of (s,t) in in a 2D matrix,
 * for all s<t
 * comb(i,j) = (s,t)
 **/
std::vector<int> findSortedCombinations(int m){
  int num_of_comb = numberOfCombinations(m);
  // 2D vector of size num_of_comb*2 with initial value 0
  //std::vector<std::vector<int> > comb(num_of_comb,std::vector<int>(2, 0));
  // Each set of two represents a combination of (s,t) for a given iteration (counter)
  //int comb[num_of_comb*2];
  std::vector<int> comb(num_of_comb*2);
  int counter = 0;
  for(int diff=0;diff<m-1;diff++){
      for(int t=diff;t<m;t++){
          int s = t-diff;
          comb[counter*2] = s;
          comb[counter*2+1] = t;
          counter++;
      }
  }
  return comb;
}

/**
 * Summarizes column k of matrix M,
 * from s to t.
 **/

int summarize(vector<double> M,int k,int s,int t, int rows){
  double sum;
  for(int i=s;i<=t;i++){
      sum = sum + M[i*rows + k];
  }
  return sum;
}


/*From K-tuple L_1,...,L_m max(L_1,...) returns a tuple with the K largest elements*/
vector<double> max(vector<double> L1, vector<double> L2, vector<double> L3){
  // merge matrices and perform quicksort
  std::vector<double> merge;
  merge.reserve(L1.size() + L2.size() + L3.size());
  merge.insert( merge.end(), L1.begin(), L1.end());
  merge.insert( merge.end(), L2.begin(), L2.end());
  merge.insert( merge.end(), L3.begin(), L3.end());
  std::sort(merge.begin(), merge.end());
  std::reverse(merge.begin(),merge.end());
  vector<double>::const_iterator first = merge.begin() ;
  vector<double>::const_iterator last = merge.begin() + L1.size() ;
  vector<double> max_vector(first, last);
  return max_vector;
}


void addInt(vector<double> &v, int a){
  for (int i = 0;i<v.size();i++){
      v[i] = v[i]+a;
  }
}

vector<double> add(vector<double> v1, vector<double> v2){
  vector<double> v3;
  for(int i = 0; i<v1.size();i++){
      v3.push_back(v1[i] + v2[i]);
  }
  return v3;
}
