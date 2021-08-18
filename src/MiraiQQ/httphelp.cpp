#include "httphelp.h"

#include <afxinet.h>

static std::string& replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}

bool HttpGet( const std::string & strUrl_,const std::string & strSavePath,std::string & contentType,unsigned int timeout )
{
	unsigned short nPort;        //用于保存目标HTTP服务端口
	CString strServer, strObject;    //strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType,dwRet;        //dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号

	std::string strUrl = strUrl_;
	replace_all_distinct(strUrl,"https://","http://");

	//解析URL，获取信息
	if(!AfxParseURL(strUrl.c_str(), dwServiceType, strServer, strObject, nPort))
	{
		//解析失败，该Url不正确
		return false;
	}
	//创建网络连接对象，HTTP连接对象指针和用于该连接的HttpFile文件对象指针，注意delete
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	//建立网络连接
	CHttpConnection *pHtCon = intsess.GetHttpConnection(strServer,nPort);
	if(pHtCon == NULL)
	{
		//建立网络连接失败
		intsess.Close();
		return false;
	}
	//发起GET请求
	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject);
	if(pHtFile == NULL)
	{
		//发起GET请求失败
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		return false;
	}
	//提交请求
	pHtFile->SendRequest();
	//获取服务器返回的状态号
	pHtFile->QueryInfoStatusCode(dwRet);
	if (dwRet != HTTP_STATUS_OK)
	{
		//服务器不接受请求
		intsess.Close();
		delete pHtCon;;
		delete pHtFile;;
		return false;
	}
	//获取文件大小
	UINT nFileLen = (UINT)pHtFile->GetLength();
	DWORD dwRead = 1;        //用于标识读了多少，为1是为了进入循环
	//创建缓冲区
	CHAR *szBuffer = new CHAR[nFileLen+1];
	TRY
	{
		//创建文件
		CFile PicFile(strSavePath.c_str(),CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);
		while(dwRead>0)
		{
			//清空缓冲区
			memset(szBuffer,0,(size_t)(nFileLen+1));
			//读取到缓冲区
			dwRead = pHtFile->Read(szBuffer,nFileLen);
			//写入到文件
			PicFile.Write(szBuffer,dwRead);
		}
		//关闭文件
		PicFile.Close();
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
	}
	CATCH(CFileException,e)
	{
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
		return false;            //读写文件异常
	}
	END_CATCH
		return true;
}
