#include<iostream>
#include<string>
#include <fstream>
using namespace std;

template<class T>
class Node
{
public:
	T data;
	Node<T> * next;
	Node(T d = 0, Node* N = NULL)
	{
		data = d;
		next = N;
	}
};

template<class T>
class Stack
{
private:
	Node<T> * top;
public:
	Stack()
	{
		top = NULL;
	}
	~Stack()
	{
		Node<T>* nodeToDelete = 0;
		while (top != 0)
		{
			nodeToDelete = top;
			top = top->next;
			delete nodeToDelete;
		}
	}
	bool isEmpty()
	{
		if (top == NULL)
		{
			return true;
		}
		return false;
	}
	bool push(const T & val)
	{
		if (isEmpty())
		{
			top = new Node<T>(val, NULL);
			//cout << "pushed " << val << endl;
		}
		else
		{
			Node<T> * newNode = new Node<T>(val, top);
			top = newNode;
			//cout << "pushed " << val << endl;
		}
		return true;
	}
	T pop()
	{
		T el;
		if (!isEmpty())
		{
			el = top->data;
			Node<T> * temp = top;
			top = temp->next;
			delete temp;
			temp = NULL;
		}
		//cout << "\t\t\t\t\tpopped " << el << endl;
		return el;
	}
	T get_top()
	{
		return top->data;
	}
	void Print()
	{
		for (Node<T>* temp = top; temp != 0; temp = temp->next)
		{
			cout << temp->data << "\t";
		}
		cout << endl;
	}
};

class XMLData
{
	friend int ReadDataFromFile(ifstream &fin, Stack<XMLData> &);
	friend ostream& operator<<(ostream& out, const XMLData &);
private:
	int line_count;
	char* tag;
public:
	XMLData()
	{
		line_count = 0;
		tag = nullptr;
	}

	XMLData(int l, char*buffer)
	{
		line_count = l;

		int len = strlen(buffer);
		tag = new char[len + 1];
		strcpy(tag, buffer);
		tag[len + 1] = '\0';

	}

	void show_error(char ch, int l)
	{
		cout << endl << "ERROR: " << endl;
		cout << "\tAt line number:\t" << l << endl;
		cout << "\tMissing symbol:\t" << ch << endl << endl;
	}

};

