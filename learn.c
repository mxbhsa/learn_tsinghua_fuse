/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <python2.7/Python.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *no_str = "no result\n";
static const char *course_url = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/MyCourse.jsp?language=cn";
//static const char *note_url = "http://learn.tsinghua.edu.cn/MultiLanguage/public/bbs/note_list_student.jsp";
//static const char *homwork_url = "http://learn.tsinghua.edu.cn/MultiLanguage/lesson/student/hom_wk_brw.jsp";
char bufPath[200] = "/home/mxb/fuse_temp/";
char pythonPath[200] = "/home/mxb/";


struct cFile
{
	char name[200];
	char link[1500];
	char type[50];
	char submit_id[20];
	char bufPath[1000];
	int size;
	int isReady;
	struct cFile * next;
	struct cFile *homework;
};

struct Course
{
	char course_id[20];
	char course_name[100];
	char teacher_name[50];
	char info[15000];
	int infoLen;
	struct cFile *downList;
	int downNum;
	struct cFile *noteList;
	int noteNum;
	struct cFile *homeworkList;
	int homeNum;
	struct Course *next;
};

struct User
{
	char userid[100];
	char userpass[100];
	int course_num;
	struct Course *course_list;
	struct User *next;
};


void add_into_list(struct User *);

struct User *userList = NULL;

void add_into_list(struct User *u)
{
	u->next = userList;
	userList = u;
}




int getDownloadInfo(struct User * user, struct Course * course)
{
	if(course->downList != NULL)
		return -2;
	Py_Initialize();

    	printf("开始获取%s的%s的课程文件!\n",user->userid,course->course_name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200] = {0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}

		pFunc = PyObject_GetAttrString(pModule,"parseDownload");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		PyObject *pArgs = PyTuple_New(3); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",course->course_id ));//0-
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
		else
		{
			int downNum =  PyList_Size(pReturn), i = 0;
			printf("down file num : %d\n",downNum/4);
			for(;i < downNum; i++)
			{
				struct cFile * newF = (struct cFile *)malloc(sizeof(struct cFile));
				char *name;
				PyArg_Parse(PyList_GetItem(pReturn, i), "s", &name);	//link
				strcpy(newF->link,name);
				PyArg_Parse(PyList_GetItem(pReturn, ++i), "s", &name); //type
				strcpy(newF->type,name);
				PyArg_Parse(PyList_GetItem(pReturn, ++i), "i", &newF->size); //size

				PyArg_Parse(PyList_GetItem(pReturn,++i), "s", &name);	//name
				strcpy(newF->name,name);
				//printf("%s %d\n",newC->course_name,newC->course_id);
				newF->next = course->downList;
				newF->homework = NULL;
				newF->isReady = -1;
				memset(newF->bufPath,0,sizeof(newF->bufPath));
				course->downList = newF;
			}
			course->downNum = downNum/4;
		}
	Py_Finalize();
	return 0;
}





int getCourseNote(struct User * user, struct Course * course)
{
	if(course->noteList != NULL || !strcmp(course->course_id,"122360"))
		return -2;
	Py_Initialize();

    	printf("开始获取%s的%s的课程公告!\n",user->userid,course->course_name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200]={0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}
		pFunc = PyObject_GetAttrString(pModule,"parseCourseNoteList");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		PyObject *pArgs = PyTuple_New(4); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",bufPath ));
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s",course->course_id ));//0-
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 3, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
		else
		{
			int downNum =  PyList_Size(pReturn), i = 0;
			printf("note num : %d\n",downNum/3);
			for(;i < downNum; i++)
			{
				struct cFile * newF = (struct cFile *)malloc(sizeof(struct cFile));
				memset(newF->bufPath,0,sizeof(newF->bufPath));
				memset(newF->name,0,sizeof(newF->name));
				char *name;
				PyArg_Parse(PyList_GetItem(pReturn, i), "s", &name);	//bufPath
				strcpy(newF->bufPath,name);
				PyArg_Parse(PyList_GetItem(pReturn,++i), "s", &name);	//name
				strcpy(newF->name,name);
				PyArg_Parse(PyList_GetItem(pReturn,++i), "i", &newF->size);	//name
				newF->next = course->noteList;
				newF->isReady = -1;
				newF->homework = NULL;
				course->noteList = newF;
			}
			course->noteNum = downNum/3;
		}
	Py_Finalize();
	return 0;
}


