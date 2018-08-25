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
    //ѹ���ӿ�
	void Compress(const char* file)
	{
		// 1.ͳ���ļ����ַ����ֵĴ���
        std::ifstream ifs(file);
		char ch;
		while (ifs >> ch)
		{
			_infos[ch]._count++;
		}

		// 2.����Huffman��
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> t(_infos, 256, invalid);

		// 3.����Huffman����
		GetHuffmanCode(t.GetRoot());


		// 4.ѹ��
        std::string compressfile = file;
		compressfile += ".huffman";
        std::ofstream ofs(compressfile.c_str());
        
        //д���ַ����ֵĴ����������ѹʱ�ؽ�Huffman��
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
        //�ص��ļ�ĳһ���������0�ͱ�ʾ�ص��ļ����ʼ
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
    //����Huffman����
	void GetHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;

		if (root->_left == NULL && root->_right == NULL)
		{
            //�ߵ������˵���ߵ���ĳһ��Ҷ�ӽڵ�
            std::string& code = _infos[root->_w._ch]._code;
			Node* cur = root;
			Node* parent = cur->_parent;
			while (parent)
			{
				if (cur == parent->_left)
					code += '0';
				else
					code += '1';
                //������
				cur = parent;
				parent = parent->_parent;
			}

			reverse(code.begin(), code.end());
			return;
		}

		GetHuffmanCode(root->_left);
		GetHuffmanCode(root->_right);
	}

    //��ѹ�ӿ�
	void Uncompress(const char* file)
    {
        //�򿪲���ȡѹ���ļ�
        std::ifstream ifs(file);
        //ȥ��ѹ���ļ��ĺ�׺��
        //���Ϊ��ѹ�ļ����ļ���
        std::string unfile = file;
        size_t pos = unfile.find('.');
        if(pos != std::string::npos)
        {
            unfile.erase(pos);
        }
        unfile += ".unhuffman";
        std::ofstream ofs(unfile.c_str());
        //�����ַ����ֵĴ���
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


        //��ѹʱ�ؽ�Huffman��
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
