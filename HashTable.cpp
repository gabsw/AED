#ifndef AED
#define AED
#include <iostream>
#include <string>
#include <string.h>
#include <vector> 
#include <fstream>
#include <algorithm>
#include <utility>
#define INT_MIN -2147483648
#define INT_MAX 2147483647
using namespace std;


class Node{
			public:
				string key;
				int first_position;
				int last_position;
				int counter;
				vector<int> all_positions;
				Node* next;
				Node* previous;
				Node* tail;
			public:
				string getKey(){ return key;}
				int getCounter(){return counter;}
				vector<int> getAllPositions(){
					return all_positions;
				}

				int getFirstPosition(){
					return first_position;
				}

				int getLastPosition(){
					return last_position;
				}
		};
		
class Stats
{
	public:
		Stats(Node* node) {
			fillInfo(node);
		}

		~Stats(void){};
	public:
		int first_position=0;
		int last_position=0;
		vector<int> positions;
		int maxDistance=0;
		int minDistance=0;
		double avgDistance=0;
		string key="";
		int counter=0;

	private:
		void fillInfo(Node* node){
			this->positions=node->getAllPositions();
			this->first_position=node->getFirstPosition();
			this->last_position=node->getLastPosition();
			this->key=node->getKey();
			this->counter=node->getCounter();
		}

};

class HashTable{
	
	private:
		int tablesize=100;
		Node** table;
		int size=0;
		int maxtablesize=75;
		const float threshold=0.75;
		int nwords=0;
	private:
		unsigned int hash_function(const string &string,unsigned int s)
		{
			static unsigned int table[256];
			unsigned int crc,i,j;
			if(table[1] == 0u)
				for(i = 0u;i < 256u;i++)
					for(table[i] = i,j = 0u;j < 8u;j++)
						if(table[i] & 1u)
							table[i] = (table[i] >> 1) ^ 0xAED00022u;
						else
							table[i] >>= 1;
			crc = 0xAED02019u; 
			const char*str = string.c_str();
			while(*str !='\0')
				crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
		return crc % s;
		}

		void expand(){
			int newsize=tablesize*1.5;
			this->maxtablesize=newsize*threshold;
			this->size=0;

			Node** newArray=new Node*[newsize]; //tamanho da table aumenta 50%
			for(int i=0;i<tablesize;i++){
				Node* temp=table[i];
				if (temp!=nullptr) //verificar se a posicao i está vazia
				{
					Node* tail=temp->tail;
					if (tail==nullptr)	//se nao existir o Node tail, entao a lista so contem um Node
					{					//por isso esse Node é inserido imediatamente
						rePut(newArray,temp,newsize);
					}else{
						while(tail!=nullptr){ //Percorrer a lista do fim para o inicio
							//se começar no primeiro Node da lista, quando fazer put desse elemento, 
							//Vai inserir uma lista toda que começa por esse Node
							rePut(newArray,tail,newsize);
							tail=tail->previous; 
						}
					}
				}
			}
			this->tablesize=newsize;
			this->table=newArray;
			newArray=NULL;
			delete[] newArray; 
		}

		int rePut(Node** newArray,Node* node,int size){
			Node* newNode=new Node;  //uma copia de node
			newNode->key = node->key;
			newNode->first_position=node->first_position;
			newNode->last_position=node->last_position;
			newNode->counter=node->counter;
			newNode->all_positions=node->all_positions;
			newNode->next=nullptr;
			newNode->previous=nullptr;
			newNode->tail=nullptr; //o ultimo Node da lista

			int hash=hash_function(node->key,size);
			Node* last=nullptr;
			Node* temp = newArray[hash];
			Node* head= newArray[hash];

			if(temp==nullptr){ //se Node temp for null, então a posicao hash está vazia
				newArray[hash]=newNode;	//guardar Node temp nessa posicao
			}
			else{
				while(temp!=nullptr) {	//a posição hash não está vazia
					last=temp;	
					temp=temp->next;	//se temp->key!=key, avança para o proximo Node
				}	
				last->next = newNode;
				newNode->previous=last;
				head->tail=newNode; //newNode passa ser o ultimo Node da lista que começa por head,ou seja,começa por newArray[hash];
				size++;
			}
			newNode=NULL;
			delete newNode;
		}

	public:
			HashTable(void){
				this->table=new Node*[tablesize];
			}

			~HashTable(void){
				delete[] table;
			}

