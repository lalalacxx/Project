# coding:utf-8
from bs4 import BeautifulSoup
import urllib2
import json
import MySQLdb
import base64


def OpenPage(url):
    headers = {
        'User-Agent': 'xxxxx',
    }
    req = urllib2.Request(url)
    f = urllib2.urlopen(req)
    return f.read()


def Test1():
    url = 'http://jy.51uns.com:8022/Pro_StudentEmploy/StudentJobFair/Zhaoping.aspx?WorkType=0'
    html = OpenPage(url)
    print html


def ParseMainPage(page):
    '''
    本函数的目标是为了获取到所有的招聘信息对应的 id
    得到了 id 之后, 才能拼装出详细页的 url
    解析得到的 json 结构, 遍历其中的结果集合把 id 解析出来
    就可以构造 url
    '''
    # json.loads 将 page 这个字符串转换成了 python 中的字典结构
    data = json.loads(page)
    rows = data['rows']
    url_prefix = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535006490883&fn=GetOneZhaopin&StartDate=2000-01-01&JobId='
    return [url_prefix + row['Id'] for row in rows]



def Test2():
    '''
    验证招聘信息的详情页是否能正确获取
    '''
    url = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535006490883&fn=GetOneZhaopin&StartDate=2000-01-01&JobId=6c486989f5424026a214c074cf038f3d'
    print OpenPage(url)


def ParseDetailPage(page):
    '''
    关注的信息：
    公司名称->CompanyTitle
    薪酬待遇->WorkPrice
    工作岗位->WorkPositon
    工作职责->EmployContent
    '''
    data = json.loads(page)
    data = data['Data']
    content = data['EmployContent']
    soup = BeautifulSoup(content, 'html.parser')
    result = soup.find_all('p')
    text = '\n'.join([item.get_text() for item in result])
    return (data['CompanyTitle'], data['WorkPrice'],
            data['WorkPositon'], text)


# 详情页需要通过这个 url 来获取到
# http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535006490883&fn=GetOneZhaopin&JobId=b7986ae4f3f94e0bb79677c68b871e30&StartDate=2000-01-01

# 数据库操作需要安装两个第三方的模块
# yum install python-devel  python 的开发工具包
# pip install mysql-python  python2 之中访问 MySQL 开发工具包
def WriteData(data):
    '''
    把 data 数据写到数据库中
    data 是一个四元组. 四元组中包含
    公司名称, 薪水, 岗位, 职责

    ORM 基于面向对象的思想来操作数据库. 创建一个对象和数据库
    的某个表关联到一起, 修改对象中字段的值, 就会同时修改数据库
    '''
    #1. 和数据库建立链接
    db = MySQLdb.connect(host='127.0.0.1', user='root', passwd='961120',
                    db='JobInfo', charset='utf8')
    #2. 构造 cursor 对象
    cursor = db.cursor()
    #3. 构造 SQL 语句
    #由于 SQL 中如果包含了一些特殊字符(例如 单引号), 
    #可能会导致 SQL 语句解析出错. 需要对字符串进行 base64 编码
    content = base64.b64encode(data[3])
    sql = "insert into CrawlerSchool values('%s', '%s', '%s', '%s')" % (
        data[0], data[1], data[2], content)
    print 'sql =', sql
    #4. 执行 SQL 语句
    cursor.execute(sql)
    #5. 把请求提交到数据库服务器
    db.commit()
    #6. 关闭数据库连接
    db.close()


def Run():
    '''
    整个程序的入口
    '''
    # 1. 获取到主页内容

    # 2. 根据主页内容解析所有详情页的 url
    # 3. 遍历所有详情页的url
    # 4. 获取到每个详情页的内容, 并解析
    # 5. 把解析结果写到数据库

if __name__ == '__main__':
    # Test1()
    # Test2()
