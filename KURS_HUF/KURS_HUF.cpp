#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <cstring>
#include <vector>
#include <algorithm>
#include <bitset>
#include <cmath>
#include <sstream>
using namespace std;

//Собственный класс для удобной работы с битами и байтами
static class BitArray
{
public:
    int size;
    vector <char> bytes;
    char* b;

    char masks[8] = { 0b00000001,0b00000010, 0b00000100,
    0b00001000, 0b00010000, 0b00100000, 0b01000000, char(0b10000000) };
    
    BitArray(int size)
    {
        this->size = size;
        int sizeInBytes = size / 8;
        if (size % 8 > 0)
        {
            sizeInBytes = sizeInBytes + 1;
        }
        for (int i = 0; i < sizeInBytes; i++)
        {
            bytes.push_back(0);
        }
    }
    void make_b()
    {
        b = &bytes[0];
    }
    int Get_Size_In_Bytes()
    {
        int sizeInBytes = size / 8;
        if (size % 8 > 0)
        {
            sizeInBytes = sizeInBytes + 1;
        }
        return sizeInBytes;
    }
    int get(int index)
    {
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        if (bitIndex == 7) return (bytes[byteIndex] & 128) > 0 ? 1 : 0;
        else
            return (bytes[byteIndex] & masks[bitIndex]) > 0 ? 1 : 0;
    }
    void set(int index, int value)
    {
        int byteIndex = index / 8;
        int bitIndex = index % 8;
        if (value > 0)
        {
            bytes[byteIndex] = (char)(bytes[byteIndex] | masks[bitIndex]);
        }
        else
        {
            bytes[byteIndex] = (char)(bytes[byteIndex] & ~masks[bitIndex]);
        }

    }
    string toString()
    {
        int j = 0;
        string sb;
        for (int i = 0; i < size; i++)
        {
            sb.append(get(i) > 0 ? "1" : "0");
            j++;
            if (j % 8 == 0) sb += " ";
        }
        return sb;
    }

};
//Структура вершины дерева хаффмана
struct Huffman_Node
{
    char symbol;
    int frequency;
    Huffman_Node* left, * right;

    Huffman_Node(char symbol, int frequency)
    {
        left = NULL;
        right = NULL;
        this->symbol = symbol;
        this->frequency = frequency;
    }
};
//Функция для сравнения двух вершин по частоте, нужна для сортирвоки
struct Compare_Nodes
{
    inline bool operator()(const Huffman_Node Node1, const Huffman_Node Node2)
    {
        return (Node1.frequency < Node2.frequency);
    }
};
//Создание словаря значение+частота
map<char, int> Make_Map_Of_Freq(string MyString)
{
    map<char, int> MyMap;
    int i;
    for (i = 0; i < MyString.length(); i++)
    {
        char c = MyString[i];
        auto item = MyMap.find(c);
        if (item != MyMap.end())
        {
            MyMap[c]++;
        }
        else
            MyMap.insert(make_pair(c, 1));
    }
    return MyMap;
}

//Создание вектора вершин
vector<Huffman_Node> Make_Vector_Of_Nodes(map<char, int> const& MyMap)
{
    vector<Huffman_Node> MyVector;
    for (auto pair : MyMap) {
        MyVector.push_back(Huffman_Node(pair.first, pair.second));
    }
    return MyVector;
}
//Создание бинарного дерева кодов Хаффмана
Huffman_Node Make_Huffman_Tree(vector<Huffman_Node> Vector_Of_Nodes)
{
    while (Vector_Of_Nodes.size() > 1)
    {
        sort(Vector_Of_Nodes.begin(), Vector_Of_Nodes.end(), Compare_Nodes());
        Huffman_Node* Left_Node = new Huffman_Node(Vector_Of_Nodes[0].symbol, Vector_Of_Nodes[0].frequency);
        Left_Node->left = Vector_Of_Nodes[0].left;
        Left_Node->right = Vector_Of_Nodes[0].right;
        Vector_Of_Nodes.erase(Vector_Of_Nodes.begin());
        Huffman_Node* Right_Node = new Huffman_Node(Vector_Of_Nodes[0].symbol, Vector_Of_Nodes[0].frequency);
        Right_Node->left = Vector_Of_Nodes[0].left;
        Right_Node->right = Vector_Of_Nodes[0].right;
        Vector_Of_Nodes.erase(Vector_Of_Nodes.begin());

        Huffman_Node* parent = new Huffman_Node(NULL, Left_Node->frequency + Right_Node->frequency);
        parent->left = Left_Node;
        parent->right = Right_Node;
        Vector_Of_Nodes.push_back(*parent);
    }
    return Vector_Of_Nodes[0];
}