			void put(string key,int position) {
				Node* newNode=new Node; //criar Node com a chave e a posição
				newNode->key = key;
				newNode->first_position=position;
				newNode->last_position=position;
				newNode->all_positions.push_back(position);
				newNode->counter=1;
				newNode->next=nullptr;
				newNode->previous=nullptr;
				newNode->tail=nullptr;
				int hash=hash_function(key,tablesize);
				Node* last=nullptr;
				Node* temp = table[hash];
				Node* head= table[hash];

				int keyRepeat = -1;
				if(temp==nullptr){ //se Node temp for null, entao a posicao hash esta vazia
					table[hash]=newNode;	//guardar Node temp nessa posicao
					nwords++;
				}
				else{
					while(temp!=nullptr) {	//se nao for null, entao a posicao hash nao esta vazia
											//temp neste caso é o primeiro Node nessa posição
						if(temp->key.compare(key)==0) {	//comparar as keys
							temp->counter++;	//se for igual,counter++;
							temp->last_position=position;
							temp->all_positions.push_back(position);
							keyRepeat = 1;		//como key já existe na posição hash-->keyRepeat=1;	
							newNode=NULL;
							delete newNode;			
							break;
						}else {
							last=temp;	
							temp=temp->next;	//se temp->key!=key, avança para o proximo Node
						}
					}	
					if(keyRepeat!=1){ //se nao encontrar a key na lista,acrescentar o novo Node no fim da lista
						last->next=newNode;//o ultimo Node da lista vai ser newNode
						newNode->previous=last;
						head->tail=newNode; //o tail vai ser newNode
						size++;
						nwords++;
					}
				}
				if (size>maxtablesize)
				{
					expand();
				}

			}

			int* getPositionAndCounter(string key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(temp->key.compare(key)==0) {
							static int a[3];
							a[0]=temp->first_position;
							a[1]=temp->last_position;
							a[2]=temp->counter;
							return a;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}

			int* getPositions(string key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(temp->key.compare(key)==0) {
							static int a[2];
							a[0]=temp->first_position;
							a[1]=temp->last_position;
							return a;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}

			int getCounter(string key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(temp->key.compare(key)==0) {
							return temp->counter;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}


			int getSize(){
				return tablesize;
			}

			int getNwords(){
				return nwords;
			}
			
			vector<Stats> computeDistances(){
				vector<Stats> AllResults;
				int max;
				int min;
				double avg;
				int total;
				int flag=-1;
				for (int i = 0; i < tablesize; i++)
				{	
					Node* temp=table[i];
					while(temp!=nullptr){
						Stats s=Stats(temp);
						max=INT_MIN;
						min=INT_MAX;
						avg=0;
						total=0;
						flag=-1;
						for (int i = 0; i < s.positions.size()-1; i++)
						{
							int distance=abs(s.positions[i+1]-s.positions[i]);
							if (distance>max)
							{
								max=distance;
							}
							if (distance<min)
							{
								min=distance;
							}

							total+=distance;
							flag=1;
						}
						if (flag==1)
						{
							avg=(double)total/s.positions.size();
							s.maxDistance=max;
							s.minDistance=min;
							s.avgDistance=avg;
							AllResults.push_back(s);
						}else
						{
							s.maxDistance=0;
							s.minDistance=0;
							s.avgDistance=0;
							AllResults.push_back(s);	
						}
						temp=temp->next;
					}
					
				}
				return AllResults;
			}

			void printTable(){

				for (int i = 0; i < tablesize; i++)
				{
					Node* temp=table[i];
					while(temp!=nullptr){
						cout<< temp->key<<" [position:"<<temp->first_position<<"; counter:"<<temp->counter<<"]\n"<<endl;
						temp=temp->next;
					}
				}
			}

};

HashTable readFileToHashTable(const string &filename) {
    fstream file;
    string word;

    // opening file
    file.open(filename.c_str());

    HashTable hashTable;
    long position = 1;
    // extracting words from the file
    while (file >> word) {
        // change everything to lowercase ?
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        transform(word.begin(), word.end(), word.begin(), ::tolower);

        hashTable.put(word, position);
        position++;
    }

    file.close();

    return hashTable;
}

int main(void)
{

	  HashTable sherlock = readFileToHashTable("SherlockHolmes.txt");

	  vector<Stats> stats=sherlock.computeDistances();

	 for (int i = 0; i < stats.size(); i++)
	  {
	    Stats s=stats[i];
	    cout<<"Word: "<<s.key<<" | counter: "<<s.counter<<" | max distance: "<<s.maxDistance<<" | mix distance: "
	    <<s.minDistance<<" | avg distance: "<<s.avgDistance<<"\n"<<endl;
	  }


	  cout<<"\nNumber of words-->"<<sherlock.getNwords()<<endl;

  return 0;
}

#endif

