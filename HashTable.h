#ifndef AED
#define AED
#include <iostream>
#include <string.h>
#include <vector> 
#define INT_MIN -2147483648
#define INT_MAX 2147483647
using namespace std;

class HashTable{
	private:
		class Node{
			public:
				char* key;
				int first_position;
				int last_position;
				int counter;
				vector<int> all_positions;
				Node* next;
				Node* previous;
				Node* tail;
		};
	private:
		int tablesize=100;
		Node** table;
		const int maxListSize=10;
	private:
		unsigned int hash_function(const char *str,unsigned int s)
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
			while(*str !='\0')
				crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
		return crc % s;
		}

		void expand(){
			int newsize=tablesize*1.5;
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
							tail->next=nullptr;
							tail->tail=nullptr;
							tail=tail->previous; 
						}
					}
				}
			}
			this->tablesize=newsize;
			this->table=newArray;
		}

		int rePut(Node** newArray,Node* node,int size){
			Node* newNode=new Node;  //uma copia de node
			newNode->key = node->key;
			newNode->first_position=node->first_position;
			newNode->last_position=node->last_position;
			newNode->counter=1;
			newNode->next=nullptr;
			newNode->previous=nullptr;
			newNode->tail=nullptr; //o ultimo Node da lista

			int hash=hash_function(node->key,size);

			Node* last=nullptr;
			Node* temp = newArray[hash];
			Node* head= newArray[hash];

			int keyRepeat = -1;
			if(temp==nullptr){ //se Node temp for null, então a posicao hash está vazia
				newArray[hash]=newNode;	//guardar Node temp nessa posicao
			}
			else{
				while(temp!=nullptr) {	//se nao for null, entao a posição hash não está vazia
										//temp neste caso é o primeiro Node nessa posição
					if(!strcmp(temp->key,newNode->key)) {	//comparar as keys
						temp->counter++;	//se for igual,counter++;
						keyRepeat = 1;		//como key já existe na posição hash-->keyRepeat=1;
						break;
					}else {
						last=temp;	
						temp=temp->next;	//se temp->key!=key, avança para o proximo Node
					}
				}	
				
				if(keyRepeat!=1){ //se nao encontrar a key na lista,acrescenta a nova key no fim da lista
					last->next = newNode;
					newNode->previous=last;
					head->tail=newNode; //newNode passa ser o ultimo Node da lista que começa por head,ou seja,começa por newArray[hash];
				}
			}
		}

	public:
			HashTable(void){
				this->table=new Node*[tablesize];
			}

			~HashTable(void){

			}

			void put(char* key,int position) {
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

				int listSize=0;
				int keyRepeat = -1;
				if(temp==nullptr){ //se Node temp for null, entao a posicao hash esta vazia
					table[hash]=newNode;	//guardar Node temp nessa posicao
				}
				else{
					while(temp!=nullptr) {	//se nao for null, entao a posicao hash nao esta vazia
											//temp neste caso é o primeiro Node nessa posição
						if(!strcmp(temp->key,key)) {	//comparar as keys
							temp->counter++;	//se for igual,counter++;
							temp->last_position=position;
							temp->all_positions.push_back(position);
							keyRepeat = 1;		//como key já existe na posição hash-->keyRepeat=1;
							break;
						}else {
							last=temp;	
							temp=temp->next;	//se temp->key!=key, avança para o proximo Node
							listSize++;
						}
					}	
					if(keyRepeat!=1){ //se nao encontrar a key na lista,acrescentar o novo Node no fim da lista
						last->next=newNode;//o ultimo Node da lista vai ser newNode
						newNode->previous=last;
						head->tail=newNode; //o tail vai ser newNode
						listSize++;
					}
				}
				if (listSize>maxListSize)
				{
					expand();
				}
			}

			int* getPositionAndCounter(char* key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(!strcmp(temp->key,key)) {
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

			int* getPositions(char* key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(!strcmp(temp->key,key)) {
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

			int getCounter(char* key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(!strcmp(temp->key,key)) {
							return temp->counter;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}

			int size(){
				return tablesize;
			}

			vector<vector<int> > computeDistances(){
				vector<vector<int> > AllResults;
				int max;
				int min;
				int avg;
				int total;
				int size;
				int flag=-1;
				for (int i = 0; i < tablesize; i++)
				{	
					Node* temp=table[i];
					while(temp!=nullptr){
						vector<int> results;
						max=INT_MIN;
						min=INT_MAX;
						avg=0;
						total=0;
						size=0;
						for (int i = 0; i < temp->all_positions.size(); i++)
						{
							for (int k = i+1; k < temp->all_positions.size(); k++)
							{
								int distance=abs(temp->all_positions[i]-temp->all_positions[k]);
								if (distance>max)
								{
									max=distance;
								}
								if (distance<min)
								{
									min=distance;
								}

								total+=distance;
								size++;
								flag=1;
							}
						}
						if (flag==1)
						{
							avg=total/size;
							results.push_back(max);
							results.push_back(min);
							results.push_back(avg);
							AllResults.push_back(results);
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
						cout<< temp->key<<"[ position:"<<temp->first_position<<"; counter:"<<temp->counter<<"]\n"<<endl;
						temp=temp->next;
					}
				}
			}

};
#endif