int getHomeworks(struct User * user, struct Course * course)
{
	if(course->homeworkList != NULL)
		return -2;
	Py_Initialize();

    	printf("开始获取%s的%s的课程作业!\n",user->userid,course->course_name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200]={0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}
		pFunc = PyObject_GetAttrString(pModule,"parseHomeworkList");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		PyObject *pArgs = PyTuple_New(4); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",bufPath ));
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s",course->course_id ));//0-
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 3, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
		else
		{
			int homeNum =  PyList_Size(pReturn), i = 0;
			printf("homework num : %d\n",homeNum/4);
			for(;i < homeNum; i++)
			{
				struct cFile * newF = (struct cFile *)malloc(sizeof(struct cFile));
				memset(newF->bufPath,0,sizeof(newF->bufPath));
				memset(newF->name,0,sizeof(newF->name));
				char *ret;
				PyArg_Parse(PyList_GetItem(pReturn, i), "s", &ret);	//bufPath
				strcpy(newF->bufPath,ret);
				PyArg_Parse(PyList_GetItem(pReturn,++i), "s", &ret);	//name
				strcpy(newF->name,ret);
				PyArg_Parse(PyList_GetItem(pReturn,++i), "i", &newF->size);	//name
				PyArg_Parse(PyList_GetItem(pReturn,++i), "s", &ret);	//name
				strcpy(newF->submit_id,ret);
				printf("idididididi %s\n",newF->submit_id);
				newF->next = course->homeworkList;
				newF->isReady = -1;
				newF->homework = NULL;
				course->homeworkList = newF;
			}
			course->noteNum = homeNum/4;
		}
	Py_Finalize();
	return 0;
}

/*
int getFileTypeSize(struct User * user, struct cFile *file)
{

	Py_Initialize();

    	printf("开始获取%s的课程文件%s信息!\n",user->userid,file->name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('/home/mxb')");

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}

		pFunc = PyObject_GetAttrString(pModule,"getFileTypeSize");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		PyObject *pArgs = PyTuple_New(3); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",file->link ));//0-
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
		else
		{
				char *ret;
				PyArg_Parse(PyList_GetItem(pReturn, 0), "s", &ret);
				strcpy(file->type,ret);
				PyArg_Parse(PyList_GetItem(pReturn, 1), "i", &file->size);
				//printf("%d size!\n",file->size);
		}
	Py_Finalize();

	return 0;
}*/


int getCourseInfo(struct User * user, struct Course * course)
{
	if(strlen(course->info) > 0)
		return 0;
	Py_Initialize();

    	printf("开始获取%s的课程%s的信息!\n",user->userid,course->course_name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200]={0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}

		pFunc = PyObject_GetAttrString(pModule,"parseCourseInfo");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		PyObject *pArgs = PyTuple_New(3); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",course->course_id ));//0-
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
		else
		{
				char *ret;
				int infoLen = 0;

				PyArg_Parse(PyList_GetItem(pReturn, 0), "i", &infoLen);
				PyArg_Parse(PyList_GetItem(pReturn, 1), "s", &ret);
				if(infoLen > sizeof(course->info))
					infoLen = sizeof(course->info);
				memcpy(course->info,ret,infoLen);
				course->infoLen = infoLen;
				//printf("%d size!\n",file->size);
		}
	Py_Finalize();

	return 0;
}

struct cFile * findFile(const char *path, struct User * user, struct Course * course, struct cFile* fileList, char *fFolder)
{
	char *fname = path + 1 + strlen(user->userid) + 1 + strlen(course->course_name) + 1 + strlen(fFolder) +1;
	struct cFile * nowF = fileList;
	while(nowF)
	{
		if(strcmp(nowF->name, fname) == 0)
			return nowF;
		nowF = nowF->next;
	}
	return NULL;
}


