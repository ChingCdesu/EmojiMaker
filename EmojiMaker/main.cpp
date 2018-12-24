#include <Windows.h>	//gdiplus.h����
#include <string>
#include "gdiplus.h"		//ͼƬ����ͷ�ļ�

#pragma comment(lib,"Gdiplus.lib")	//ͼ�δ�����ļ�

using namespace std;
using namespace Gdiplus;	//�����ռ�

/*�����ֽ��ַ���ת���ɿ��ַ�*/
/*szStrԭ���ֽ��ַ�*/
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
/*��ȡͼƬ������*/
/*formatͼƬ��ʽ  pClsidͼƬ����������*/
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

/*cmd����*/
/*���� ...\EmojiMaker.exe <����> <ͼƬID> <��>*/
int main(int agrc, char* agrv[])
{
	if (agrc != 4) return -1;
	//��ʼ��Gdiplus
	Gdiplus::GdiplusStartupInput m_GdiplusStrartupInput;
	ULONG_PTR m_GdiplusToken;
	Gdiplus::GdiplusStartup(&m_GdiplusToken, &m_GdiplusStrartupInput, NULL);
	//�������������
	const wstring charcter = AnsiToUnicode(agrv[1]);
	const wstring num = AnsiToUnicode(agrv[2]);
	const wstring str = AnsiToUnicode(agrv[3]);


	/*DebugTest*/
	//wstring charcter = L"py";
	//wstring num = L"1";
	//wstring str = L"txt";


	//ͳ�ƺ��ֺ�Ӣ���ַ��ĸ���
	UINT UnicodeNum = 0;
	UINT AscIINum = 0;
	for (size_t tmp = 0; tmp < str.length(); tmp++)
	{
		if (str[tmp] >= L' ' && str[tmp] <= L'~') AscIINum++;
		else UnicodeNum++;
	}
	//����ַ���������������˳�����
	if (UnicodeNum * 2 + AscIINum > 30) return -2;
	//ԭͼjpg��·��
	wstring path = L"A:\\_ChingC\\��Q Pro\\data\\emojis\\" + charcter + L"\\" + num + L".jpg";
	//��ͼƬΪBitmap��λͼ��
	Gdiplus::Bitmap *pic = new Gdiplus::Bitmap(path.c_str());
	//��ȡ���ȺͿ��
	UINT width = pic->GetWidth();
	UINT height = pic->GetHeight();
	//ÿ���ַ��Ĵ�С
	double CharacterSize;
	if (str.length() == 0)//���û���ַ��򷵻�ԭͼ
		CharacterSize = 0;
	else if (UnicodeNum * 2 + AscIINum <= 10)
		CharacterSize = 0.2 * width;
	else
		CharacterSize = width / str.length();
	//�½�λͼemoji����ȣ�ԭͼ�߶�+�ַ��߶ȣ��ͻ���graphic������λͼemoji��С��
	Gdiplus::Bitmap *emoji = new Gdiplus::Bitmap(width, height + CharacterSize);
	Gdiplus::Graphics *graphic = new Gdiplus::Graphics(emoji);
	//������Ϳ�ϰ�ɫ����
	graphic->Clear(Gdiplus::Color::White);
	//���û��������Ч��
	graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	//��ԭͼ����������
	graphic->DrawImage(pic, 0, 0, height, width);
	//���������ʽΪ����
	StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	//����������
	Gdiplus::FontFamily fontFamily(L"����");
	//��������
	//FontStyleRegular		�ǼӴ���б����
	//UnitPoint					���ظ�ʽ��Ⱦ
	Gdiplus::Font font(&fontFamily, 0.65 * CharacterSize, Gdiplus::FontStyleRegular,Gdiplus::Unit::UnitPoint);
	//���ú�ɫ��ˢ
	Gdiplus::SolidBrush BlackBrush(Gdiplus::Color::Color(255, 0, 0, 0));
	//�ַ�λ��
	Gdiplus::PointF CharacterStartPoint(0.5 * width, height);
	//���ַ���������
	//str.c_str()					�ַ���
	//str.length()				�ַ�������
	graphic->DrawString(str.c_str(), str.length(), &font, CharacterStartPoint, &format, &BlackBrush);
	
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);//����ͼƬ��png��ʽ��Ⱦ
	//���浽ָ��·��
	//pngClsid					��Ⱦ��
	emoji->Save(L"A:\\_ChingC\\��Q Pro\\data\\image\\emoji.png", &pngClsid, NULL);
	//�ر�Gdiplus��������ڴ���ʴ�����ͣ�ã�
	//GdiplusShutdown(m_GdiplusToken);
	return 0;
}