#!/usr/bin/env python
# coding=utf-8

import urllib2    #自带的库
# 1、获取主页面内容
def OpenPage(url):
    '''
    根据url构造HTTP请求发送给服务器
    获取到HTTP服务器的响应
    '''
    # 为了构造请求对象，构造一个header对象
    # headers = {}
    # 构造请求对象
    req = urllib2.Request(url)
    # 发送HTTP请求，获取到一个文件对象
    f = urllib2.urlopen(req)
    # 从文件中读取返回结果
    data = f.read()
    return data

def Test1():
    url = 'http://www.shengxu6.com/book/2967.html'
    print OpenPage(url)

if __name__ == '__main__'
