#ifndef _GARBAGE_CLEAN_
#define _GARBAGE_CLEAN_

#include <Public/Helper/qaxpublic.h>
#include <QAXBase/Interface/ISpecialValue.h>
#include <Public/Helper/StringUtils.h>
#include <Windows.h>
#include <io.h>
#include <list>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <vector>
#include <QCheckBox>
#include <QTableView>
#include <QLCDNumber>
#include <QStandardItemModel>

enum GarbageFileType
{
    BlankFileType = 0,
    SystemTempFile,
    InternetCache,
};

enum SignOfDeleteFile
{
    BlankSign = 0,
    DeleteSucceed,
    DeleteFailed,
    PathNoExist,
    FileOpening
};

struct FileAttrib {         //ɨ�赽���ļ�������ṹ������
    std::wstring _path;     //·��        ---->���ı�λ�ã���Ҫע��GarbageClean.cpp�ļ��е�FindAllFile����������д��
    uint32_t _size;         //�ļ���С     ---->���ı�λ�ã���Ҫע��GarbageClean.cpp�ļ��е�FindAllFile����������д��
    std::wstring _exten;    //�ļ���׺
    HANDLE _fHandle;        //�ļ��ڵ�ǰ�����еľ��
    //uint32_t _attrib;        //�ļ�����
};

class FirstGarbageClean {
public:
    //�ж��ļ��Ƿ�������ļ���
    bool IsFolder(const WIN32_FIND_DATA& fileInfo);

    bool IsNotDot(const WIN32_FIND_DATA& fileInfo);

    //��ȡϵͳ��ʱ�ļ���,����ʱ�ļ����ַ����洢��path��
    bool GetSysTempFolder(wchar_t* path);

    //��ȡ�ļ���չ������.Ϊ���֣����ļ�û�к�׺.�����ȡʧ��(һֱ���ȡ�ڴ�������������ԭ��)
    const wchar_t* GetFileExtNameFromPath(const wchar_t* lpszPathName);

    //��ȡ�ļ���չ������.Ϊ���֣�����׺�洢��buffer��
    bool GetFileExtNameFromPath(const wchar_t* lpszPathname, Base::CPathString& buffer);


private:
    ISpecialValuePtr& InternalGetSpecialValuePtr(void);

private:
    ISpecialValuePtr _svptr;
};

static FirstGarbageClean FGC;

//���ļ���Сת��Ϊ��B����KB�����ַ���
std::wstring GetFileSize(uint32_t size);

//����dirĿ¼�µ������ļ����������ļ��У������ļ�����<��׺�����Զ����ļ�����>�洢��fileBuf��
void FindAllFile(std::unordered_map<std::wstring, std::list<FileAttrib>>& fileBuf,\
    std::unordered_map<std::wstring, uint64_t>& extenFileSize, const wchar_t* dir);

//�����ļ�����ɾ���ļ������ݾ���·��ɾ���ļ�����ʹ��FileAttrib��Ϊ�βΣ����ں�����ɾ���ļ��ĸ����ж�
DWORD DeleteFileByFileAttr(FileAttrib& fileAttr, std::unordered_map<std::wstring, uint64_t>& extenFileSize);

//���ݺ�׺ɾ���ļ�
void DeleteFileByExten(std::unordered_map<std::wstring, std::list<FileAttrib>>& fileBuf,\
    std::unordered_map<std::wstring, uint64_t>& extenFileSize, const wchar_t* ex);

//ɾ�������ļ�
void DeleteAllFile(std::unordered_map<std::wstring, std::list<FileAttrib>>& fileBuf,\
    std::unordered_map<std::wstring, uint64_t>& extenFileSize);

#endif // !_GARBAGE_CLEAN_

