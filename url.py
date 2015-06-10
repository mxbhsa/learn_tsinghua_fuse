#encoding=utf-8
import sys
reload(sys)
sys.setdefaultencoding('utf8')
import urllib2
import urllib
import cookielib
from HTMLParser import HTMLParser
from htmlentitydefs import entitydefs

class courseHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.courseGet = False
        self.links = []
        self.names = []
	
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "a":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "href" and value.find('course_id') != -1):
                        self.links.append(value[value.find('course_id')+10:])
                        self.courseGet = True
                        
    def handle_data(self,data):
		if(self.courseGet):
		    self.names.append(data.strip().replace('/',' '))
		    self.names.append(self.links.pop().strip())
	#	    self.courseGet = False
        
    def handle_endtag(self, tag):
        if tag == "a":
            self.courseGet = False


class downloadHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.courseGet = False
        self.inCourse = False
        self.links = []
        self.names = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        self.courseGet = False
        self.inCourse = False
        if tag == "a":
            isDownload = False
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "target" and value == "_top"):
                        isDownload = True
                if (not isDownload):
                    return
                for (variable, value)  in attrs:
                    if (variable == "href"):
                        self.links.append('http://learn.tsinghua.edu.cn' + value)
                        self.courseGet = True
                        
    def handle_data(self,data):
        if(self.courseGet and len(data.strip())):
            #print data.strip()
            #print self.names
            if self.inCourse :
                self.names.append(self.names.pop() +data.strip().replace('/',' '))
            else:
                self.names.append(data.strip().replace('/',' '))
            self.inCourse = True
            
    
    def handle_entityref(self,name): 
        if self.courseGet :
            if(self.inCourse):
                fuck = self.names.pop() + ' '
            self.names.append(fuck)


class infoHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.courseGet = False
        self.links = []
        self.names = ''
	
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td":
            self.courseGet = True
        if tag == 'p' and self.courseGet :
            self.courseGet = True
    
    
    def handle_endtag(self, tag):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td" and self.courseGet:
            self.courseGet = False
            self.names += ('\n')
                        
    def handle_data(self,data):
		if(self.courseGet):
		    self.names += (data.strip())
    
    

class noteListHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.noteGet = False
        self.inNote = False
        self.links = []
        self.names = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "a":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "href"):
                        self.links.append('http://learn.tsinghua.edu.cn/MultiLanguage/public/bbs/' + value)
                        self.noteGet = True
    
    def handle_endtag(self, tag):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "a" and self.noteGet:
            self.noteGet = False
            self.inNote = False
    
    def handle_data(self,data):

        if(self.noteGet and len(data.strip())):
            #print data.replace('/',' ')
            if self.inNote :
                self.names.append(self.names.pop() +data.strip().replace('/',' '))
            else:
                self.names.append(data.strip().replace('/',' '))
            self.inNote = True
            
    
    def handle_entityref(self,name): 
        if self.noteGet :
            if(self.inNote):
                if name == 'nbsp':
                    fuck = self.names.pop() + ' '
                elif entitydefs.has_key(name):
                    fuck = self.names.pop() + entitydefs[name]
                else:
                    fuck = self.names.pop() + ' '
                self.names.append(fuck)
    


class noteHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.courseGet = False
        self.links = []
        self.names = ''
	
 
    def handle_starttag(self, tag, attrs):
        #if(ord(tag[0]) > 128):
         #   return
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td":
            self.courseGet = True
        if tag == 'div' and self.courseGet :
            self.courseGet = True
        if tag == 'p' and self.courseGet :
            self.courseGet = True
    
    
    def handle_endtag(self, tag):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td" and self.courseGet:
            self.courseGet = False
            self.names += '\n'
                        
    def handle_data(self,data):
		if(self.courseGet):
		    self.names += (data.strip())
    
    def handle_entityref(self,name): 
        if self.courseGet :
            if name == 'nbsp':
                    self.names += ' '
            elif entitydefs.has_key(name):
                self.names += entitydefs[name]
            else:
                self.names += ' '
                

class homeworkListHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.noteGet = False
        self.inNote = False
        self.links = []
        self.names = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "a":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "href"):
                        self.links.append('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/' + value)
                        self.noteGet = True
    
    def handle_endtag(self, tag):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "a" and self.noteGet:
            self.noteGet = False
            self.inNote = False
    
    def handle_data(self,data):

        if(self.noteGet and len(data.strip())):
            #print data.replace('/',' ')
            if self.inNote :
                self.names.append(self.names.pop() +data.strip().replace('/',' '))
            else:
                self.names.append(data.strip().replace('/',' '))
            self.inNote = True
            
    
    def handle_entityref(self,name): 
        if self.noteGet :
            if(self.inNote):
                if name == 'nbsp':
                    fuck = self.names.pop() + ' '
                elif entitydefs.has_key(name):
                    fuck = self.names.pop() + entitydefs[name]
                else:
                    fuck = self.names.pop() + ' '
                self.names.append(fuck)
    


class homeworkHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.courseGet = False
        self.links = []
        self.names = ''
	
 
    def handle_starttag(self, tag, attrs):
        #if(ord(tag[0]) > 128):
         #   return
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td":
            self.courseGet = True
        if tag == 'div' and self.courseGet :
            self.courseGet = True
        if tag == 'p' and self.courseGet :
            self.courseGet = True
    
    
    def handle_endtag(self, tag):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "td" and self.courseGet:
            self.courseGet = False
            self.names += '\n'
                        
    def handle_data(self,data):
		if(self.courseGet):
		    self.names += (data.strip())
    
    def handle_entityref(self,name): 
        if self.courseGet :
            if name == 'nbsp':
                    self.names += ' '
            elif entitydefs.has_key(name):
                self.names += entitydefs[name]
            else:
                self.names += ' '
                

class recidHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.noteGet = False
        self.inNote = False
        self.post_rec_id = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "input":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "value" and value == "提交作业"):
                        for (variable, value)  in attrs:
                            if(variable == "onclick"):
                        #确认
                                rec_id_pos = value.find('?id=');
                                print value[rec_id_pos+4:rec_id_pos+10]
                                self.post_rec_id.append(value[rec_id_pos+4:rec_id_pos+10])

####################################################################################



def getUserFile(url,user,password):
    login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
    try:
        cj = cookielib.CookieJar()
        #cookieJar作为参数，获得一个opener的实例
        opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
        opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)')]
        data = urllib.urlencode({"userid":user,"userpass":password})
        opener.open(login_page,data)
        op=opener.open(url)
        return op
    except Exception,e:
        print str(e)

def getLoginOp(user, password):
    login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
    cj = cookielib.CookieJar()
    opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)')]
    data = urllib.urlencode({"userid":user,"userpass":password})
    opener.open(login_page,data)
    return opener
    

def parseCourseNoteList(path,courseid,user,password):
    opener = getLoginOp(user,password)
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/public/bbs/getnoteid_student.jsp?course_id='+courseid)

    data= op.read()
    
    hp = noteListHTMLParser()
    hp.feed(data)
    hp.close()
    ans = []
    ii = 0
    print 'read note list python'
    for i in hp.names:
        print i
        ans.append(path+i)
        ans.append(i)
        f = open(path+i,'w')
        hp2 = noteHTMLParser()
        op=opener.open(hp.links[ii])
        data= op.read()
        hp2.feed(data)
        print hp2.names
        ii += 1
        f.write(hp2.names)     
        ans.append(len(hp2.names))   
        hp2.close()
    return ans
    

def parseHomeworkList(path,courseid,user,password):
    opener = getLoginOp(user,password)
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_brw.jsp?course_id='+courseid)

    data= op.read()
    
    hp = homeworkListHTMLParser()
    hp.feed(data)
    hp.close()
    rechp = recidHTMLParser()
    rechp.feed(data)
    rechp.close()
    ans = []
    ii = 0
    print 'read homework list python'
    for i in hp.names:
        #print path+i
        ans.append(path+i)
        ans.append(i)
        f = open(path+i,'w')
        
        hp2 = homeworkHTMLParser()
        op=opener.open(hp.links[ii])
        data= op.read()
        hp2.feed(data)
        #print hp2.names
        f.write(hp2.names)     
        ans.append(len(hp2.names)) 
        ans.append(rechp.post_rec_id[ii])
        hp2.close()
        ii += 1
    print ans
    return ans