static int hello_getattr(const char *path, struct stat *stbuf)
{
	printf("** get attr %s\n",path);
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;	//文件类型
		stbuf->st_nlink = 1;	//引用计数
		stbuf->st_size = strlen(hello_str); //文件大小
		return 0;
	}else if(strcmp(path,"/fuck") == 0){
		stbuf->st_mode = S_IFREG | S_IWGRP | S_IWOTH;
		stbuf->st_nlink = 1;	//引用计数
		stbuf->st_size = strlen(no_str); //文件大小
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	struct User * nowU = userList;
	while(nowU)
	{
		if((strstr(path+1,nowU->userid) == path+1) )
		{
			if(strlen(nowU->userid) == strlen(path)-1) // 找的就是此目录
			{
				stbuf->st_mode = S_IFDIR | S_IWGRP | S_IWOTH;
				stbuf->st_nlink = 2+nowU->course_num;	//引用计数
				return 0;
			}
			else
				break;
		}
		nowU = nowU->next;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	struct Course * nowC = NULL;
	if(nowU)
	{
		//此时nowU为其父目录
		//访问用户子目录
		nowC = nowU->course_list;
		while(nowC)
		{
			//printf("course name: %s\n",nowC->course_name);
			if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
			{
				if(strlen(nowC->course_name) == strlen(path)-2-strlen(nowU->userid)) // 找的就是此目录
				{
					stbuf->st_mode = S_IFDIR | S_IWGRP | S_IWOTH;
					stbuf->st_nlink = 4;	//引用计数
					return 0;
				}
				else
					break;
			}
			nowC = nowC->next;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	if(nowC)
	{
		//此时进入课程的内容中
		if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程公告") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
			{
				printf("search 课程公告%s \n",nowC->course_name);
				stbuf->st_mode = S_IFDIR | 0444;	//文件类型
				stbuf->st_nlink = 1;	//引用计数
				stbuf->st_size = nowC->noteNum; //文件大小
				return 0;
			}
			else
			{
				struct cFile * nowF = NULL;
				if((nowF = findFile(path, nowU, nowC, nowC->noteList,"课程公告")) != NULL)
				{
					//	if(nowF->size < 0)//文件属性还未取得
					//getFileTypeSize(nowU, nowF );
					stbuf->st_mode = S_IFREG | 0444;	//文件类型
					stbuf->st_nlink = 1;	//引用计数
					stbuf->st_size = nowF->size; //文件大小
					return 0;
				}
				return -ENOENT;
			}
		}
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程信息" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程信息") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
			{
				printf("读取课程信息！！！！！！！！！！！！\n");
				stbuf->st_mode = S_IFREG | 0444;	//文件类型
				stbuf->st_nlink = 1;	//引用计数
				stbuf->st_size = nowC->infoLen; //文件大小
				return 0;
			}
			return -ENOENT;
		}
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程文件") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
			{
				stbuf->st_mode = S_IFDIR | 0444;	//文件类型
				stbuf->st_nlink = 1;	//引用计数
				stbuf->st_size = nowC->downNum; //文件大小
				printf("** 当前路径文件数目%d\n", nowC->downNum);
				return 0;
			}
			else
			{
				struct cFile * nowF = NULL;
				if((nowF = findFile(path, nowU, nowC, nowC->downList,"课程文件")) != NULL)
				{
					stbuf->st_mode = S_IFREG | 0444;	//文件类型
					stbuf->st_nlink = 1;	//引用计数
					stbuf->st_size = nowF->size; //文件大小
					return 0;
				}
				return -ENOENT;
			}
		}
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程作业") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
			{
				stbuf->st_mode = S_IFDIR | 0444;	//文件类型
				stbuf->st_nlink = 1;	//引用计数
				stbuf->st_size = nowC->homeNum; //文件大小
				return 0;
			}
			else
			{
				char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1;
				struct cFile * nowF = nowC->homeworkList;
				while(nowF)
				{
					printf("asdfasd0002f %s\n",nowF->name);
					if(strstr(hName,nowF->name) == hName)
					{
						char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1+strlen(nowF->name)+1;
						if((findFile(path, nowU, nowC, nowC->homeworkList,"课程作业")) != NULL)
						{
							stbuf->st_mode = S_IFDIR | 0444;	//文件类型
							stbuf->st_nlink = 1;	//引用计数
							stbuf->st_size = 2; //文件大小
							return 0;
						}
						else if(hName - path <= strlen(path))//找其下文件
						{
							printf("asdfa %s %s %s\n",hName,nowF->name,nowF->homework);
							if(strcmp(hName,nowF->name) == 0)
							{
								stbuf->st_mode = S_IFREG | 0444;	//文件类型
								stbuf->st_nlink = 1;	//引用计数
								stbuf->st_size = nowF->size; //文件大小
								return 0;
							}
							else if(nowF->homework)
							{
								printf("asdfa222 %s %s\n",hName,nowF->homework->name);
								if(strcmp(hName,nowF->homework->name) == 0)
								{
									stbuf->st_mode = S_IFREG | 0444;	//文件类型
									stbuf->st_nlink = 1;	//引用计数
									stbuf->st_size = nowF->homework->size; //文件大小
									return 0;
								}
							}
							return -ENOENT;
						}
						return-ENOENT;
					}
					nowF = nowF->next;
				}
				return -ENOENT;
			}
		}
		else
		return -ENOENT;
	}


	return -ENOENT;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	printf("** readdir\n");
	(void) offset;
	(void) fi;

	//访问根目录
	if (strcmp(path, "/") == 0)
	{
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		//filler(buf, hello_path + 1, NULL, 0);
		//filler(buf, "fuck", NULL, 0);

		struct User * nowU = userList;//所有用户处于根目录
		while(nowU)
		{
			filler(buf, nowU->userid, NULL, 0);
			nowU = nowU->next;
		}
		return 0;
	}
	printf("visit %s\n",path);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//访问用户子目录
	struct User * nowU = userList;//所有用户处于根目录
	while(nowU)
	{

		if((strstr(path+1,nowU->userid) == path+1)) //找到该目录
		{
			if(strlen(nowU->userid) == strlen(path)-1) //已经到最底层
			{
				filler(buf, ".", NULL, 0);
				filler(buf, "..", NULL, 0);
				struct Course * nowC = nowU->course_list;
				while(nowC)//显示用户所有课程
				{
					filler(buf, nowC->course_name, NULL, 0);
					nowC = nowC->next;
				}
				return 0;
			}
			else
				break;
		}
		nowU = nowU->next;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//此时到
	struct Course * nowC = NULL;
	if(nowU)
		{
			nowC = nowU->course_list;
			//此时nowU为其父目录
			//访问用户子目录
			while(nowC)
			{
				if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
				{
					if(strlen(nowC->course_name) == strlen(path)-2-strlen(nowU->userid)) // 找的就是此目录
					{
						filler(buf, ".", NULL, 0);
						filler(buf, "..", NULL, 0);

						filler(buf, "课程作业", NULL, 0);
						filler(buf, "课程公告", NULL, 0);
						filler(buf, "课程信息", NULL, 0);
						filler(buf, "课程文件", NULL, 0);

						return 0;
					}
					else
						break;
				}
				nowC = nowC->next;
			}
		}
	printf("** readdir %s\n",path);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		getDownloadInfo(nowU, nowC);
		printf("** find file list %s\n",path);
		struct cFile* nowF = nowC->downList;
		while(nowF)
		{
			printf("**************************file: %s\n",nowF->name);
			nowF->isReady = -1;
			filler(buf, nowF->name, NULL, 0);
			nowF = nowF->next;
		}
//		filler(buf,"测试.txt",NULL,0);
		return 0;
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		printf("** find note list %s\n",path);
		getCourseNote(nowU, nowC);
		struct cFile* nowF = nowC->noteList;
		while(nowF)
		{
			printf("###################note: %s\n",nowF->name);
			nowF->isReady = -1;
			filler(buf, nowF->name, NULL, 0);
			nowF = nowF->next;
		}
		return 0;
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		printf("** find homework list %s\n",path);
		getHomeworks(nowU,nowC);

		if(strlen("课程作业") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
		{
			struct cFile* nowF = nowC->homeworkList;
			while(nowF)
			{
				printf("###################homework: %s\n",nowF->name);
				nowF->isReady = -1;
				filler(buf, nowF->name, NULL, 0);
				nowF = nowF->next;
			}
			return 0;
		}
		else //其下文件
		{
			struct cFile* nowF = NULL;
			if((nowF = findFile(path, nowU, nowC, nowC->homeworkList,"课程作业")) != NULL)
			{
					printf("###################222homework: %s\n",nowF->name);
					nowF->isReady = -1;
					filler(buf, nowF->name, NULL, 0);
					if(nowF->homework)
						filler(buf, nowF->homework->name, NULL, 0);
					return 0;
			}
		}
	}
}

