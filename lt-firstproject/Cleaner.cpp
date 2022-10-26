#include "Cleaner.h"

Cleaner::Cleaner(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::CleanerClass)
{
	ui->setupUi(this);
	isStart = false;

	ui->clear_all->setEnabled(false);
	ui->select_all->setEnabled(false);
	ui->unselect->setEnabled(false);

	timer = new QTimer;
	timeRecord = new QTime(0, 0, 0);
	ui->time->setDigitCount(8);
	ui->time->setSegmentStyle(QLCDNumber::Flat);
	ui->time->display(timeRecord->toString("hh:mm:ss"));

	connect(ui->start, SIGNAL(clicked()), this, SLOT(on_startButton_clicked()));
	connect(ui->stop, SIGNAL(clicked()), this, SLOT(on_stopButtton_clicked()));
	connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
	connect(ui->clear_all, SIGNAL(clicked()), this, SLOT(on_deleteFileButton_clicked()));
	connect(ui->unfold_all, SIGNAL(clicked), this, SLOT(on_unfold_all_clicked()));
	connect(ui->fold_all, SIGNAL(clicked), this, SLOT(on_fold_all_clicked()));
	//connect(ui-)
	ui->treeView->setSortingEnabled(true);
	ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	model = new QStandardItemModel(ui->treeView);
}

Cleaner::~Cleaner()
{
	delete ui;
}


void Cleaner::on_startButton_clicked()
{
	if (!isStart)
	{
		timer->start(1000);
	}
	else
	{
		timer->stop();
	}	
	if (model->rowCount() != 0)
	{
		model->clear();
		initTree();
	}
	else
	{
		initTree();
	}
	

	timer->stop();
	isStart = !isStart;
	ui->clear_all->setEnabled(true);
	ui->select_all->setEnabled(true);
	ui->unselect->setEnabled(true);
}

void Cleaner::on_stopButtton_clicked()
{
	IsStop = true;
	
	timer->stop();    //��ʱ��ֹͣ
	timeRecord->setHMS(0, 0, 0); //ʱ����Ϊ0
	ui->time->display(timeRecord->toString("hh:mm:ss")); //��ʾ00:00:00
	isStart = false;
}

void Cleaner::updateTime()
{
	*timeRecord = timeRecord->addSecs(1);
	ui->time->display(timeRecord->toString("hh:mm:ss"));
}

void Cleaner::on_deleteFileButton_clicked()
{
	//����checkbox��ѡ�����ɾ���ļ�
	/*
    std::vector< QCheckBox*> extenCheckBox(5);
    extenCheckBox[0] = ui->checkBox1;
    extenCheckBox[1] = ui->checkBox2;
    extenCheckBox[2] = ui->checkBox3;
    extenCheckBox[3] = ui->checkBox4;
    extenCheckBox[4] = ui->checkBox5;
	for (auto& cb : extenCheckBox)
	{
		if (cb->isChecked())
		{
			std::wstring suffix = GetSuffixFormCheckBox(cb);
			if (wcscmp(suffix.c_str(), L"***"))//��ѡ��Ĳ���***ѡ���ִ��ɾ������
			{
                DeleteFileByExten(fileBuf, extenFileSize, suffix.c_str());
			}
		}
	}
	UpdateCheckBoxText(extenCheckBox, fileBuf, extenFileSize);
	UpdateTableViewText(ui->tableView, ui->fCountN, scanModel, fileBuf);
	*/
	
}

void Cleaner::initTree()
{
	//model = new QStandardItemModel(ui->treeView);
	model->setHorizontalHeaderLabels(QStringList() << "�ļ���Ϣ" << "��С");

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", \
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	fileBuf.clear();
	extenFileSize.clear();
	FindAllFile(fileBuf, extenFileSize, dir.toStdWString().c_str());
	auto suffixSize = 0;
	int fNCount = 0;

	for (auto& [k, v] : fileBuf)
	{
		QStandardItem* itemSuffix = new QStandardItem(QIcon(":/res/folder.png"), QString::fromStdWString(k));
		itemSuffix->setCheckable(true);
		itemSuffix->setTristate(true);
		model->appendRow(itemSuffix);
		for (auto& it : v)
		{
			auto fSize = it._size;
			QStandardItem* itemFileName = new QStandardItem(QString::fromStdWString(it._path));
			itemFileName->setCheckable(true);
			//itemFileName->setTristate(true);
			QStandardItem* itemFileSize = new QStandardItem(QString::fromStdWString(GetFileSize(fSize)));
			itemSuffix->appendRow(itemFileName);
			itemSuffix->setChild(itemFileName->index().row(), 1, itemFileSize);
			suffixSize += fSize;
			fNCount++;
			
		}
		QStandardItem* itemSumSuffixSize = new QStandardItem(QString::fromStdWString(GetFileSize(suffixSize)));
		model->setItem(itemSuffix->row(), 1, itemSumSuffixSize);
	};
	ui->fCountN->display(fNCount);
	connect(model, &QStandardItemModel::itemChanged, this, &Cleaner::treeItemChanged);
	ui->treeView->setModel(model);
}