def parseCourseInfo(courseid,user,password):
    opener = getLoginOp(user,password)
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/course_info.jsp?course_id='+courseid)

    data= op.read()
    
    hp = infoHTMLParser()
    hp.feed(data)
    hp.close()
    ans = []
    ans.append(len(hp.names))
    ans.append(hp.names)
    #print ans
    return ans

def parseCourse(url,user,password):
    page = getUserPage(url,user,password)
    hp = courseHTMLParser()
    hp.feed(page)
    hp.close()
    #print(hp.names)
    return hp.names

'''
def getFileTypeSize(url,user,password):
    u = getUserFile(url,user,password)  
    meta = u.info()  
    file_type = '.'+(meta.getheaders("Content-Disposition")[0].split('.')[-1].split('"')[0])
    print filetype
    file_size = int(meta.getheaders("Content-Length")[0]) 
    print file_size
    ans = []
    ans.append(file_type)
    ans.append(file_size)
    return ans
''' 
    
def downloadFile(path,url,user,password):
    #file_name = url.split('/')[-1]  
    u = getUserFile(url,user,password)  
    f = open(path, 'wb')  
    meta = u.info()  
    file_size = int(meta.getheaders("Content-Length")[0])  
    print file_size
    file_size_dl = 0  
    block_sz = 8192  
    while True:  
        buffer = u.read(block_sz)  
        if not buffer:  
            break  
      
        file_size_dl += len(buffer)  
        f.write(buffer)  
    f.close()
    print '下载完成！！！！！！！！！'
    return path


def parseDownload(courseid,user,password):
    opener = getLoginOp(user,password)
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/download.jsp?course_id='+courseid)

    data= op.read()
    
    hp = downloadHTMLParser()
    hp.feed(data)
    hp.close()
    ans = []
    ii = 0
    
    for i in hp.names:
        print i
        ans.append(hp.links[ii])
        
        u = opener.open(hp.links[ii])
        meta = u.info()  
        file_type = '.'+(meta.getheaders("Content-Disposition")[0].split('.')[-1].split('"')[0])
        file_size = int(meta.getheaders("Content-Length")[0]) 
        
        ans.append(file_type)
        ans.append(file_size)
        ans.append(i+file_type)
        ii += 1
    for i in ans:
        print i
    print len(hp.names)
    return ans
	

def getUserPage(url,user,password):
    #登陆页面，可以通过抓包工具分析获得，如fiddler，wireshark
    login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
    #try:
    #获得一个cookieJar实例
    cj = cookielib.CookieJar()
    #cookieJar作为参数，获得一个opener的实例
    opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    #伪装成一个正常的浏览器，避免有些web服务器拒绝访问。
    opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)')]
    #生成Post数据，含有登陆用户名密码。
    data = urllib.urlencode({"userid":user,"userpass":password})
    #以post的方法访问登陆页面，访问之后cookieJar会自定保存cookie
    opener.open(login_page,data)
    #以带cookie的方式访问页面
    op=opener.open(url)
    #读取页面源码
    data= op.read()

    #print data
    return data
    #except Exception,e:
     #   print str(e)
#访问某用户的个人主页，其实这已经实现了人人网的签到功能。
if __name__ == '__main__':
#fusermount -u /home/mxb/fuse
#./hello /home/mxb/fuse -d
    #f = open('info.txt', 'w') 
    parseHomeworkList('./test/','124097','ma-xb12',"Tsinghua2012")
    #f.write(a)
    #getUserPage('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/MyCourse.jsp?language=cn','ma-xb12','Tsinghua2012');
    #getFileTypeSize('http://learn.tsinghua.edu.cn/uploadFile/downloadFile_student.jsp?module_id=322&filePath=OcwlnZxhKf3pBMMAL86yYqQ/rkOtMMivFCRg9evL3Nyk4uDyy61qw0815PWjIRoxRs2i15DQ5DIsE7ZaBEzlwKfAfbhdnycEzv33oiTB78s%3D&course_id=124095&file_id=1327082','ma-xb12','Tsinghua2012')
	#fileHandle = open ( 'lear.html', 'w') 
	#parseCourse("http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/MyCourse.jsp?language=cn","ma-xb12","Tsinghua2012")
