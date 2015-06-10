#encoding=utf-8
import sys
import random 
import time
reload(sys)
sys.setdefaultencoding('utf8')
import urllib2
import urllib
import cookielib
from HTMLParser import HTMLParser
from htmlentitydefs import entitydefs
#import poster
import os
import httplib2 
#from poster.encode import multipart_encode  
#from poster.streaminghttp import StreamingHTTPHandler, StreamingHTTPRedirectHandler, StreamingHTTPSHandler  


'''
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
'''   


class recidHTMLParser(HTMLParser):
    def __init__(self, _submit_id):
        HTMLParser.__init__(self)
        self.noteGet = False
        self.inNote = False
        self.post_rec_id = ''
        self.submit_id = _submit_id
        self.links = []
        self.names = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "input":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == "disabled" and value == "true"):
                        return
                for (variable, value)  in attrs:
                    if(variable == "onclick" and self.submit_id in value):
                #确认
                        rec_id_pos = value.find('rec_id=');
                        self.post_rec_id = value[rec_id_pos+7:rec_id_pos+14]


class postInfoHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.noteGet = False
        self.inNote = False
        self.post_rec_id = ''
        self.saveDir = ''
        self.old_filename = ''
        self.newfilename = ''
        self.post_homewk_link = ''
        self.submit_id = ''
        self.links = []
        self.names = []
 
    def handle_starttag(self, tag, attrs):
        #print "Encountered the beginning of a %s tag" % tag
        if tag == "input":
            if len(attrs) == 0: pass
            else:
                for (variable, value)  in attrs:
                    if (variable == 'name' and value == "saveDir"):
                        for (variable, value)  in attrs:
                            if (variable == "value"):
                                self.saveDir = value
                                return
                    elif(variable == 'name' and value == "old_filename"):
                        for (variable, value)  in attrs:
                            if (variable == "value"):
                                self.old_filename = value
                                return
                    elif(variable == 'name' and value == "newfilename"):
                        for (variable, value)  in attrs:
                            if (variable == "value"):
                                self.newfilename = value
                                return
                    elif(variable == 'name' and value == "post_homewk_link"):
                        for (variable, value)  in attrs:
                            if (variable == "value"):
                                self.post_homewk_link = value
                                return
                    elif(variable == 'name' and value == "submit_id"):
                        for (variable, value)  in attrs:
                            if (variable == "value"):
                                self.submit_id = value
                                return
    