struct User * findUser(const char *path, struct User *nowU)
{
	nowU = userList;
		while(nowU)
		{
			if((strstr(path+1,nowU->userid) == path+1) )
			{
				if(strlen(nowU->userid) == strlen(path)-1) // 找的就是此目录
					return nowU;
				else
					return NULL;
			}
			nowU = nowU->next;
		}
		return NULL;
}


int uploadHomework(struct User * user, struct Course * course,struct cFile*uphwkName)
{
		if(uphwkName->homework->isReady > 0)
			return 0;
		Py_Initialize();

    	printf("开始上传%s的课程%s的作业%s-submit_id=%s\n",user->userid,course->course_name,uphwkName->name,uphwkName->submit_id);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200]={0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("upload");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}

		pFunc = PyObject_GetAttrString(pModule,"handin");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}
		//handin(courseid,userid, userpass,  submit_id, upload_filename)
		PyObject *pArgs = PyTuple_New(5); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",course->course_id ));//0-
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userpass));//1---
		PyTuple_SetItem(pArgs, 3, Py_BuildValue("s", uphwkName->submit_id));//1---
		PyTuple_SetItem(pArgs, 4, Py_BuildValue("s", uphwkName->homework->bufPath));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数
		printf("上传完成%s！\n",uphwkName->homework->name);
		uphwkName->homework->isReady ++;
	Py_Finalize();

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	printf("open %s %d %d %d\n",path,fi->flags,fi->direct_io,fi->keep_cache);

	if (strcmp(path, hello_path) == 0)
	{
		return 0;
	}

	struct User * nowU = userList;
		while(nowU)
		{
			if((strstr(path+1,nowU->userid) == path+1) )
					break;
			nowU = nowU->next;
		}
	if(!nowU)
		return -ENOENT;

	struct Course * nowC = NULL;
	//访问用户子目录
	nowC = nowU->course_list;
	while(nowC)
	{
		if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
				break;
		nowC = nowC->next;
	}
	if(!nowC)
			return -ENOENT;

	if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程信息" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		if(strlen("课程信息") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
		{
			return 0;
		}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
			struct cFile * nowF = NULL;
			if((nowF = findFile(path, nowU, nowC, nowC->downList,"课程文件")) != NULL)
			{
				nowF->isReady ++;
				if(nowF->isReady > 0) // 确认打开
				{
					printf("准备打开文件%s\n",nowF->name);
					//getchar();
					return 0;
				}
			}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
			struct cFile * nowF = NULL;
			if((nowF = findFile(path, nowU, nowC, nowC->noteList,"课程公告")) != NULL)
			{
					printf("准备打开文件%s\n",nowF->name);
					//getchar();
					return 0;
			}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
			char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1;
			struct cFile * nowF = nowC->homeworkList;
			while(nowF)
			{
				if(strstr(hName,nowF->name) == hName)
				{
					char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1+strlen(nowF->name)+1;
					if(hName - path <= strlen(path))//找其下文件
					{
						if(strcmp(hName,nowF->name) == 0)
							return 0;
						else if(nowF->homework)
							if(strcmp(hName,nowF->homework->name) == 0)
							{
								printf("上传作业！！！！ %d\n",nowF->homework->isReady);
								uploadHomework(nowU,nowC,nowF);
								return 0;
							}
						return -ENOENT;
					}
					return-ENOENT;
				}
				nowF = nowF->next;
			}
			return -ENOENT;
	}

	return -ENOENT;
}


