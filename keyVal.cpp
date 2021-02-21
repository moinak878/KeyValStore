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
#include<climits>

using namespace std;

int MAX; 
class BPTree;
class Node
{
	bool IS_LEAF;
	int *key, size;
	Node** ptr;
	friend class BPTree;
public:
	Node();
};

class BPTree
{
	Node *root;
	void insertInternal(int,Node*,Node*);
	void removeInternal(int,Node*,Node*);
	Node* findParent(Node*,Node*);
public:
	BPTree();
	void search(int);
	void insert(int);
	void remove(int);
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
			if(!command.substr(0,6).compare("insert"))
			{
				stringstream argument(command.substr(7));
				argument>>x;
				bpt.insert(x);
			}
			else if(!command.substr(0,6).compare("delete"))
			{
				stringstream argument(command.substr(7));
				argument>>x;
				bpt.remove(x);
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
	cout<<"Commands:\nsearch <value> to search\n";
	cout<<"insert <value> to insert\n";
	cout<<"delete <value> to delete\n";
	cout<<"display to display\n";
	cout<<"save to save log\n";
	cout<<"exit to exit\n";
	do
	{
		cout<<"Enter command: ";
		getline(cin,command);
		if(!command.substr(0,6).compare("search"))
		{
			stringstream argument(command.substr(7));
			argument>>x;
			bpt.search(x);
		}
		else if(!command.substr(0,6).compare("insert"))
		{
			stringstream argument(command.substr(7));
			argument>>x;
			bpt.insert(x);
			tempLog.append(command);
			tempLog.append("\n");
		}
		else if(!command.substr(0,6).compare("delete"))
		{
			stringstream argument(command.substr(7));
			argument>>x;
			bpt.remove(x);
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
	ptr = new Node*[MAX+1];
}
BPTree::BPTree()
{
	root = NULL;
}
void BPTree::search(int x)
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
				cout<<"Found\n";
				return;
			}
		}
		cout<<"Not found\n";
	}
}
void BPTree::insert(int x)
{
	if(root==NULL)
	{
		root = new Node;
		root->key[0] = x;
		root->IS_LEAF = true;
		root->size = 1;
		cout<<"Created root\nInserted "<<x<<" successfully\n";
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
			}
			temp->key[i] = x;
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
			for(int i = 0; i < MAX; i++)
			{
				virtualNode[i] = temp->key[i];
			}
			int i = 0, j;
			while(x > virtualNode[i] && i < MAX) i++;
			
			for(int j = MAX+1;j > i; j--)
			{
				virtualNode[j] = virtualNode[j-1];
			}
			virtualNode[i] = x; 
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
			}
			for(i = 0, j = temp->size; i < newLeaf->size; i++, j++)
			{
				newLeaf->key[i] = virtualNode[j];
			}
			//modify parent
			if(temp == root)
			{
				//create new root
				Node* newRoot = new Node;
				newRoot->key[0] = newLeaf->key[0];
				newRoot->ptr[0] = temp;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
				cout<<"Created new root\n";
			}
			else
			{
				insertInternal(newLeaf->key[0],parent,newLeaf);
			}
		}
	}
}
void BPTree::insertInternal(int x, Node* temp, Node* child)
{
	if(temp->size < MAX)
	{
		
		int i = 0;
		while(x > temp->key[i] && i < temp->size) i++;
		
		for(int j = temp->size;j > i; j--)
		{
			temp->key[j] = temp->key[j-1];
		}
		for(int j = temp->size+1; j > i+1; j--)
		{
			temp->ptr[j] = temp->ptr[j-1];
		}
		temp->key[i] = x;
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
		Node* virtualPtr[MAX+2];
		for(int i = 0; i < MAX; i++)
		{
			virtualKey[i] = temp->key[i];
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
		}
		virtualKey[i] = x; 
		
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
			newRoot->ptr[0] = temp;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
			cout<<"Created new root\n";
		}
		else
		{
			
			insertInternal(temp->key[temp->size] ,findParent(root,temp) ,newInternal); //using dfs
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
void BPTree::remove(int x)
{
	
	if(root==NULL)
	{
		cout<<"Tree empty\n";
	}
	else
	{
		Node* temp = root;
		Node* parent;
		int leftSibling, rightSibling;
		
		while(temp->IS_LEAF == false)
		{
			for(int i = 0; i < temp->size; i++)
			{
				parent = temp;
				leftSibling = i-1; 
				rightSibling =  i+1; 
				if(x < temp->key[i])
				{	
					temp = temp->ptr[i];
					break;
				}
				if(i == temp->size - 1)
				{
					leftSibling = i;
					rightSibling = i+2;
					temp = temp->ptr[i+1];
					break;
				}
			}
		}
		
		bool found = false;
		int pos;
		for(pos = 0; pos < temp->size; pos++)
		{
			if(temp->key[pos] == x)
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			cout<<"Not found\n";
			return;
		}
		//del key
		for(int i = pos; i < temp->size; i++)
		{
			temp->key[i] = temp->key[i+1];
		}
		temp->size--;
		if(temp == root)//if root,make all pointers NULL
		{
			cout<<"Deleted "<<x<<" from leaf node successfully\n";
			for(int i = 0; i < MAX+1; i++)
			{
				temp->ptr[i] = NULL;
			}
			if(temp->size == 0)//if all keys are deleted
			{
				cout<<"Tree died\n";
				delete[] temp->key;
				delete[] temp->ptr;
				delete temp;
				root = NULL;
			}
			return;
		}
		temp->ptr[temp->size] = temp->ptr[temp->size+1];
		temp->ptr[temp->size+1] = NULL;
		cout<<"Deleted "<<x<<" from leaf node successfully\n";
		if(temp->size >= (MAX+1)/2)//no underflow
		{
			return;
		}
		cout<<"Underflow in leaf node!\n";
		//underflow condition
		//first we try to transfer a key from sibling node
		//check if left sibling exists
		if(leftSibling >= 0)
		{
			Node *leftNode = parent->ptr[leftSibling];
			//check if it is possible to transfer
			if(leftNode->size >= (MAX+1)/2+1)
			{
				//make space for transfer
				for(int i = temp->size; i > 0; i--)
				{
					temp->key[i] = temp->key[i-1];
				}
				//shift pointer to next leaf
				temp->size++;
				temp->ptr[temp->size] = temp->ptr[temp->size-1];
				temp->ptr[temp->size-1] = NULL;
				//transfer
				temp->key[0] = leftNode->key[leftNode->size-1];
				//shift pointer of leftsibling
				leftNode->size--;
				leftNode->ptr[leftNode->size] = temp;
				leftNode->ptr[leftNode->size+1] = NULL;
				//update parent
				parent->key[leftSibling] = temp->key[0];
				cout<<"Transferred "<<temp->key[0]<<" from left sibling of leaf node\n";
				return;
			}
		}
		if(rightSibling <= parent->size)//check if right sibling exist
		{
			Node *rightNode = parent->ptr[rightSibling];
			//check if it is possible to transfer
			if(rightNode->size >= (MAX+1)/2+1)
			{
				//shift pointer to next leaf
				temp->size++;
				temp->ptr[temp->size] = temp->ptr[temp->size-1];
				temp->ptr[temp->size-1] = NULL;
				//transfer
				temp->key[temp->size-1] = rightNode->key[0];
				//shift pointer of rightsibling
				rightNode->size--;
				rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size+1];
				rightNode->ptr[rightNode->size+1] = NULL;
				//shift conent of right sibling
				for(int i = 0; i < rightNode->size; i++)
				{
					rightNode->key[i] = rightNode->key[i+1];
				}
				//update parent
				parent->key[rightSibling-1] = rightNode->key[0];
				cout<<"Transferred "<<temp->key[temp->size-1]<<" from right sibling of leaf node\n";
				return;
			}
		}
		//must merge and delete a node
		if(leftSibling >= 0)//if left sibling exist
		{
			Node* leftNode = parent->ptr[leftSibling];
			// transfer all keys to leftsibling and then transfer pointer to next leaf node
			for(int i = leftNode->size, j = 0; j < temp->size; i++, j++)
			{
				leftNode->key[i] = temp->key[j];
			}
			leftNode->ptr[leftNode->size] = NULL;
			leftNode->size += temp->size;
			leftNode->ptr[leftNode->size] = temp->ptr[temp->size];
			cout<<"Merging two leaf nodes\n";
			removeInternal(parent->key[leftSibling],parent,temp);// delete parent node key
			delete[] temp->key;
			delete[] temp->ptr;
			delete temp;
		}
		else if(rightSibling <= parent->size)//if right sibling exist
		{
			Node* rightNode = parent->ptr[rightSibling];
			// transfer all keys to temp and then transfer pointer to next leaf node
			for(int i = temp->size, j = 0; j < rightNode->size; i++, j++)
			{
				temp->key[i] = rightNode->key[j];
			}
			temp->ptr[temp->size] = NULL;
			temp->size += rightNode->size;
			temp->ptr[temp->size] = rightNode->ptr[rightNode->size];
			cout<<"Merging two leaf nodes\n";
			removeInternal(parent->key[rightSibling-1],parent,rightNode);// delete parent node key
			delete[] rightNode->key;
			delete[] rightNode->ptr;
			delete rightNode;
		}
	}
}
void BPTree::removeInternal(int x, Node* temp, Node* child)
{
	//deleting the key x first
	//checking if key from root is to be deleted
	if(temp == root)
	{
		if(temp->size == 1)//if only one key is left, change root
		{
			if(temp->ptr[1] == child)
			{
				delete[] child->key;
				delete[] child->ptr;
				delete child;
				root = temp->ptr[0];
				delete[] temp->key;
				delete[] temp->ptr;
				delete temp;
				cout<<"Changed root node\n";
				return;
			}
			else if(temp->ptr[0] == child)
			{
				delete[] child->key;
				delete[] child->ptr;
				delete child;
				root = temp->ptr[1];
				delete[] temp->key;
				delete[] temp->ptr;
				delete temp;
				cout<<"Changed root node\n";
				return;
			}
		}
	}
	int pos;
	for(pos = 0; pos < temp->size; pos++)
	{
		if(temp->key[pos] == x)
		{
			break;
		}
	}
	for(int i = pos; i < temp->size; i++)
	{
		temp->key[i] = temp->key[i+1];
	}
	//now deleting the pointer child
	for(pos = 0; pos < temp->size+1; pos++)
	{
		if(temp->ptr[pos] == child)
		{
			break;
		}
	}
	for(int i = pos; i < temp->size+1; i++)
	{
		temp->ptr[i] = temp->ptr[i+1];
	}
	temp->size--;
	if(temp->size >= (MAX+1)/2-1)//no underflow
	{
		cout<<"Deleted "<<x<<" from internal node successfully\n";
		return;
	}
	cout<<"Underflow in internal node!\n";
	//underflow, try to transfer first
	if(temp==root)return;
	Node* parent = findParent(root, temp);
	int leftSibling, rightSibling;
	//finding left n right sibling of temp
	for(pos = 0; pos < parent->size+1; pos++)
	{
		if(parent->ptr[pos] == temp)
		{
			leftSibling = pos - 1;
			rightSibling = pos + 1;
			break;
		}
	}
	//try to transfer
	if(leftSibling >= 0)//if left sibling exists
	{
		Node *leftNode = parent->ptr[leftSibling];
		//check if it is possible to transfer
		if(leftNode->size >= (MAX+1)/2)
		{
			//make space for transfer of key
			for(int i = temp->size; i > 0; i--)
			{
				temp->key[i] = temp->key[i-1];
			}
			//transfer key from left sibling through parent
			temp->key[0] = parent->key[leftSibling];
			parent->key[leftSibling] = leftNode->key[leftNode->size-1];
			//transfer last pointer from leftnode to temp
			//make space for transfer of ptr
			for (int i = temp->size+1; i > 0; i--)
			{
				temp->ptr[i] = temp->ptr[i-1];
			}
			//transfer ptr
			temp->ptr[0] = leftNode->ptr[leftNode->size];
			temp->size++;
			leftNode->size--;
			cout<<"Transferred "<<temp->key[0]<<" from left sibling of internal node\n";
			return;
		}
	}
	if(rightSibling <= parent->size)//check if right sibling exist
	{
		Node *rightNode = parent->ptr[rightSibling];
		//check if it is possible to transfer
		if(rightNode->size >= (MAX+1)/2)
		{
			//transfer key from right sibling through parent
			temp->key[temp->size] = parent->key[pos];
			parent->key[pos] = rightNode->key[0];
			for (int i = 0; i < rightNode->size -1; i++)
			{
				rightNode->key[i] = rightNode->key[i+1];
			}
			//transfer first pointer from rightnode to temp
			//transfer ptr
			temp->ptr[temp->size+1] = rightNode->ptr[0];
			for (int i = 0; i < rightNode->size; ++i)
			{
				rightNode->ptr[i] = rightNode->ptr[i+1];
			}
			temp->size++;
			rightNode->size--;
			cout<<"Transferred "<<temp->key[0]<<" from right sibling of internal node\n";
			return;
		}
	}
	//transfer wasnt posssible hence do merging
	if(leftSibling >= 0)
	{
		//leftnode + parent key + temp
		Node *leftNode = parent->ptr[leftSibling];
		leftNode->key[leftNode->size] = parent->key[leftSibling];
		for(int i = leftNode->size+1, j = 0; j < temp->size; j++)
		{
			leftNode->key[i] = temp->key[j];
		}
		for(int i = leftNode->size+1, j = 0; j < temp->size+1; j++)
		{
			leftNode->ptr[i] = temp->ptr[j];
			temp->ptr[j] = NULL;
		}
		leftNode->size += temp->size+1;
		temp->size = 0;
		//delete temp
		removeInternal(parent->key[leftSibling], parent, temp);
		cout<<"Merged with left sibling\n";

	}
	else if(rightSibling <= parent->size)
	{
		//temp + parent key + rightnode
		Node *rightNode = parent->ptr[rightSibling];
		temp->key[temp->size] = parent->key[rightSibling-1];
		for(int i = temp->size+1, j = 0; j < rightNode->size; j++)
		{
			temp->key[i] = rightNode->key[j];
		}
		for(int i = temp->size+1, j = 0; j < rightNode->size+1; j++)
		{
			temp->ptr[i] = rightNode->ptr[j];
			rightNode->ptr[j] = NULL;
		}
		temp->size += rightNode->size+1;
		rightNode->size = 0;
		//delete temp
		removeInternal(parent->key[rightSibling-1], parent, rightNode);
		cout<<"Merged with right sibling\n";
	}
}
void BPTree::display(Node* temp)
{
	//depth first display
	if(temp!=NULL)
	{
		for(int i = 0; i < temp->size; i++)
		{
			cout<<temp->key[i]<<" ";
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
	//clean up logic
	if(temp!=NULL)
	{
		if(temp->IS_LEAF != true)
		{
			for(int i = 0; i < temp->size+1; i++)
			{
				cleanUp(temp->ptr[i]);
			}
		}
		for(int i = 0; i < temp->size; i++)
		{
			cout<<"Deleted key from memory: "<<temp->key[i]<<"\n";
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