def get_mime_type(filename):   #filename 文件路径

    #返回文件路径后缀名
    filename_type = os.path.splitext(filename)[1][1:]
    type_list = {
                'html'  :       'text/html',
                'htm'   :       'text/html',
                'shtml' :       'text/html',
                'css'   :       'text/css',
                'xml'   :       'text/xml',
                'gif'   :       'image/gif',
                'jpeg'  :       'image/jpeg',
                'jpg'   :       'image/jpeg',
                'js'    :       'application/x-javascript',
                'atom'  :       'application/atom+xml',
                'rss'   :       'application/rss+xml',
                'mml'   :       'text/mathml',
                'txt'   :       'text/plain',
                'jad'   :       'text/vnd.sun.j2me.app-descriptor',
                'wml'   :       'text/vnd.wap.wml',
                'htc'   :       'text/x-component',
                'png'   :       'image/png',
                'tif'   :       'image/tiff',
                'tiff'  :       'image/tiff',
                'wbmp'  :       'image/vnd.wap.wbmp',
                'ico'   :       'image/x-icon',
                'jng'   :       'image/x-jng',
                'bmp'   :       'image/x-ms-bmp',
                'svg'   :       'image/svg+xml',
                'svgz'  :       'image/svg+xml',
                'webp'  :       'image/webp',
                'jar'   :       'application/java-archive',
                'war'   :       'application/java-archive',
                'ear'   :       'application/java-archive',
                'hqx'   :       'application/mac-binhex40',
                'doc'   :       'application/msword',
                'pdf'   :       'application/pdf',
                'ps'    :       'application/postscript',
                'eps'   :       'application/postscript',
                'ai'    :       'application/postscript',
                'rtf'   :       'application/rtf',
                'xls'   :       'application/vnd.ms-excel',
                'ppt'   :       'application/vnd.ms-powerpoint',
                'wmlc'  :       'application/vnd.wap.wmlc',
                'kml'   :       'application/vnd.google-earth.kml+xml',
                'kmz'   :       'application/vnd.google-earth.kmz',
                '7z'    :       'application/x-7z-compressed',
                'cco'   :       'application/x-cocoa',
                'jardiff'       :       'application/x-java-archive-diff',
                'jnlp'  :       'application/x-java-jnlp-file',
                'run'   :       'application/x-makeself',
                'pl'    :       'application/x-perl',
                'pm'    :       'application/x-perl',
                'prc'   :       'application/x-pilot',
                'pdb'   :       'application/x-pilot',
                'rar'   :       'application/x-rar-compressed',
                'rpm'   :       'application/x-redhat-package-manager',
                'sea'   :       'application/x-sea',
                'swf'   :       'application/x-shockwave-flash',
                'sit'   :       'application/x-stuffit',
                'tcl'   :       'application/x-tcl',
                'tk'    :       'application/x-tcl',
                'der'   :       'application/x-x509-ca-cert',
                'pem'   :       'application/x-x509-ca-cert',
                'crt'   :       'application/x-x509-ca-cert',
                'xpi'   :       'application/x-xpinstall',
                'xhtml' :       'application/xhtml+xml',
                'zip'   :       'application/zip',
                'py'    :       'application/x-python-script',
                'bin'   :       'application/octet-stream',
                'exe'   :       'application/octet-stream',
                'dll'   :       'application/octet-stream',
                'deb'   :       'application/octet-stream',
                'dmg'   :       'application/octet-stream',
                'eot'   :       'application/octet-stream',
                'iso'   :       'application/octet-stream',
                'img'   :       'application/octet-stream',
                'msi'   :       'application/octet-stream',
                'msp'   :       'application/octet-stream',
                'msm'   :       'application/octet-stream',
                'mid'   :       'audio/midi',
                'midi'  :       'audio/midi',
                'kar'   :       'audio/midi',
                'mp3'   :       'audio/mpeg',
                'ogg'   :       'audio/ogg',
                'm4a'   :       'audio/x-m4a',
                'ra'    :       'audio/x-realaudio',
                '3gpp'  :       'video/3gpp',
                '3gp'   :       'video/3gpp',
                'mp4'   :       'video/mp4',
                'mpeg'  :       'video/mpeg',
                'mpg'   :       'video/mpeg',
                'mov'   :       'video/quicktime',
                'webm'  :       'video/webm',
                'flv'   :       'video/x-flv',
                'm4v'   :       'video/x-m4v',
                'mng'   :       'video/x-mng',
                'asx'   :       'video/x-ms-asf',
                'asf'   :       'video/x-ms-asf',
                'wmv'   :       'video/x-ms-wmv',
                'avi'   :       'video/x-msvideo'
                }
    #判断数据中是否有该后缀名的 key
    if ( filename_type in type_list.keys() ):
        return  type_list[filename_type]
    else:
        return  ''
#该片段来自于http://outofmemory.cn
    


#########################################################################

