#pragma once
#pragma execution_character_set("utf-8")//��֤���ĵ�������ʾ

#include <QtWidgets/QMainWindow>
#include "GarbageClean.h"
#include "ui_Cleaner.h"
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QDebug>

class Cleaner : public QMainWindow, public FirstGarbageClean
{
	Q_OBJECT

public:
	Cleaner(QWidget* parent = Q_NULLPTR);
	~Cleaner();

	void initTree();
	//wstring GetSuffixFromItem(QCheckBox* qcb);
private slots:
	void on_startButton_clicked();
	void on_stopButtton_clicked();
	void updateTime();
	void on_deleteFileButton_clicked();

	void on_treeView_clicked(const QModelIndex& index);
	void treeItemChanged(QStandardItem* item);
	void treeItem_checkAllChild(QStandardItem* item, bool check = true);
	void treeItem_checkAllChild_recursion(QStandardItem* item, bool check = true);
	void treeItem_CheckChildChanged(QStandardItem* item);
	Qt::CheckState checkSibling(QStandardItem* item);
	//void getSelectedItemsData(QStandardItem* item);

	void on_unfold_all_clicked();
	void on_fold_all_clicked();

private:
	Ui::CleanerClass* ui;
	bool IsStop = false;//ֹͣɨ�跽��
	QTimer* timer;//��ʱ����ÿ�����ʱ��
	QTime* timeRecord;//��¼ʱ��
	bool isStart;
	std::unordered_map<std::wstring, std::list<FileAttrib>> fileBuf;//�洢ɨ�������ܶ�ط���Ҫ�õ�����������Զ��嵽����
	std::unordered_map<std::wstring, uint64_t> extenFileSize;//�洢��׺�ļ��ܴ�С
	QStandardItemModel* scanModel;//��model������������Խ����Դ�˷ѵ�����
	QStandardItemModel* model;
};
