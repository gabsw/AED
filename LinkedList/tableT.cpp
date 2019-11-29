#include <cstdlib>
#include <iostream>
#include <vector> 
using std::rand;
using std::cout;
using std::endl;

#include "HashTable.h"
int main(void)
{
  HashTable table;
  char* key1=(char*)"haha";
  char* key2=(char*)"kaka";
  char* key3=(char*)"wawa";
  char* key4=(char*)"xaxa";
  char* key5=(char*)"gaga";
  char* key6=(char*)"papa";
  char* key7=(char*)"baba";
  char* key8=(char*)"zaza";
  table.put(key1,1);
  table.put(key2,2);
  table.put(key3,3);
  table.put(key4,4);
  table.put(key5,5);
  table.put(key6,6);
  table.put(key7,7);
  table.put(key7,8);

  vector<vector<int> > v=table.computeDistances();
  for(int i=0;i<v.size();i++){
      cout<<"Position "<<i<<"--> ";
     for (int w = 0; w<v[i].size(); ++w)
     {
      cout<<v[i][w]<<" " ;
     }
      cout<<endl;
  }
    
  cout<<"***************"<<endl;
  table.printTable();
  cout<<"***************"<<endl;
  return 0;
}