def submit(submit_id, old_filename,post_rec_id, local_filepath, course_id,userid,userpass):
    local_filename = local_filepath.split('/')[-1]
    '''
    if submit_id == 1:
        submit_id = '641103'
        old_filename = '/124095/homewkrec/2012011402_641103_85909454_HW1.docx'
        post_rec_id = '4062704'
        post_id = submit_id
        local_filename = "url2.py"
        course_id = '124095'
        userid = '2012011402'
        userpass = 'Tsinghua2012'
    '''
    print 'over1'
    newfilename = userid+'_'+submit_id+'_'
    newfullfilenamep1 = newfilename+str(int(random.random()*10000))
    newfullfilenamep2 = str(int(random.random()*10000))+'_'+local_filename

    boundary = '----WebKitFormBoundary%s' % hex(long(time.time() * 100000))[2:]
    data = []
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'saveDir')
    data.append('%s' % course_id+'/homewkrec/')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'filename')
    data.append('%s' % newfullfilenamep1+newfullfilenamep2)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'old_filename')
    data.append('%s' % old_filename)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'errorURL')
    data.append('%s' % '/uperror.jsp?error=')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'returnURL')
    data.append('%s' % '/win_close.jsp')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'newfilename')
    data.append('%s' % newfilename)
    data.append('--%s' % boundary)


    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'post_id')
    data.append('%s' % submit_id)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'post_rec_id')
    data.append('%s' % post_rec_id)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'post_homewk_link')
    data.append('%s' % newfullfilenamep1+'0'+newfullfilenamep2)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'file_unique_flag')
    data.append('%s' % '0')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'url_post')
    data.append('%s' % '/MultiLanguage/lesson/student/hom_wk_handin.jsp')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'css_name')
    data.append('%s' % '')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'tmpl_name')
    data.append('%s' % '')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'course_id')
    data.append('%s' % course_id)
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'module_id')
    data.append('%s' % '325')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'post_rec_homewk_detail')
    data.append('%s' % 'from python')
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"; filename="%s"' % ('upfile',local_filename))
    data.append('Content-Type: %s\r\n' % get_mime_type(local_filename))
    fr=open(local_filepath,'rb')
    filecontent = fr.read()
    data.append(filecontent)
    fr.close()
    data.append('--%s' % boundary)

    data.append('Content-Disposition: form-data; name="%s"\r\n' % 'Submit')
    data.append('%s' % '提交')
    data.append('--%s--\r\n' % boundary)

    submit_url='http://learn.tsinghua.edu.cn/uploadFile/uploadFile.jsp'
    handin_url = 'http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_handin.jsp'
    http_body='\r\n'.join(data)
    #try:
    #header
    #req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % boundary)

    print 'over2'


    #post data to server
    login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
    print 'over3'
    #cookieJar作为参数，获得一个opener的实例
    http = httplib2.Http()
    #opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)'),('Referer', 'http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_submit.jsp?id='+submit_id+'&course_id='+course_id+'&rec_id='+post_rec_id),('Content-Type', 'multipart/form-data; boundary=%s' % boundary)]
    logindata = {"userid":userid,"userpass":userpass}
    login_header = {'Content-type': 'application/x-www-form-urlencoded','User-agent':'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)'}
    logged_response, content = http.request(login_page, 'POST', headers=login_header, body=urllib.urlencode(logindata))
    #'Cookie': logged_response['set-cookie']
    print 'over4'
    upload_header = {   'Content-Type':'multipart/form-data; boundary=%s' % boundary, \
                        'User-agent':'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)', \
                        'Connection': 'keep-alive' ,\
                        'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',\
                        'Origin':'http://learn.tsinghua.edu.cn',\
                        'Accept-Language': 'zh-CN,zh;q=0.8,en;q=0.6',\
                        'Accept-Encoding':'gzip, deflate',\
                        'Cache-Control': 'max-age=0',\
                        'Refer':'http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_submit.jsp?id='+submit_id+'&course_id='+course_id+'&rec_id='+post_rec_id,\
                        'Cookie': logged_response['set-cookie']
                    }
    #response, content = http.request(submit_url, 'POST', headers=upload_header, body=http_body)
    print 'over5'
    #req=urllib2.Request(submit_url, data=http_body)
    #req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % boundary)
    #header
    '''
    req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % boundary)
    req.add_header('User-Agent','Mozilla/5.0')
    req.add_header('Connection', 'keep-alive')
    req.add_header('Accept','text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8')
    req.add_header('Origin','http://learn.tsinghua.edu.cn')
    req.add_header('Accept-Language', 'zh-CN,zh;q=0.8,en;q=0.6')
    req.add_header('Accept-Encoding','gzip, deflate')
    req.add_header('Cache-Control', 'max-age=0')
    req.add_header('Refer','http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_submit.jsp?id='+submit_id+'&course_id='+course_id+'&rec_id='+recidhp.post_rec_id)
    #req.add_header('Cookie','_ga=GA1.3.1793295275.1416054436; JSESSIONID=abcPZLm_7--lq-bdz-_2u; THNSV2COOKIE=YmY1NzcxNTdiZWI2MTdjM2YwNzQ1YmY1NWJkNDZlZDZgMjAxMjAxMTQwMmDC7c.+sfJgU2A1OS42Ni4xMzAuMTlgMjY3ODA2Njc1YDE0MzM0ODE3NDkyNThgMDFaQjk2NTI1SFdLMjhIODI0UUNMOFdIYA--; THNSV2APACHECOOKIE=a881ef7cff3ec3692c7ae3d501b4427bMTQzMzQ4MTc0OXwyMDEyMDExNDAyfFN8NTkuNjYuMTMwLjE5fFV8L2hvbWUvcmVzaW5hcHAvd2x4dC9odGRvY3MvQ3VyVmVyc2lvbi9BcHBGaWxlL2tlamlhbi9kYXRhLzEyNDA5NS9ob21ld2tyZWM-')
    

    resp = opener.open(req, timeout=5)
    qrcont=resp.read()
    '''
    print content
    print 'over6'
    submit_header = {   'Content-Type':'application/x-www-form-urlencoded', \
                        'User-agent':'Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)', \
                        'Connection': 'keep-alive' ,\
                        'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',\
                        'Origin':'http://learn.tsinghua.edu.cn',\
                        'Accept-Language': 'zh-CN,zh;q=0.8,en;q=0.6',\
                        'Accept-Encoding':'gzip, deflate',\
                        'Cache-Control': 'max-age=0',\
                        'Refer':submit_url,\
                        'Cookie': logged_response['set-cookie']
                    }

    params = {  "saveDir" : course_id+'/homewkrec/',
                "filename" : newfullfilenamep1+'0'+newfullfilenamep2, \
                'old_filename' :  old_filename ,\
                'errorURL' : '/uperror.jsp?error=' ,\
                'returnURL' : '/win_close.jsp',\
                'newfilename' : newfilename,\
                'post_id' : submit_id,\
                'post_rec_id' : post_rec_id,\
                'post_homewk_link' : newfullfilenamep1+'0'+newfullfilenamep2,\
                'file_unique_flag' : '0',\
                'url_post' : '/MultiLanguage/lesson/student/hom_wk_handin.jsp',\
                'css_name' : '',\
                'tmpl_name' : '',\
                'course_id' : course_id,\
                'module_id' : '325',\
                'post_rec_homewk_detail' : filecontent,\
                'Submit' : '提交',\
                'tj' : ''\
            }
    response, content = http.request(handin_url, 'POST', headers=submit_header, body=urllib.urlencode(params))
    #req=urllib2.Request(handin_url, data=urllib.urlencode(params))
    #req=urllib2.Request('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_handin.jsp', data=urllib.urlencode(params))
    #req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % boundary)
    #header
    '''
    #req.add_header('Content-Type', 'multipart/form-data; boundary=%s' % boundary)
    req.add_header('User-Agent','Mozilla/5.0')
    req.add_header('Connection', 'keep-alive')
    req.add_header('Accept','text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8')
    req.add_header('Origin','http://learn.tsinghua.edu.cn')
    req.add_header('Accept-Language', 'zh-CN,zh;q=0.8,en;q=0.6')
    req.add_header('Accept-Encoding','gzip, deflate')
    req.add_header('Cache-Control', 'max-age=0')
    req.add_header('Refer',submit_url)
    '''
    #req.add_header('Cookie','_ga=GA1.3.1793295275.1416054436; JSESSIONID=abcPZLm_7--lq-bdz-_2u; THNSV2COOKIE=YmY1NzcxNTdiZWI2MTdjM2YwNzQ1YmY1NWJkNDZlZDZgMjAxMjAxMTQwMmDC7c.+sfJgU2A1OS42Ni4xMzAuMTlgMjY3ODA2Njc1YDE0MzM0ODE3NDkyNThgMDFaQjk2NTI1SFdLMjhIODI0UUNMOFdIYA--; THNSV2APACHECOOKIE=a881ef7cff3ec3692c7ae3d501b4427bMTQzMzQ4MTc0OXwyMDEyMDExNDAyfFN8NTkuNjYuMTMwLjE5fFV8L2hvbWUvcmVzaW5hcHAvd2x4dC9odGRvY3MvQ3VyVmVyc2lvbi9BcHBGaWxlL2tlamlhbi9kYXRhLzEyNDA5NS9ob21ld2tyZWM-')


    #resp=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_handin.jsp',urllib.urlencode(params))
    #resp = urllib2.urlopen(req, timeout=5)

    #get response
    qrcont=content
    print qrcont

    '''
    opener = poster.streaminghttp.register_openers()  
    opener.add_handler(urllib2.HTTPCookieProcessor(cj))  
      
    
    datagen, headers = poster.encode.multipart_encode(params)  
    request = urllib2.Request(submit_url, data, headers)  
    result = urllib2.urlopen(request)  
    '''
    fr2=open('ans.html','w')
    fr2.write(str(qrcont))
    fr2.close()