void Cleaner::on_treeView_clicked(const QModelIndex& index)
{
	QString str;
	str += QStringLiteral("��ǰѡ�У�%1\nrow:%2,column:%3\n").arg(index.data().toString())
		.arg(index.row()).arg(index.column());
	str += QStringLiteral("������%1\n").arg(index.parent().data().toString());
	QString name, info;
	if (index.column() == 0)
	{
		name = index.data().toString();
		info = index.sibling(index.row(), 1).data().toString();
	}
	str += QStringLiteral("���ƣ�%1\n��Ϣ��%2\n").arg(name).arg(info);
}

void Cleaner::treeItemChanged(QStandardItem* item)
{
	if (item == nullptr)
		return;
	if (item->isCheckable())
	{
		model->blockSignals(true);
		Qt::CheckState state = item->checkState();
		if (item->isTristate())
		{
			//�����ѡ��״̬
			if (state != Qt::PartiallyChecked)
			{
				treeItem_checkAllChild(item, state == Qt::Checked ? true : false);
			}
		}
		else
		{
			treeItem_CheckChildChanged(item);

		}
		model->blockSignals(false);
	}
	ui->treeView->update();
}

//�ӽڵ�ݹ�ȫѡ
void Cleaner::treeItem_checkAllChild(QStandardItem* item, bool check)
{
	if (item == nullptr)
		return;
	int rowCount = item->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		QStandardItem* childItems = item->child(i);
		treeItem_checkAllChild_recursion(childItems, check);
	}
	if (item->isCheckable())
	{
		item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
		
	}
}

//�ݹ麯�����������νڵ�������ӽڵ�
void Cleaner::treeItem_checkAllChild_recursion(QStandardItem* item, bool check)
{
	if (item == nullptr)
		return;
	int rowCount = item->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		QStandardItem* childItems = item->child(i);
		treeItem_checkAllChild_recursion(childItems, check);
	}
	if (item->isCheckable())
	{
		item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
	}
		
}

//�жϵ�ǰ���ֵܽڵ�ľ������
Qt::CheckState Cleaner::checkSibling(QStandardItem* item)
{
	//��ͨ�����ڵ��ȡ�ֵܽڵ�
	QStandardItem* parent = item->parent();
	if (nullptr == parent)
		return item->checkState();
	int brotherCount = parent->rowCount();
	int checkedCount(0), unCheckedCount(0);
	Qt::CheckState state;
	for (int i = 0; i < brotherCount; ++i)
	{
		QStandardItem* siblingItem = parent->child(i);
		state = siblingItem->checkState();
		if (Qt::PartiallyChecked == state)
			return Qt::PartiallyChecked;
		else if (Qt::Unchecked == state)
			++unCheckedCount;
		else
			++checkedCount;
		if (checkedCount > 0 && unCheckedCount > 0)
			return Qt::PartiallyChecked;
	}
	if (unCheckedCount > 0)
		return Qt::Unchecked;
	return Qt::Checked;
}

//���ڵ�ݹ鴦��
void Cleaner::treeItem_CheckChildChanged(QStandardItem* item)
{
	if (nullptr == item)
		return;
	Qt::CheckState siblingState = checkSibling(item);
	QStandardItem* parentItem = item->parent();
	if (nullptr == parentItem)
		return;
	if (Qt::PartiallyChecked == siblingState)
	{
		if (parentItem->isCheckable() && parentItem->isTristate())
			parentItem->setCheckState(Qt::PartiallyChecked);
	}
	else if (Qt::Checked == siblingState)
	{
		if (parentItem->isCheckable())
			parentItem->setCheckState(Qt::Checked);
	}
	else
	{
		if (parentItem->isCheckable())
			parentItem->setCheckState(Qt::Unchecked);
	}
	treeItem_CheckChildChanged(parentItem);
}

/*
void Cleaner::getSelectedItemsData(QStandardItem* item)
{
	Qt::CheckState state = item->checkState();
	//�����ѡ��״̬
	if (state != Qt::PartiallyChecked)
	{
		//�ж��Ƿ����ӽڵ�
		if (!item->isTristate())
		{
			item->setSelectable(true);
		}
	}
}
*/

//std::wstring Cleaner:: GetSuffixFromItem(QCheckBox* qcb)
//{
//    std::wstring text = qcb->text().toStdWString();
//    wchar_t* pos = (wchar_t*)wcschr(text.c_str(), L' ');
//    if (pos != nullptr)
//    {
//        *pos = 0;
//    }
//    return text;
//}

void Cleaner::on_unfold_all_clicked()
{
	ui->treeView->expandAll();
}

void Cleaner::on_fold_all_clicked()
{
	ui->treeView->collapseAll();
}