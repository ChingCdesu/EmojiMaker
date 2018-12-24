#include <Windows.h>	//gdiplus.h依赖
#include <string>
#include "gdiplus.h"		//图片处理头文件

#pragma comment(lib,"Gdiplus.lib")	//图形处理库文件

using namespace std;
using namespace Gdiplus;	//命名空间

/*将单字节字符串转换成宽字符*/
/*szStr原单字节字符*/
wchar_t* AnsiToUnicode(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}
/*获取图片编码器*/
/*format图片格式  pClsid图片编码器容器*/
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

/*cmd调用*/
/*方法 ...\EmojiMaker.exe <人名> <图片ID> <字>*/
int main(int agrc, char* agrv[])
{
	if (agrc != 4) return -1;
	//初始化Gdiplus
	Gdiplus::GdiplusStartupInput m_GdiplusStrartupInput;
	ULONG_PTR m_GdiplusToken;
	Gdiplus::GdiplusStartup(&m_GdiplusToken, &m_GdiplusStrartupInput, NULL);
	//将参数传入变量
	const wstring charcter = AnsiToUnicode(agrv[1]);
	const wstring num = AnsiToUnicode(agrv[2]);
	const wstring str = AnsiToUnicode(agrv[3]);


	/*DebugTest*/
	//wstring charcter = L"py";
	//wstring num = L"1";
	//wstring str = L"txt";


	//统计汉字和英文字符的个数
	UINT UnicodeNum = 0;
	UINT AscIINum = 0;
	for (size_t tmp = 0; tmp < str.length(); tmp++)
	{
		if (str[tmp] >= L' ' && str[tmp] <= L'~') AscIINum++;
		else UnicodeNum++;
	}
	//如果字符超过最大数量则退出程序
	if (UnicodeNum * 2 + AscIINum > 30) return -2;
	//原图jpg的路径
	wstring path = L"A:\\_ChingC\\酷Q Pro\\data\\emojis\\" + charcter + L"\\" + num + L".jpg";
	//打开图片为Bitmap（位图）
	Gdiplus::Bitmap *pic = new Gdiplus::Bitmap(path.c_str());
	//获取长度和宽度
	UINT width = pic->GetWidth();
	UINT height = pic->GetHeight();
	//每个字符的大小
	double CharacterSize;
	if (str.length() == 0)//如果没有字符则返回原图
		CharacterSize = 0;
	else if (UnicodeNum * 2 + AscIINum <= 10)
		CharacterSize = 0.2 * width;
	else
		CharacterSize = width / str.length();
	//新建位图emoji（宽度，原图高度+字符高度）和画布graphic（基于位图emoji大小）
	Gdiplus::Bitmap *emoji = new Gdiplus::Bitmap(width, height + CharacterSize);
	Gdiplus::Graphics *graphic = new Gdiplus::Graphics(emoji);
	//给画布涂上白色背景
	graphic->Clear(Gdiplus::Color::White);
	//设置画布抗锯齿效果
	graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	//将原图画到画布上
	graphic->DrawImage(pic, 0, 0, width, height);
	//设置字体格式为居中
	StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	//设置字体组
	Gdiplus::FontFamily fontFamily(L"黑体");
	//设置字体
	//FontStyleRegular		非加粗倾斜字体
	//UnitPoint			像素格式渲染
	Gdiplus::Font font(&fontFamily, 0.65 * CharacterSize, Gdiplus::FontStyleRegular,Gdiplus::Unit::UnitPoint);
	//设置黑色笔刷
	Gdiplus::SolidBrush BlackBrush(Gdiplus::Color::Color(255, 0, 0, 0));
	//字符位置
	Gdiplus::PointF CharacterStartPoint(0.5 * width, height);
	//画字符到画布上
	//str.c_str()			字符串
	//str.length()			字符串长度
	graphic->DrawString(str.c_str(), str.length(), &font, CharacterStartPoint, &format, &BlackBrush);
	
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);//设置图片以png格式渲染
	//保存到指定路径
	//pngClsid			渲染器
	emoji->Save(L"A:\\_ChingC\\酷Q Pro\\data\\image\\emoji.png", &pngClsid, NULL);
	//关闭Gdiplus（会出现内存访问错误，已停用）
	//GdiplusShutdown(m_GdiplusToken);
	return 0;
}