def handin(courseid, userid, userpass, submit_id, upload_filepath):
    print courseid+ userid+ userpass+ submit_id+ upload_filepath
    login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
    cj = cookielib.CookieJar()
    #cookieJar作为参数，获得一个opener的实例
    opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)')]
    logindata = urllib.urlencode({"userid":userid,"userpass":userpass})
    opener.open(login_page,logindata)


    #获得课程作业页面
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_brw.jsp?course_id='+courseid)
    #获得
    pagedata = op.read()
    recidhp = recidHTMLParser(submit_id)
    recidhp.feed(pagedata)
    print recidhp.post_rec_id

    #获得作业提交页面
    op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_submit.jsp?id='+submit_id+'&course_id='+courseid+'&rec_id='+recidhp.post_rec_id)
    pagedata = op.read()
    #print pagedata
    subinfohp = postInfoHTMLParser()
    subinfohp.feed(pagedata)
    print subinfohp.old_filename
    print 'start'
    submit(submit_id,subinfohp.old_filename,recidhp.post_rec_id,upload_filepath,courseid,userid,userpass)
    print 'over'
    print get_mime_type(upload_filepath.split('/')[-1])


if __name__ == '__main__':

        userid = '2012011402'
        userpass = 'Tsinghua2012'
        courseid = '124097'
        submit_id = '643677'
        upload_filename = './down.py'
        
        handin(courseid, userid, userpass, submit_id, upload_filename)
        login_page = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/teacher/loginteacher.jsp"
