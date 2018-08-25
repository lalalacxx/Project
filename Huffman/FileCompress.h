#pragma once
#include<fstream>
#include<string>
#include<assert.h>
#include<algorithm>
#include "HuffmanTree.h"

typedef long long LongType;
struct CharInfo
{
	char _ch;
	LongType _count;
    std::string _code;

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
    struct TmpInfo
    {
        char _ch;
        LongType _count;
    };

public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}
    //压缩接口
	void Compress(const char* file)
	{
		// 1.统计文件中字符出现的次数
        std::ifstream ifs(file);
		char ch;
		while (ifs >> ch)
		{
			_infos[ch]._count++;
		}

		// 2.构建Huffman树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> t(_infos, 256, invalid);

		// 3.生成Huffman编码
		GetHuffmanCode(t.GetRoot());


		// 4.压缩
        std::string compressfile = file;
		compressfile += ".huffman";
        std::ofstream ofs(compressfile.c_str());
        
        //写入字符出现的次数，方便解压时重建Huffman树
        /*for(size_t i = 0;i < 256;i++)
        {
            if(_infos[i]._count > 0)
            {
                ofs << _infos[i]._ch<<std::endl;
                ofs<<_infos[i]._count<<std::endl;
            }
        }
        CharInfo info;
        info._ch = '0';
        info._count = 0;
        ofs<<info._ch<<std::endl;
        ofs<<info._count<<std::endl;*/
        for(size_t i = 0;i < 256;i++)
        {
            if(_infos[i]._count > 0)
            {
                TmpInfo info;
                info._ch = _infos[i]._ch;
                info._count = _infos[i]._count;

                ofs.write((char*)&info,sizeof(TmpInfo));
            }
        }
        TmpInfo info;
        info._count = 0;
        ofs.write((char*)&info,sizeof(TmpInfo));

		ifs.clear();
        //回到文件某一处，这里的0就表示回到文件的最开始
		ifs.seekg(0);
		char value = 0;
		int pos = 0;
		while (ifs >> ch)
		{
            std::string& code = _infos[ch]._code;
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
					assert(false);
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
        if(pos > 0)
        {
            ofs << value;
        }
	}
    //生成Huffman编码
	void GetHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;

		if (root->_left == NULL && root->_right == NULL)
		{
            //走到这里就说明走到了某一个叶子节点
            std::string& code = _infos[root->_w._ch]._code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (cur == parent->_left)
					code += '0';
				else
					code += '1';
                //倒着走
				cur = parent;
				parent = parent->_parent;
			}

			reverse(code.begin(), code.end());
			return;
		}

		GetHuffmanCode(root->_left);
		GetHuffmanCode(root->_right);
	}

    //解压接口
	void Uncompress(const char* file)
    {
        //打开并读取压缩文件
        std::ifstream ifs(file);
        //去掉压缩文件的后缀名
        //则就为解压文件的文件名
        std::string unfile = file;
        size_t pos = unfile.find('.');
        if(pos != std::string::npos)
        {
            unfile.erase(pos);
        }
        unfile += ".unhuffman";
        std::ofstream ofs(unfile.c_str());
        //读入字符出现的次数
        /*char ch;
        LongType count;
        do
        {
            ifs>>ch;
            ifs>>count;
            if(count > 0)
            {
                _infos[ch]._count = count;
            }
        }while(count > 0);*/
        TmpInfo info;
        do
        {   
            ifs.read((char*)&info,sizeof(TmpInfo));
            if(info._count > 0)
            {
                _infos[info._ch]._count = info._count;
            }
        }while(info._count > 0);


        //解压时重建Huffman树
        CharInfo invalid;
        invalid._count = 0;
        HuffmanTree<CharInfo> t(_infos,256,invalid);
        Node* root = t.GetRoot();
        LongType filesize = root->_w._count;

        char value;
        Node* cur = root;
        while(ifs >> value)
        {
            for(size_t i = 0;i < 8;i++)
            {
                if(value & (1<<i))  //  '1'
                {
                    cur = cur->_right;
                }
                else
                {
                    cur = cur->_left;
                }
                if(cur->_left == NULL && cur->_right == NULL)
                {
                    ofs << cur->_w._ch;
                    cur = root;
                    if(--filesize == 0)
                    {
                        return;
                    }
                }
            }
        }
    }
private: 
	CharInfo _infos[256];
};

void TestCompress()
{
	FileCompress fc;
	fc.Compress("input.txt");
}
void TestUncompress()
{
	FileCompress fc;
	fc.Uncompress("input.txt.huffman");
}
