#pragma once
#include<fstream>
#include<string>
#include<algorithm>
#include "HuffmanTree.h"

typedef long long LongType;
struct CharInfo
{
	char _ch;
	LongType _count;
	string _code;

	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;

		return ret;
	}

	bool operator >(const CharInfo& info)
	{
		return _count > info._count;
	}

	bool operator != (const CharInfo& info)
	{
		return _count != info._count;
	}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}

	void Compress(const char* file)
	{
		// 1.统计文件中字符出现的次数
		ifstream ifs(file);
		char ch;
		while (ifs >> ch)
		{
			_infos[ch]._count++;
		}

		// 2.构建HuffmanTree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> t(_infos, 256, invalid);

		// 3.生成Huffman code
		GetHuffmanCode(t.GetRoot());

		// 4.压缩
		string compressfile = file;
		compressfile += ".huffman";
		ofstream ofs(compressfile.c_str());

		ifs.clear();
		ifs.seekg(0);
		char value = 0;
		int pos = 0;
		while (ifs >> ch)
		{
			string& code = _infos[ch]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '1')
				{
					value |= (1 << pos);
				}
				else if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}
				else
				{
					//assert(false);
				}
				++pos;

				if (pos == 8)
				{
					ofs << value;
					pos = 0;
					value = 0;
				}
			}
		}
	}

	void GetHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;

		if (root->_left == NULL
			&& root->_right == NULL)
		{
			string& code = _infos[root->_w._ch]._code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (cur == parent->_left)
					code += '0';
				else
					code += '1';

				cur = parent;
				parent = parent->_parent;
			}

			reverse(code.begin(), code.end());
			//_infos[root->_w._ch]._code = code;
			return;
		}

		GetHuffmanCode(root->_left);
		GetHuffmanCode(root->_right);
	}

	void Uncompress(const char* file);
private: 
	CharInfo _infos[256];
};

void TestCompress()
{
	FileCompress fc;
	fc.Compress("input.txt");
}