int ReadDataFromFile(ifstream &fin, Stack<XMLData> &X)
{
	Stack<char> S;
	fin.open("h.txt");

	char Buffer[180];
	char*buffer;
	char ch;
	XMLData x;
	char temp[80];

	while (!fin.eof())
	{
		fin.getline(Buffer, 180);			//imput a new line

		x.line_count++;						//keeping track of line count

		int len = strlen(Buffer);
		buffer = new char[len + 1];
		strcpy(buffer, Buffer);
		buffer[len + 1] = '\0';				//allocating and placing null because charcter pointer is used instead of string

		int dq_count = 0;					//double quotes
		int sq_count = 0;					//single quotes
		int q_count = 0;					//question mark
		int h_count = 0;					//hyphen

		bool check_line = false;

		for (int i = 0; buffer[i] != '\0'; i++)//to determine whether the line has to be checked or not
		{                                      //if it doesnt contain any tags, no need to check
			if (buffer[i] == '<')
			{
				check_line = true;
				break;
			}
		}

		for (int i = 0; buffer[i] != '\0'; i++)		// this loop checks for the matching characters i.e <,?,",' etc
		{
			int j = 0;
			if (check_line)
			{
				if (buffer[i] == '<')			//checks the matching of <
				{
					S.push(buffer[i]);
					if (x.line_count == 1)		//conformity of header 
					{
						if (buffer[i + 1] != '?')//header always starts with <?
						{
							cout << "ERROR: Prolog format is not proper or prolog is absent." << endl << endl;
							return 0;
						}
					}
					if (buffer[i + 1] == '!')	//comments always start with <!
					{
						if (buffer[i + 2] != '-' || buffer[i + 3] != '-') //syntax of header <!--
						{
							cout << "\n\nERROR:\n\tThe syntax of comment is incorrect at line number " << x.line_count << endl << endl << endl;
							return 0;
						}
						if (buffer[i + 2] == '-' && buffer[i + 3] == '-')
						{
							i = i + 4;
							while (buffer[i] != '-')					//ignore the comment
								i++;
						}
					}
				}
				if (buffer[i] == '"')
				{
					if (dq_count < 1)
					{
						dq_count++;
						S.push(buffer[i]);				//same as checking hyphens, 
					}									//a counter is set to keep track of incoming double quotes 
					else                                //and their pushing and popping accordingly
					{
						ch = S.pop();
						if (ch != buffer[i])
						{
							x.show_error(ch, x.line_count);
							i--;
							break;
						}
						dq_count = 0;
					}
				}
				if (buffer[i] == '?')
				{
					if (q_count < 1)
					{
						q_count++;							//same as checking hyphens, a counter is set to keep track 
						S.push(buffer[i]);
						if (x.line_count == 1)				//checking the syntax of header
						{
							if (buffer[i - 1] != '<')		//if a ? occurs and is not preceded by < it's a syntax error in the header
							{
								cout << "Prolog format is not proper." << endl << endl;
								return 0;
							}
						}
					}
					else
					{
						ch = S.pop();
						if (ch != buffer[i])
						{
							x.show_error(ch, x.line_count);
							return 0;
						}
					}
				}
				if (buffer[i] == '>')
				{
					if (!S.isEmpty())			//while popping when > occurs, the stack may already be empty in case there is an error
					{
						ch = S.pop();
						if (ch != '<')			//mismatch
						{
							x.show_error(ch, x.line_count);
							return 0;
						}
					}
					else
					{
						x.show_error('<', x.line_count);		//if stack is empty, it means their was < character missing in the start
						return 0;
					}
					i++;
					while (buffer[i] != '<' && buffer[i] != '\0')		//to ignore the text content
						i++;											//so that if any of those characters occurs in the text content for which a check has been implemented, 
					i--;												//those characters wont be pushed on the stack
				}
				if (buffer[i] == 39)		//checks for singlw quotes
				{
					if (sq_count < 1)
					{
						sq_count++;
						S.push(buffer[i]);				//same as checking hyphens, 
					}                                   //a counter is set to keep track of incoming double quotes 
					else                                //and their pushing and popping accordingly
					{
						ch = S.pop();
						if (ch != buffer[i])
						{
							x.show_error(ch, x.line_count);
							i--;
							break;
						}
						sq_count = 0;
					}
				}
				if (buffer[i] == '-')
				{
					if (x.line_count != 1)
					{
						if (h_count < 1)		//pushes two hyphens
						{
							h_count++;
							S.push(buffer[i]);
						}
						else
						{
							ch = S.pop();		//if two hypehns are already pushed, pop one in case of another occurance 
							if (ch != buffer[i])//compares the popped character with the character currently in buffer
							{
								x.show_error(ch, x.line_count);
								return 0;		//terminates program after showing error
							}
						}
					}
					if (buffer[i - 1] == '<' && buffer[i - 1] != '!') //if a hyphen occurs any place other than the header, it should not show an error
					{
						cout << "\n\nERROR:\n\tThe syntax of comment is incorrect at line number " << x.line_count << endl << endl << endl;
						return 0;
					}
				}
			}
		}
		while (!S.isEmpty())				// when all the charcters in a complete line have been parsed and checked and stack has been popped simultaneously 
		{									//and at the end the stack is not empty, it indicates that
			char ch = S.pop();				// the match of a some of the already pushed characters does not exist in the file
			if (ch == '<')
				x.show_error('>', x.line_count);
			else
				x.show_error(ch, x.line_count);
			return 0;
		}

		/////////////////////////////////////////

		for (int i = 0; buffer[i] != '\0'; i++)				//this loop matches tags
		{
			int j = 0;
			if (x.line_count != 1)							//ignores header while matching tags
			{
				if (buffer[i] == '<')						//this checks the starting of tag. so any data without it is not pushed on the stack
				{
					if (buffer[i + 1] == '!')				//if ! occurs, then it is a comment and is ignored altogether
						break;
					i++;
					if (buffer[i] != '/')					//if i+1 is not / then it is not a closing tag and needs to be pushed in the stack
					{
						j = 0;
						while (buffer[i] != '>' && buffer[i] != 32)		//tag is completed when > is found or a space occurs after which the attribute starts
							temp[j++] = buffer[i++];
						temp[j] = '\0';
						XMLData d(x.line_count, temp);					//pushes the tag in the stack
						X.push(d);
					}
					else                                    //if a / occurs after < means it is a closing tag
					{
						j = 0;
						i++;								// minus / characters store in string
						while (buffer[i] != '>')
							temp[j++] = buffer[i++];
						temp[j] = '\0';

						XMLData temp1;
						temp1 = X.pop();
						if (strcmp(temp1.tag, temp))		//comparing after popping
						{
							cout << endl << "ERROR: " << endl;
							cout << "\tTag <" << temp1.tag << "> mentioned at line number " << temp1.line_count << " is mismatched." << endl;
							cout << "\n(Either the closing tag is missing, '/' is not present in the closing tag, there is a case mismatch error in the opening and closing tags or the opening tag of the next closing tag is missing.)\n" << endl;
							return 0;
						}
					}
				}
			}

		}
	}

	if (!X.isEmpty())
	{
		XMLData temp1;					// if stack is not empty at the end of file it means there is a missing tag
		temp1 = X.pop();
		cout << endl << "ERROR: " << endl;
		cout << "\tMissing closing tag of <" << temp1.tag << "> mentioned at line number " << temp1.line_count << endl;
		cout << "\n(Either the closing tag is missing, '/' is not present in the closing tag or there is a case mismatch error in the opening and closing tags)\n" << endl;
		return 0;
	}

	cout << "\n\nThere are no errors in the XML File!" << endl << endl;
	return 1;
}

ostream& operator<<(ostream& out, const XMLData &obj)
{
	out << obj.tag;
	return out;
}

int main()
{
	ifstream fin;
	Stack<XMLData> X;
	ReadDataFromFile(fin, X);
	return 0;
}