//Получение кода для символа
string Get_Code_For_Symbol(Huffman_Node* Node, char Ch, string ParentCode)
{
    if (Node->symbol == Ch && Node->left==NULL && Node->right==NULL)
    {
        return ParentCode;
    }
    else
    {
        if (Node->left != NULL)
        {
            string Code = Get_Code_For_Symbol(Node->left, Ch, ParentCode + "0");
            if (Code != "")
            {
                return Code;
            }
        }
        if (Node->right != NULL)
        {
            string Code = Get_Code_For_Symbol(Node->right, Ch, ParentCode + "1");
            if (Code != "")
            {
                return Code;
            }
        }
    }
    return "";
}
//Создание словаря символ + его код по Хаффману
map<char, string> Make_Map_Of_Huffman_Codes(map<char, int> Map_Of_Freq, Huffman_Node Hufman_Tree)
{
    map<char, string> Huffman_Codes;
    for (const auto& iter : Map_Of_Freq)
    {
        Huffman_Codes.emplace(iter.first, Get_Code_For_Symbol(&Hufman_Tree, iter.first, ""));
    }
    return Huffman_Codes;
}
//Кодированние данных
string Make_Encode_String(map<char, string> Huffman_Codes, string Data)
{
    string Encode;
    for (int i = 0; i < Data.length(); i++)
    {
        Encode.append(Huffman_Codes[Data[i]]);
    }
    return Encode;
}
//Декодированние данных.
string Make_Decode_String(string Encode, Huffman_Node* Huffman_Tree, map<char, int> Map_Of_Freq)
{
    string Decode;
    Huffman_Node* Cur_Node = Huffman_Tree;
    for (int i = 0; i < Encode.length(); i++)
    {
        if (Encode[i] == '0')
        {
            Cur_Node = Cur_Node->left;
            if (Cur_Node->left== NULL && Cur_Node->right==NULL)
            {
                Decode += Cur_Node->symbol;
                Cur_Node = Huffman_Tree;
            }
        }
        else if (Encode[i] == '1')
        {
            Cur_Node = Cur_Node->right;
            if (Cur_Node->left == NULL && Cur_Node->right == NULL)
            {
                Decode += Cur_Node->symbol;
                Cur_Node = Huffman_Tree;
            }
        }
    }
    return Decode;
}
//Занесение данных из файла в строку
string Read_Incoming_Data(ifstream& in) {
    ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
//Чтение и кодировка данных
int Read_And_Code(string Incoming_Data_Path, string Encoded_Data_Path)
{
    int Map_Of_Freq_Size;
    int Size_After_In_Bits;
    double size1, size2;
    double ratio = 0;;
    ifstream Incoming_File(Incoming_Data_Path, ios_base::binary);
    string Data = Read_Incoming_Data(Incoming_File);
    Incoming_File.close();

    map<char, int> Map_Of_Freq = Make_Map_Of_Freq(Data);
    vector<Huffman_Node> Vector_Of_Nodes = Make_Vector_Of_Nodes(Map_Of_Freq); 
    Huffman_Node Hufman_Tree = Make_Huffman_Tree(Vector_Of_Nodes); 
    map<char, string> Huffman_Codes = Make_Map_Of_Huffman_Codes(Map_Of_Freq, Hufman_Tree); 
    string Encode = Make_Encode_String(Huffman_Codes, Data); 
    cout << endl;
    size1 = Data.length() * 8;
    size2 = Encode.length();
    cout << "Size of original data in bits: " << Data.length() * 8 << endl;
    cout << "Size of compresed data in bits: " << Encode.length() << endl;
    ratio = size2 / size1 * 100;
    cout << "File compression ratio: " << ratio << endl;
    ofstream Encoded_File(Encoded_Data_Path, ios_base::binary);;
    Map_Of_Freq_Size = Map_Of_Freq.size();
    Encoded_File.write((char*)&Map_Of_Freq_Size, sizeof(Map_Of_Freq_Size));
    for (auto pair : Map_Of_Freq) {

        Encoded_File.write((char*)&pair.first, 1);
        Encoded_File.write((char*)&pair.second, 4);
    }
    Size_After_In_Bits = Encode.length();
    BitArray bits = BitArray(Size_After_In_Bits);
    for (int i = 0; i < Size_After_In_Bits; i++)
    {
        bits.set(i, Encode[i] == '1' ? 1 : 0);
    }
    bits.make_b();
    Encoded_File.write((char*)&Size_After_In_Bits, 4);
    Encoded_File.flush();
    Encoded_File.write(bits.b, bits.Get_Size_In_Bytes());
    Encoded_File.close();
    cout << "Compression is finished" << endl;
    return 0;
}
//Чтение и декодировка данных
int Read_And_Decode(string Encoded_Data_Path, string Decoded_Data_Path)
{
    int Size_Of_Freq_Table;
    char temp_char;
    int temp_int;
    int Data_Size_In_Bits;
    string Encode;
    map<char, int> Map_Of_Freq;

    ifstream Encoded_File(Encoded_Data_Path, ios_base::binary);
    Encoded_File.read((char*)&Size_Of_Freq_Table, 4);
    for (int i = 0; i < Size_Of_Freq_Table; i++) 
    {
        Encoded_File.read((char*)&temp_char, 1);
        Encoded_File.read((char*)&temp_int, 4);
        Map_Of_Freq.emplace(temp_char, temp_int);
    }
    Encoded_File.read((char*)&temp_int, 4);
    
    BitArray bitarray1 = BitArray(temp_int);
    for (int i = 0; i < bitarray1.Get_Size_In_Bytes(); i++)
    {
        Encoded_File.read((char*)&temp_char, 1);
        bitarray1.bytes[i] = temp_char;
    }
    Encoded_File.close();

    for (int i = 0; i < bitarray1.size; i++)
    {
        Encode.append(bitarray1.get(i) == 1 ? "1" : "0");
    }

    vector<Huffman_Node> Vector_Of_Nodes = Make_Vector_Of_Nodes(Map_Of_Freq); 
    Huffman_Node Hufman_Tree = Make_Huffman_Tree(Vector_Of_Nodes); 
    map<char, string> Huffman_Codes = Make_Map_Of_Huffman_Codes(Map_Of_Freq, Hufman_Tree); 
    string Decode1 = Make_Decode_String(Encode, &Hufman_Tree, Map_Of_Freq);

    ofstream file2(Decoded_Data_Path, ios_base::binary);

    for (int i = 0; i < Decode1.length(); i++)
    {
        temp_char = Decode1[i];
        file2.write((char*)&temp_char, 1);
    }
    file2.close();
    cout << endl;
    cout << "Decompression is finished" << endl;
    cout << "Size in bits after decompression: " << Decode1.length()*8 << endl;
    return 0;

}
int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Welcome to Huffman coding application!" << endl;
    cout << "Hello World!" << endl;
    cout << endl;
    cout << endl;
    string Encoded_Data_Path = "D:\\KURS\\test.hfmn";
    string Incoming_Data_Path = "D:\\KURS\\5.5.txt";
    string Decoded_Data_Path = "D:\\KURS\\decoded.txt";
    string s1, s2, s3;
    int mode;

    cout << "Standart directories are: " << endl;
    cout << "Original data: " << Incoming_Data_Path << endl;
    cout << "Compresed data: " << Encoded_Data_Path << endl;
    cout << "Decoded data: " << Decoded_Data_Path<< endl;
    cout << endl;
    cout << "If you dont want to chage directories, write 0" << endl;
    cout << "DONT FORGET TO WRITE DOUBLE SLASH!!!" << endl;
    cout << endl;
    cout << endl;
    cout << "Please, write Original data path: ";
    cin >> s1;
    if (s1 != "0") Incoming_Data_Path = s1;
    cout << endl;

    cout << "Please, write Compresed data path: ";
    cin >> s2;
    if (s2 != "0") Encoded_Data_Path = s2;
    cout << endl;

    cout << "Please, write Decoded data path: ";
    cin >> s3;
    if (s3 != "0") Decoded_Data_Path = s3;
    cout << endl;
    cout << "Choose mod" << endl;
    cout << "1. Compres" << endl;
    cout << "2. Decompres" << endl;
    cin >> mode;
    if(mode ==1 ) Read_And_Code(Incoming_Data_Path, Encoded_Data_Path);
    
    else if(mode ==2) Read_And_Decode(Encoded_Data_Path, Decoded_Data_Path);
    
    return 0;
}

