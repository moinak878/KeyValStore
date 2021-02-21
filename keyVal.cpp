//Author: moinak878


/**
			implement a simple B+ Tree
			all cmds entered are logged in a file
			btree rebuilt from the log file. --> perisistent 
			TODO :
				user auth (??)
				transactions (theory done)

**/
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>

using namespace std;

int MAX; 
class BPTree;
class Node
{
	bool IS_LEAF;
	int *key,size;
	string *value;
	Node** ptr;
	friend class BPTree;
public:
	Node();
};

class BPTree
{
	Node *root;
	void setInternal(int,string,Node*,Node*);
	Node* findParent(Node*,Node*);
public:
	BPTree();
	void get(int);
	void set(int,string);
	void display(Node*);
	Node* getRoot();
	void cleanUp(Node*);
	~BPTree();
};

int main(int argc, char* argv[])
{
	BPTree bpt;
	string command;
	int x;
	string val;
	bool close = false;
	string tempLog; //log string 
	ifstream fin;
	ofstream fout;

	if(argc > 1)
	{
		fin.open(argv[1]);
		if(!fin.is_open())
		{
			cout<<"File not found\n";
			return 0;
		}
		int i = 1;
		getline(fin, tempLog, '\0'); 
		fin.close();
		fin.open(argv[1]);
		getline(fin,command);
		stringstream max(command);
		max>>MAX;
		while(getline(fin,command))
		{
			if(!command.substr(0,3).compare("set"))
			{
				stringstream argument(command.substr(4,5));
				argument>>x;
				stringstream arg(command.substr(6));
				arg>>val;
				bpt.set(x,val);
			}
			else
			{
				cout<<"Unknown command: "<<command<<" at line #"<<i<<"\n";
				return 0;
			}
			i++;
		}
		cout<<"Tree loaded successfully from: \""<<argv[1]<<"\"\n";
		fin.close();
	}
	else
	{
		cout<<"Enter the max degree\n";
		cin>>command;
		stringstream max(command);
		max>>MAX;
		tempLog.append(command);
		tempLog.append("\n");
		cin.clear();
		cin.ignore(1);
	}
	
	//main menu
	cout<<"Commands:\nget <key> to get\n";
	cout<<"set <key> <value> to set\n";
	cout<<"display to display\n";
	cout<<"save to save log\n";
	cout<<"exit to exit\n";
	do
	{
		cout<<"Enter command: ";
		getline(cin,command);
		if(!command.substr(0,3).compare("get"))
		{
			stringstream argument(command.substr(4));
			argument>>x;
			bpt.get(x);
		}
		else if(!command.substr(0,3).compare("set"))
		{
			stringstream argument(command.substr(4,5));
			argument>>x;
			stringstream arg(command.substr(6));
			arg>>val;
			bpt.set(x,val);
			tempLog.append(command);
			tempLog.append("\n");
		}
		else if(!command.compare("display"))
		{
			bpt.display(bpt.getRoot());
		}

		else if(!command.compare("save"))
		{
			cout<<"Enter file name: ";
			string filename;
			cin>>filename;
			fout.open(filename);
			fout<<tempLog;
			fout.close();
			cout<<"Saved successfully into file: \""<<filename<<"\"\n";
			cin.clear();
			cin.ignore(1);
		}

		else if(!command.compare("exit"))
		{
			close = true;
		}
		else
		{
			cout<<"Invalid command\n";
		}
	}while(!close);
	return 0;
}
Node::Node()
{
	key = new int[MAX];
	value = new string[MAX];
	ptr = new Node*[MAX+1];
}
BPTree::BPTree()
{
	root = NULL;
}
void BPTree::get(int x)
{
	
	if(root==NULL)
	{
		cout<<"Tree empty\n";
	}
	else
	{
		Node* temp = root;
		while(temp->IS_LEAF == false)
		{
			for(int i = 0; i < temp->size; i++)
			{
				if(x < temp->key[i])
				{
					temp = temp->ptr[i];
					break;
				}
				if(i == temp->size - 1)
				{
					temp = temp->ptr[i+1];
					break;
				}
			}
		}
		for(int i = 0; i < temp->size; i++)
		{
			if(temp->key[i] == x)
			{
				cout<<"Found (key,value) : ( "<<temp->key[i]<<" , "<<temp->value[i]<<" )\n";
				return;
			}
		}
		cout<<"Not found\n";
	}
}
void BPTree::set(int x,string s)
{
	if(root==NULL)
	{
		root = new Node;
		root->key[0] = x;
		root->value[0]=s;
		root->IS_LEAF = true;
		root->size = 1;
		cout<<"Created root\nInserted "<<"("<<x<<","<<s<<") successfully\n";
	}
	else
	{
		Node* temp = root;
		Node* parent;
		while(temp->IS_LEAF == false)
		{
			parent = temp;
			for(int i = 0; i < temp->size; i++)
			{
				if(x < temp->key[i])
				{
					temp = temp->ptr[i];
					break;
				}
				if(i == temp->size - 1)
				{
					temp = temp->ptr[i+1];
					break;
				}
			}
		}
		if(temp->size < MAX)
		{
			int i = 0;
			while(x > temp->key[i] && i < temp->size) i++;
			for(int j = temp->size;j > i; j--)
			{
				temp->key[j] = temp->key[j-1];
				temp->value[j]=temp->value[j-1];
			}
			temp->key[i] = x;
			temp->value[i]=s;
			temp->size++;
			temp->ptr[temp->size] = temp->ptr[temp->size-1];
			temp->ptr[temp->size-1] = NULL;
			cout<<"Inserted "<<x<<" successfully\n";
		}
		else
		{
			cout<<"Inserted "<<x<<" successfully\n";
			cout<<"Overflow in leaf node!\nSplitting leaf node\n";
			//overflow 
			Node* newLeaf = new Node;
			int virtualNode[MAX+1];
			string virtualVal[MAX+1];
			for(int i = 0; i < MAX; i++)
			{
				virtualNode[i] = temp->key[i];
				virtualVal[i] = temp->value[i];
			}
			int i = 0, j;
			while(x > virtualNode[i] && i < MAX) i++;
			
			for(int j = MAX+1;j > i; j--)
			{
				virtualNode[j] = virtualNode[j-1];
				virtualVal[j] = virtualVal[j-1];
			}
			virtualNode[i] = x; 
			virtualVal[i]=s;
			newLeaf->IS_LEAF = true;
			//split temp into 2 Leaf nodes
			temp->size = (MAX+1)/2;
			newLeaf->size = MAX+1-(MAX+1)/2;
			temp->ptr[temp->size] = newLeaf;
			//newleaf node points to the next leaf node
			newLeaf->ptr[newLeaf->size] = temp->ptr[MAX];
			temp->ptr[MAX] = NULL;
			//elements into new leaf nodes
			for(i = 0; i < temp->size; i++)
			{
				temp->key[i] = virtualNode[i];
				temp->value[i]=virtualVal[i];
			}
			for(i = 0, j = temp->size; i < newLeaf->size; i++, j++)
			{
				newLeaf->key[i] = virtualNode[j];
				newLeaf->value[i]=virtualVal[j];
			}
			//modify parent
			if(temp == root)
			{
				//create new root
				Node* newRoot = new Node;
				newRoot->key[0] = newLeaf->key[0];
				newRoot->value[0]=newLeaf->value[0];
				newRoot->ptr[0] = temp;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
				cout<<"Created new root\n";
			}
			else
			{
				setInternal(newLeaf->key[0],newLeaf->value[0],parent,newLeaf);
			}
		}
	}
}
void BPTree::setInternal(int x,string s, Node* temp, Node* child)
{
	if(temp->size < MAX)
	{
		
		int i = 0;
		while(x > temp->key[i] && i < temp->size) i++;
		
		for(int j = temp->size;j > i; j--)
		{
			temp->key[j] = temp->key[j-1];
			temp->value[j]=temp->value[j-1];
		}
		for(int j = temp->size+1; j > i+1; j--)
		{
			temp->ptr[j] = temp->ptr[j-1];
		}
		temp->key[i] = x;
		temp->value[i]=s;
		temp->size++;
		temp->ptr[i+1] = child;
		cout<<"Inserted key in an Internal node successfully\n";
	}
	else
	{
		cout<<"Inserted key in an Internal node successfully\n";
		cout<<"Overflow in internal node!\nSplitting internal node\n";
		//ovrflw
		Node* newInternal = new Node;
		
		int virtualKey[MAX+1];
		string virtualVal[MAX+1];
		Node* virtualPtr[MAX+2];
		for(int i = 0; i < MAX; i++)
		{
			virtualKey[i] = temp->key[i];
			virtualVal[i] = temp->value[i];
		}
		for(int i = 0; i < MAX+1; i++)
		{
			virtualPtr[i] = temp->ptr[i];
		}
		int i = 0, j;
		while(x > virtualKey[i] && i < MAX) i++;
		
		for(int j = MAX+1;j > i; j--)
		{
			virtualKey[j] = virtualKey[j-1];
			virtualVal[j] = virtualVal[j-1];
		}
		virtualKey[i] = x; 
		virtualVal[i]=s;
		
		for(int j = MAX+2;j > i+1; j--)
		{
			virtualPtr[j] = virtualPtr[j-1];
		}
		virtualPtr[i+1] = child; 
		newInternal->IS_LEAF = false;
		
		temp->size = (MAX+1)/2;
		newInternal->size = MAX-(MAX+1)/2;
		
		for(i = 0, j = temp->size+1; i < newInternal->size; i++, j++)
		{
			newInternal->key[i] = virtualKey[j];
			newInternal->value[i] = virtualVal[j];
		}
		for(i = 0, j = temp->size+1; i < newInternal->size+1; i++, j++)
		{
			newInternal->ptr[i] = virtualPtr[j];
		}
		// m = temp->key[temp->size]
		if(temp == root)
		{
			
			Node* newRoot = new Node;
			newRoot->key[0] = temp->key[temp->size];
			newRoot->value[0]=temp->value[temp->size];
			newRoot->ptr[0] = temp;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
			cout<<"Created new root\n";
		}
		else
		{
			
			setInternal(temp->key[temp->size],temp->value[temp->size],findParent(root,temp) ,newInternal); //using dfs
		}
	}
}
Node* BPTree::findParent(Node* temp, Node* child)
{
	
	Node* parent;
	if(temp->IS_LEAF || (temp->ptr[0])->IS_LEAF)
	{
		return NULL;
	}
	for(int i = 0; i < temp->size+1; i++)
	{
		if(temp->ptr[i] == child)
		{
			parent = temp;
			return parent;
		}
		else
		{
			parent = findParent(temp->ptr[i],child);
			if(parent!=NULL)return parent;
		}
	}
	return parent;
}


void BPTree::display(Node* temp)
{
	if(temp==NULL)
		cout<<"Tree is empty !\n\n";
	//depth first display
	if(temp!=NULL)
	{
		cout<<"\n\nAll (key,value) pairs :-\n";
		for(int i = 0; i < temp->size; i++)
		{
			cout<<"("<<temp->key[i]<<","<<temp->value[i]<<")"<<"\n";
		}
		cout<<"\n";
		if(temp->IS_LEAF != true)
		{
			for(int i = 0; i < temp->size+1; i++)
			{
				display(temp->ptr[i]);
			}
		}
	}
}
Node* BPTree::getRoot()
{
	return root;
}
void BPTree::cleanUp(Node* temp)
{
	if(temp!=NULL)
	{
		if(temp->IS_LEAF != true)
		{
			for(int i = 0; i < temp->size+1; i++)
			{
				cleanUp(temp->ptr[i]);
			}
		}
		delete[] temp->key;
		delete[] temp->ptr;
		delete temp;
	}
}
BPTree::~BPTree() //dtor
{
	cleanUp(root);
}
