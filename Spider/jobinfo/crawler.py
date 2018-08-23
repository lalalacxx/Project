#!/usr/bin/env python
# coding=utf-8

import urllib2
import json
from bs4 import BeautifulSoup

def OpenPage(url):
    req = urllib2.Request(url)
    f = urllib2.urlopen(req)
    return f.read()
def Test1():
    url = 'http://jy.51uns.com:8022/Pro_StudentEmploy/StudentJobFair/ZhaoPing.aspx?WorkType=0'
    html = OpenPage(url)
    print html


def ParseMainPage(page):
    '''
    该函数是为了获取到所有的招聘信息对应的id
    得到了ID之后，才能拼装成完整的
    解析得到的json结构，遍历其中的结果集合把id解析出来

    '''
    # json.loads把page这个字符串转换成了Python中的字典结构
    data = json.loads(page)
    rows = data['rows']
    url_perix = 
    return [row['Id'] for row in rows]

def ParseDetailPage(page):
    '''
    关心的信息：公司名称（CompanyTitle），薪资(WorkPrice)，
    工作岗位（WorkPosition）,工作职责（EmployContent）
    '''
    date = json.loads(page)
    data = data['Data']
    content = data['EmployContent']
    soup = BeautifulSoup(content,'html.parser')
    result = soup.find_all('p')
    text = '\n'.join([item.get_text() for item in result])
    return data['CompanyTitle'],data['WorkPrice'],data['WorkPosition'],data['EmployContent'],content
def Test3():
    url = ''
    page = OpenPage(url)
    result = ParseDetailPage(page)
    print result[0],result[1],result[2],result[3]





































if __name__ = '__main__':
    Test1()

