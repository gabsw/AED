#ifndef AED
#define AED
#include <iostream>
#include <string.h>
using namespace std;

class HashTable{
	private:
		class Node{
			public:
				char* key;
				int position;
				int counter;
				Node* next;
				Node* previous;
				Node* tail;
		};
	private:
		int tablesize=100;
		Node** table;
		int size;
		const int maxListSize=50;
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
				if (temp!=nullptr)
				{
					Node* tail=temp->tail;
					if (tail==nullptr)
					{
						rePut(newArray,temp,newsize);
					}else{
						while(tail!=nullptr){ //do fim para o inicio
							//se começar no primeiro Node da lista, quando faço put desse elemento, 
							//vou inserir uma lista toda que começa por esse Node
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
			newNode->position = node->position;
			newNode->counter=1;
			newNode->next=nullptr;
			newNode->previous=nullptr;
			newNode->tail=nullptr; //o ultimo Node da lista

			int hash=hash_function(node->key,size);

			Node* last=nullptr;
			Node* temp = newArray[hash];
			Node* head= newArray[hash];

			int listSize=0;
			int keyRepeat = -1;
			if(temp==nullptr){ //se Node temp for null, entao a posicao hash esta vazia
				newArray[hash]=newNode;	//guardar Node temp nessa posicao
			}
			else{
				while(temp!=nullptr) {	//se nao for null, entao a posicao hash nao esta vazia
					if(!strcmp(temp->key,newNode->key)) {	//comparar com a key do primeiro elemento dessa posicao
						temp->counter++;	//se for igual,counter++;
						keyRepeat = 1;		//key esta repetida
						break;
					}else {
						last=temp;	
						temp=temp->next;	//se temp->key!=key, avança para o proximo Node
						listSize++;
					}
				}	
				
				if(keyRepeat!=1){ //se nao encontrar a key na lista,acrescenta a nova key no fim da lista
					last->next = newNode;
					newNode->previous=last;
					head->tail=newNode; //newNode passa ser o ultimo Node da lista que começa por head,ou seja,começa por newArray[hash];
					listSize++;
				}
			}
		}

	public:
			MyHashTable(void){
				this->table=new Node*[tablesize];
			}

			~MyHashTable(void){

			}

			void put(char* key,int position) {
				Node* newNode=new Node;
				newNode->key = key;
				newNode->position = position;
				newNode->counter=1;
				newNode->next=nullptr;
				newNode->previous=nullptr;
				newNode->tail=nullptr;
				int hash=hash_function(key,tablesize);
				Node* last=nullptr;
				Node* temp = table[hash];

				Node* head= table[hash];//tenho de fazer uma copia tambem?

				int listSize=0;
				int keyRepeat = -1;
				if(temp==nullptr){ //se Node temp for null, entao a posicao hash esta vazia
					table[hash]=newNode;	//guardar Node temp nessa posicao
				}
				else{
					while(temp!=nullptr) {	//se nao for null, entao a posicao hash nao esta vazia
						if(!strcmp(temp->key,key)) {	//comparar com a key do primeiro elemento dessa posicao
							temp->counter++;	//se for igual,counter++;
							keyRepeat = 1;		//key esta repetida
							break;
						}else {
							last=temp;	
							temp=temp->next;	//se temp->key!=key, avança para o proximo Node
							listSize++;
						}
					}	
					if(keyRepeat!=1){ //se nao encontrar a key na lista,acrescentar a nova key no fim da lista
						last->next=newNode;
						newNode->previous=last;
						head->tail=newNode;
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
							static int a[2];
							a[0]=temp->position;
							a[1]=temp->counter;
							return a;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}

			int getPosition(char* key) {
				int hash =hash_function(key,tablesize);
				Node* temp = table[hash];
				if(temp==nullptr){
					cout<<"A key nao existe!"<< endl;
				}
				else{
					while(temp!=nullptr) {
						if(!strcmp(temp->key,key)) {
							return temp->position;
						}else {
							temp=temp->next;
						}
					}	
					cout<<"A key nao existe!"<< endl;
				}
			}

			int getounter(char* key) {
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
			
			void printTable(){
				for (int i = 0; i < tablesize; i++)
				{
					Node* temp=table[i];
					while(temp!=nullptr){
						cout<< temp->key<<"[" <<i<<" position:"<<temp->position<<"; counter:"<<temp->counter<<"]\n"<<endl;
						temp=temp->next;
					}
				}
			}

};
#endif