int downloadFile(struct User * user, struct cFile *file)
{

	Py_Initialize();

    	printf("开始下载%s的课程文件%s!\n",user->userid,file->name);
		PyObject * pModule = NULL;
		PyObject * pFunc = NULL;
		PyObject *pReturn = NULL;
		PyRun_SimpleString("import sys");
		char pythonPathAdd[200]={0};
		strcat(pythonPathAdd,"sys.path.append('");
		strcat(pythonPathAdd,pythonPath);
		strcat(pythonPathAdd,"')");
		PyRun_SimpleString(pythonPathAdd);

		pModule = PyImport_ImportModule("url");
		if (!pModule) {
				printf("Cant open python file!\n");
				return -1;
		}

		pFunc = PyObject_GetAttrString(pModule,"downloadFile");//Add:Python文件中的函数名    //创建参数
		if(pFunc == NULL)
		{
			printf("python function error!\n");
			return -1;
		}

		PyObject *pArgs = PyTuple_New(4);
		PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",file->bufPath ));
		PyTuple_SetItem(pArgs, 1, Py_BuildValue("s",file->link ));//0-
		PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userid));//1---
		PyTuple_SetItem(pArgs, 3, Py_BuildValue("s", user->userpass));//1---

		pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

		if(pReturn == NULL)
		{
			return -1;
			printf("null python return\n");
		}
	Py_Finalize();

	return 0;
}



