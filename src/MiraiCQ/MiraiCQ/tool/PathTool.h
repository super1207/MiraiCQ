#pragma once

#include <string>
#include <vector>

class PathTool
{
public:
	/*
	* �������ж�Ŀ¼�Ƿ����
	* ����`dir`��Ҫ�жϵ�·��
	* ����ֵ�����ڷ���`true`,�����ڻ��޷��ж��򷵻�`false`
	*/
	static bool is_dir_exist(const std::string & dir) noexcept;

	/*
	* �������ж��ļ��Ƿ����
	* ����`file`��Ҫ�жϵ��ļ�
	* ����ֵ�����ڷ���`true`,�����ڻ��޷��ж��򷵻�`false`
	*/
	static bool is_file_exist(const std::string & file) noexcept;

	/*
	* ����������Ŀ¼ (���ɵݹ鴴��)
	* ����`dir`��Ҫ������Ŀ¼
	* ����ֵ�������ɹ�������Ŀ¼�Ѿ����ڷ���`true`,����ʧ�ܷ���`false`
	*/
	static bool create_dir(const std::string& dir) noexcept;

	/*
	* ������ɾ��Ŀ¼
	* ����`dir`��Ҫɾ����Ŀ¼
	* ����ֵ��ɾ���ɹ�������Ŀ¼ԭ���Ͳ����ڴ��ڷ���`true`,ɾ��ʧ�ܷ���`false`
	*/
	static bool del_dir(const std::string& dir) noexcept;

	/*
	* ������ɾ���ļ�
	* ����`file`��Ҫɾ�����ļ�
	* ����ֵ��ɾ���ɹ��������ļ�ԭ���Ͳ����ڷ���`true`,ɾ��ʧ�ܷ���`false`
	*/
	static bool del_file(const std::string& file) noexcept;

	/*
	* ��������ÿ�ִ���ļ���exe�����ڵ�Ŀ¼������·����
	* ����ֵ������exe����Ŀ¼(ĩβ��`'\'`)��ʧ���򷵻�`""`
	*/
	static std::string get_exe_dir() noexcept;

	/*
	* ��������ÿ�ִ���ļ���exe�����ļ���
	* ����ֵ������exe�ļ�����ʧ���򷵻�`""`
	*/
	static std::string get_exe_name() noexcept;

	/*
	* ��������ȡĿ¼�µ������ļ�
	* ����`path`��Ŀ¼
	* ����ֵ���ļ������飨ֻ���ļ���,utf8���룩
	*/
	static std::vector<std::string> get_path_file(const std::string & path) noexcept;

	/*
	* ��������ȡĿ¼�µ������ļ�
	* ����`old_name`����·��
	* ����`new_name`����·��
	* ����ֵ���ɹ�����`true`
	*/
	static bool rename(const std::string& old_name, const std::string& new_name) noexcept;



};