'''
        cj = cookielib.CookieJar()
        #cookieJar作为参数，获得一个opener的实例
        opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
        opener.addheaders = [('User-agent','Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)')]

        logindata = urllib.urlencode({"userid":userid,"userpass":userpass})
        opener.open(login_page,logindata)

        #获得课程作业页面
        op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_brw.jsp?course_id='+courseid)
        #获得
        pagedata = op.read()
        recidhp = recidHTMLParser(submit_id)
        recidhp.feed(pagedata)
        print recidhp.post_rec_id

        #获得作业提交页面
        op=opener.open('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_submit.jsp?id='+submit_id+'&course_id='+courseid+'&rec_id='+recidhp.post_rec_id)
        pagedata = op.read()
        #print pagedata
        subinfohp = postInfoHTMLParser()
        subinfohp.feed(pagedata)
        print subinfohp.old_filename

        submit(submit_id,subinfohp.old_filename,recidhp.post_rec_id,upload_filename,courseid,userid,userpass)

        print get_mime_type(upload_filename)
#except Exception,e:
#    print e
'''   
#if __name__ == '__main__':
#fusermount -u /home/mxb/fuse
#./hello /home/mxb/fuse -d
    #f = open('info.txt', 'w') 
    #parseHomeworkList('','124095','ma-xb12',"Tsinghua2012")
    #f.write(a)
    #getUserPage('http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/MyCourse.jsp?language=cn','ma-xb12','Tsinghua2012');
    #getFileTypeSize('http://learn.tsinghua.edu.cn/uploadFile/downloadFile_student.jsp?module_id=322&filePath=OcwlnZxhKf3pBMMAL86yYqQ/rkOtMMivFCRg9evL3Nyk4uDyy61qw0815PWjIRoxRs2i15DQ5DIsE7ZaBEzlwKfAfbhdnycEzv33oiTB78s%3D&course_id=124095&file_id=1327082','ma-xb12','Tsinghua2012')
    #fileHandle = open ( 'lear.html', 'w') 
    #parseCourse("http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/MyCourse.jsp?language=cn","ma-xb12","Tsinghua2012")