static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	printf("** read %s\n",path);
	if(strcmp(path, hello_path) == 0)
	{
		printf("reading\n");
		len = strlen(hello_str);
		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, hello_str + offset, size);
		} else
			size = 0;
	}

	struct User * nowU = userList;
		while(nowU)
		{
			if((strstr(path+1,nowU->userid) == path+1) )
					break;
			nowU = nowU->next;
		}
	if(!nowU)
		return -ENOENT;

	struct Course * nowC = NULL;
	//访问用户子目录
	nowC = nowU->course_list;
	while(nowC)
	{
		if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
				break;
		nowC = nowC->next;
	}
	if(!nowC)
			return -ENOENT;

	if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程信息" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		if(strlen("课程信息") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
		{
			len = nowC->infoLen;
			if (offset < len) {
						if (offset + size > len)
							size = len - offset;
						memcpy(buf, nowC->info + offset, size);
					} else
						size = 0;
			return size;
		}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
			struct cFile * nowF = NULL;
			if((nowF = findFile(path, nowU, nowC, nowC->downList,"课程文件")) != NULL)
			{
				//找到文件
				if(strlen(nowF->bufPath) == 0)
				{
					//需要下载该文件
					strcpy(nowF->bufPath,bufPath);
					strcat(nowF->bufPath,nowF->name);
					printf("download file %s\n",nowF->bufPath);
					downloadFile(nowU,nowF);
				}
				if (offset > nowF->size)
					return 0;
				FILE *fp = fopen(nowF->bufPath,"r");
				if(!fp)
					return 0;
				fseek(fp,offset,SEEK_SET);
				if(offset + size > nowF->size)
					size = nowF->size - offset;
				size = fread(buf,sizeof(char),size,fp);
				fclose(fp);
				printf("*** has read %d offset:%d size:%d\n",(int)size,offset,nowF->size);
				return size;
			}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
			struct cFile * nowF = NULL;
			if((nowF = findFile(path, nowU, nowC, nowC->noteList,"课程公告")) != NULL)
			{
				if (offset > nowF->size)
					return 0;
				FILE *fp = fopen(nowF->bufPath,"r");
				if(!fp)
					return 0;
				fseek(fp,offset,SEEK_SET);
				if(offset + size > nowF->size)
					size = nowF->size - offset;
				size = fread(buf,sizeof(char),size,fp);
				fclose(fp);
				printf("*** has read %d offset:%d size:%d\n",(int)size,offset,nowF->size);
				return size;
			}
	}
	else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
	{
		char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1;
		struct cFile * nowF = nowC->homeworkList;
		while(nowF)
		{
			if(strstr(hName,nowF->name) == hName) // 找到该目录
			{
				char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1+strlen(nowF->name)+1;
				if(hName - path <= strlen(path))//找其下文件
				{
					if(strcmp(hName,nowF->name) == 0)
					{
						if (offset > nowF->size)
							return 0;
						FILE *fp = fopen(nowF->bufPath,"r");
						if(!fp)
							return 0;
						fseek(fp,offset,SEEK_SET);
						if(offset + size > nowF->size)
							size = nowF->size - offset;
						size = fread(buf,sizeof(char),size,fp);
						fclose(fp);
						printf("*** has read %d offset:%d size:%d\n",(int)size,offset,nowF->size);
						return size;
					}
					else if(nowF->homework)
					{
						if(strcmp(hName,nowF->homework->name) == 0)
						{
							if (offset > nowF->homework->size)
								return 0;
							printf("read path %s\n",nowF->homework->bufPath);
							FILE *fp = fopen(nowF->homework->bufPath,"r");
							if(!fp)
								return 0;
							fseek(fp,offset,SEEK_SET);
							if(offset + size > nowF->homework->size)
								size = nowF->homework->size - offset;
							size = fread(buf,sizeof(char),size,fp);
							fclose(fp);
							printf("*** has read %d offset:%d size:%d\n",(int)size,offset,nowF->homework->size);
							return size;
						}
					}
					return -ENOENT;
				}
				return-ENOENT;
			}
			nowF = nowF->next;
		}
		return -ENOENT;
	}
	return size;
}



static int hello_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res = 0;

	(void) fi;
	printf("write %s\n",path);
	if (strcmp(path, "/") == 0) {
		return 0;
	} else if (strcmp(path, hello_path) == 0) {
		return 0;
	}else if(strcmp(path,"/fuck") == 0){
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	struct User * nowU = userList;
	while(nowU)
	{
		if((strstr(path+1,nowU->userid) == path+1) )
		{
			if(strlen(nowU->userid) == strlen(path)-1) // 找的就是此目录
				return 0;
			else
				break;
		}
		nowU = nowU->next;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	struct Course * nowC = NULL;
	if(nowU)
	{
		//此时nowU为其父目录
		nowC = nowU->course_list;
		while(nowC)
		{
			if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
				if(strlen(nowC->course_name) == strlen(path)-2-strlen(nowU->userid)) // 找的就是此目录
					return 0;
				else
					break;
			nowC = nowC->next;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	printf("asdfasdfasdfasdf\n");
	if(nowC)
	{
		//此时进入课程的内容中
		if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
					return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程信息" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
				return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
				return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程作业") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
				return 0;
			else
			{
				char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1;
				struct cFile * nowF = nowC->homeworkList;
				while(nowF)
				{
					if(strstr(hName,nowF->name) == hName)
					{
						printf("wr111ite\n");
						char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1+strlen(nowF->name)+1;
						if(( findFile(path, nowU, nowC, nowC->homeworkList,"课程作业")) != NULL)
							return 0;
						else if(hName - path <= strlen(path))//找其下文件
						{
							printf("wri222te\n");
							if(strcmp(hName,nowF->name) == 0)
								return 0;
							else//写文件
							{
								printf("write\n");
								if(!nowF->homework)
									return 0;
								struct cFile * writeF = nowF->homework;
								char tempBufPath[1000];
								strcpy(tempBufPath,bufPath);
								strcat(tempBufPath,"_upload_");
								strcat(tempBufPath,nowF->name);
								strcat(tempBufPath,nowF->homework->name);
								strcpy(writeF->bufPath,tempBufPath);
								FILE *fp = fopen(tempBufPath,"w+");
								printf("w3333333rite %s\n",tempBufPath);
								if(!fp)
									return 0;
								fseek(fp,offset,SEEK_SET);
								size = fwrite(buf,sizeof(char),size,fp);
								nowF->homework->size += size;
								printf("*** has write %d offset:%d size:%d contnt:%s\n",(int)size,offset,nowF->homework->size,buf);
								fclose(fp);
								return size;
								return 0;
							}
							return -ENOENT;
						}
						return-ENOENT;
					}
					nowF = nowF->next;
				}
				return -ENOENT;
			}
		}
		else
			return -ENOENT;
	}
	return -ENOENT;
	return res;
}

static int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	printf("** create %s\n",path);
	if (strcmp(path, "/") == 0) {
		return 0;
	} else if (strcmp(path, hello_path) == 0) {
		return 0;
	}else if(strcmp(path,"/fuck") == 0){
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	struct User * nowU = userList;
	while(nowU)
	{
		if((strstr(path+1,nowU->userid) == path+1) )
		{
			if(strlen(nowU->userid) == strlen(path)-1) // 找的就是此目录
				return 0;
			else
				break;
		}
		nowU = nowU->next;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	struct Course * nowC = NULL;
	if(nowU)
	{
		//此时nowU为其父目录
		nowC = nowU->course_list;
		while(nowC)
		{
			if((strstr(path+2+strlen(nowU->userid),nowC->course_name) == path+2+strlen(nowU->userid)) )
				if(strlen(nowC->course_name) == strlen(path)-2-strlen(nowU->userid)) // 找的就是此目录
					return 0;
				else
					break;
			nowC = nowC->next;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	if(nowC)
	{
		//此时进入课程的内容中
		if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程公告" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
					return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程信息" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
				return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程文件" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
				return 0;
		else if((strstr(path+3+strlen(nowU->userid)+strlen(nowC->course_name),"课程作业" ) == path+3+strlen(nowC->course_name)+strlen(nowU->userid)))
		{
			if(strlen("课程作业") == strlen(path)-3-strlen(nowU->userid)-strlen(nowC->course_name)) // 找的就是此目录
				return 0;
			else
			{
				char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1;
				struct cFile * nowF = nowC->homeworkList;
				while(nowF)
				{
					if(strstr(hName,nowF->name) == hName)
					{
						char *hName = path + 1 + strlen(nowU->userid) + 1 + strlen(nowC->course_name) + 1 + strlen("课程作业") +1+strlen(nowF->name)+1;
						if(( findFile(path, nowU, nowC, nowC->homeworkList,"课程作业")) != NULL)
							return 0;
						else if(hName - path <= strlen(path))//找其下文件
						{
							if(strcmp(hName,nowF->name) == 0)
								return 0;
							else//添加文件
							{
								if(nowF->homework)
									free(nowF->homework);
								struct cFile * newF = (struct cFile *)malloc(sizeof(struct cFile));
								memset(newF,0,sizeof(struct cFile));
								char *newFilename = strrchr(path,'/')+1;
								printf("create file %s\n",newFilename);
								strcpy(newF->name,newFilename);
								newF->size = 0;
								newF->isReady = 0;
								nowF->homework = newF;
								return 0;
							}
							return -ENOENT;
						}
						return-ENOENT;
					}
					nowF = nowF->next;
				}
				return -ENOENT;
			}
		}
		else
			return -ENOENT;
	}
	return -ENOENT;
}


static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
	.write		= hello_write,
	.create 	= hello_create,
};

int getCourseList(struct User * user)
{
	Py_Initialize();

    		printf("开始获取%s的课程数据!\n",user->userid);
			PyObject * pModule = NULL;
			PyObject * pFunc = NULL;
			PyObject *pReturn = NULL;
			PyRun_SimpleString("import sys");/*
			char pythonPathAdd[200]={0};
			strcat(pythonPathAdd,"sys.path.append('");
			strcat(pythonPathAdd,pythonPath);
			strcat(pythonPathAdd,"')\n");
			printf("%s\n",pythonPathAdd);
			PyRun_SimpleString(pythonPathAdd);*/
			PyRun_SimpleString("sys.path.append('/home/mxb/')");

			pModule = PyImport_ImportModule("url");
		    if (!pModule) {
		            printf("Cant open python file!\n");
		            return -1;
		    }

			pFunc = PyObject_GetAttrString(pModule,"parseCourse");//Add:Python文件中的函数名    //创建参数
		    if(pFunc == NULL)
		    {
		    	printf("python function error!\n");
		    	return -1;
		    }
			PyObject *pArgs = PyTuple_New(3); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数
			PyTuple_SetItem(pArgs, 0, Py_BuildValue("s",course_url ));//0-
			PyTuple_SetItem(pArgs, 1, Py_BuildValue("s", user->userid));//1---
			PyTuple_SetItem(pArgs, 2, Py_BuildValue("s", user->userpass));//1---

			pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数

			if(pReturn == NULL)
			{
				return -1;
				printf("null python return\n");
			}
			else
			{
				int courseNum =  PyList_Size(pReturn), i = 0;
				printf("course num : %d\n",courseNum/2);
				for(;i < courseNum; i++)
				{
					struct Course * newC = (struct Course *)malloc(sizeof(struct Course));
					memset(newC,0,sizeof(struct Course));
					char *name;
					PyArg_Parse(PyList_GetItem(pReturn, i), "s", &name);
					strcpy(newC->course_name,name);
					PyArg_Parse(PyList_GetItem(pReturn, ++i), "s", &name);
					strcpy(newC->course_id,name);
					//printf("%s %d\n",newC->course_name,newC->course_id);
					newC->next = user->course_list;
					newC->downList = NULL;
					newC->noteList = NULL;
					newC->infoLen = 0;
					newC->homeworkList = NULL;
					user->course_list = newC;
				}
				user->course_num = courseNum/2;
				//PyArg_Parse(pReturn, "(s,s)", &result);//i表示转换成int型变量
				//printf("%d %s\n",sizeof(result),result);
			}
			Py_Finalize();
		struct Course * nowC  = user->course_list;
		while(nowC)
		{
			printf("%s %s\n", nowC->course_name, nowC->course_id);
			nowC = nowC->next;
		}
		return 0;
}

void getAllCourseInfo()
{
	struct User *nowU = userList;
	while(nowU)
	{
		struct Course *nowC = nowU->course_list;
		while(nowC)
		{
			getCourseInfo(nowU,nowC);
			nowC = nowC->next;
		}
		nowU = nowU->next;
	}
}

int main(int argc, char *argv[])
{
	FILE *f = fopen("./learn.config","r");
	if(!f)
	{
		printf("读取用户文件learn_user.config失败！使用测试帐号！\n");
		userList = (struct User *)malloc(sizeof(struct User));
		strcpy(userList->userid,"ma-xb12");
		strcpy(userList->userpass,"Tsinghua2012");
		userList->next = NULL;
		userList->course_list = NULL;
		getCourseList(userList);
	}
	else
	{
		char userid[100], userpass[100];
		int usernum = 0;
		fscanf(f,"%s\n",bufPath);
		fscanf(f,"%s\n",pythonPath);
		fscanf(f,"%d\n",&usernum);
		printf("user num : %d\n",usernum);
		while(usernum -- > 0)
		{
			memset(userid,0,sizeof(userid));
			memset(userpass,0,sizeof(userpass));
			fscanf(f,"%s\n",userid);
			fscanf(f,"%s\n",userpass);
			//fgets(userid,sizeof(userid),f);
			//fgets(userpass,sizeof(userpass),f);
			printf("用户%s\n",userid);
			struct User * u = (struct User *)malloc(sizeof(struct User));
			strcpy(u->userid,userid);
			strcpy(u->userpass,userpass);
			u->next = userList;
			u->course_list = NULL;
			userList = u;
			getCourseList(u);
		}
	}
	getAllCourseInfo();
	return fuse_main(argc, argv, &hello_oper, NULL);
}
