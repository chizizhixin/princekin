#include "urltoxml.h"
#include "ui_urltoxml.h"

UrlToXml::UrlToXml(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UrlToXml)
{
    ui->setupUi(this);
    this->setFixedWidth(689);
    setWindowTitle("URL生成XML文件");
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

    ui->errorLabel->setText("");
    ui->changeBtn->setEnabled(false);

    ui->statisticsTW->setRowCount(8);
    ui->statisticsTW->setColumnCount(4);//要在下边设置表头的前边，表头才能显示出来

    ui->statisticsTW->setHorizontalHeaderLabels(QStringList() << "参数名"<<"参数值"<<"描述"<<"验证规则");
    ui->statisticsTW->verticalHeader()->setVisible(false); // 隐藏水平header
    ui->statisticsTW->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
    ui->statisticsTW->setColumnWidth(0,150);
    ui->statisticsTW->setColumnWidth(1,150);
    ui->statisticsTW->setColumnWidth(2,150);
    ui->statisticsTW->setStyleSheet("QTableWidget{border: 1px solid #ffffff;}");

    for(int i=0;i<8;i++)
    {
        ui->statisticsTW->setItem(i,0,new QTableWidgetItem(""));//前两栏的也new个实例
        ui->statisticsTW->setItem(i,1,new QTableWidgetItem(""));
        ui->statisticsTW->setItem(i,2,new QTableWidgetItem(""));//这里先把描述这一栏new一个实例，否则以后取值的时候会报错

        QComboBox *comBox = new QComboBox();
        comBox->addItem("不验证");
        comBox->addItem("验证非空");
        comBox->addItem("需要验证");
        comBox->addItem("关键识别值");
        comBox->setStyleSheet("QComboBox QAbstractItemView::item{height:25px;}"
                              "QComboBox {border: 1px solid #e4e4e4;  border-radius: 3px;padding: 1px 2px 1px 2px;min-width: 9em;}"
                              "QComboBox::drop-down{ width: 40px; border-left-style: none;border-top-right-radius: 3px;  border-bottom-right-radius: 3px;}"
                              "QComboBox::down-arrow{image:url(:/arrow.png);}");
        comBox->setView(new QListView());

        ui->statisticsTW->setCellWidget(i,3,comBox);

        connect(comBox,SIGNAL(currentIndexChanged(int)),this,SLOT(CalculateUnique(int)));
    }
    ui->lineEdit->installEventFilter(this);
    ui->xmlNameLineEdit->installEventFilter(this);
    ui->domainNameLineEdit->installEventFilter(this);

    ui->openFileBtn->setStyleSheet("QPushButton{border-image:url(:/checkfile.png);border-radius:5px;}"
                                   "QPushButton:hover{border-image:url(:/checkfile_hover.png);}");
    ui->changeBtn->setStyleSheet("QPushButton{border-image:url(:/create.png);border-radius:5px;}"
                                 "QPushButton:hover{border-image:url(:/create_hover.png);}");
    ui->statisticsTW->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                         "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->fileTableWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                            "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");

    ui->fileTableWidget->horizontalHeader()->setStretchLastSection(true);//最后一列自适应宽度
    ui->fileTableWidget->setColumnWidth(0,75);
    ui->fileTableWidget->setColumnWidth(1,75);
    ui->fileTableWidget->setRowCount(1);
    for(int i=0;i<ui->fileTableWidget->columnCount();i++)
        ui->fileTableWidget->setItem(0,i,new QTableWidgetItem(""));//前两栏的也new个实例

    ui->fileTableWidget->verticalHeader()->setVisible(false); // 隐藏水平header

    ui->multiCBox->setToolTip("可批量生成关键识别值所对应的不同参数值的批量xml文件，其他参数验证规则与下表一致");
    ui->downLoadExcelBtn->setToolTip("下载可导入文件名及参数值的Excel模板");
    ui->importExcelBtn->setToolTip("将按照Excel模板制作的Excel数据导入至下方表格");

    xmlPath = gWorkSpace+QDir::separator()+"StatisticalXML";
    rightXmlPath = gWorkSpace+QDir::separator()+"StatisticalXML";
    QDir *temp=new QDir;
    bool exist=temp->exists(xmlPath);
    if(!exist)
    {
        temp->mkpath(xmlPath);
    }
    ui->xmlPathLineEdit->setText(xmlPath);
}


/*void UrlToXml::CalculateUnique(int index)
{
    uniqueNum=0;
    uniqueList.clear();
    for(int i=0;i<ui->statisticsTW->rowCount();i++)
    {
        QWidget * widget=ui->statisticsTW->cellWidget(i,3);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        int index=combox->currentIndex();
        if(index==3)//唯一识别值
        {
            if(uniqueNum<2)
            {
                uniqueNum++;
                uniqueList.append(ui->statisticsTW->item(i,0)->text()+"="+ui->statisticsTW->item(i,1)->text());
                if(uniqueNum==1)
                {
                    ui->fileTableWidget->setColumnCount(3);
                    QTableWidgetItem *item=new QTableWidgetItem(ui->statisticsTW->item(i,0)->text());
                    ui->fileTableWidget->setHorizontalHeaderItem(1,item);
                    item=new QTableWidgetItem("描述");
                    ui->fileTableWidget->setHorizontalHeaderItem(2,item);
                }
                else if(uniqueNum==2)
                {
                    ui->fileTableWidget->setColumnCount(4);
                    QTableWidgetItem *item=new QTableWidgetItem(ui->statisticsTW->item(i,0)->text());
                    ui->fileTableWidget->setHorizontalHeaderItem(2,item);
                    item=new QTableWidgetItem("描述");
                    ui->fileTableWidget->setHorizontalHeaderItem(3,item);
                }
            }
            else
            {
                combox->setCurrentIndex(0);
                QMessageBox::information(this,"提示","关键识别值最多两个,请重新选择");
                return;
            }
        }
    }
    if(uniqueNum==0)
    {
        ui->fileTableWidget->setColumnCount(3);
        QTableWidgetItem *item=new QTableWidgetItem("参数值");
        ui->fileTableWidget->setHorizontalHeaderItem(1,item);
        item=new QTableWidgetItem("描述");
        ui->fileTableWidget->setHorizontalHeaderItem(2,item);
    }
}*/

/*
 * 如果变化后的index是3，即某个选项被选为关键识别值，这时找到发送信号的combox,并在右侧显示；
 * 如果变化后的index不是3，需要对整个列表进行遍历，计算目前有几个关键识别值
 * 没有关键识别值时，右侧表格文案需要变成“参数值”，只有一个关键识别值时右侧表格也要重新画
 */
void UrlToXml::CalculateUnique(int index)
{

    if(index==3)
        uniqueNum++;
    else
    {
        uniqueNum=0;
        uniqueList.clear();
    }

    QString oneUniqueStr="";
    QObject *object = QObject::sender();
    QComboBox *senderBox = static_cast<QComboBox *>(object);
    for(int i=0;i<ui->statisticsTW->rowCount();i++)
    {
        QWidget * widget=ui->statisticsTW->cellWidget(i,3);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        if(index==3)
        {
            if(senderBox==combox)
            {
                if(uniqueNum<=2)
                {
                    uniqueList.append(ui->statisticsTW->item(i,0)->text()+"="+ui->statisticsTW->item(i,1)->text());
                    if(uniqueNum==1)
                    {
                        ui->fileTableWidget->setColumnCount(3);
                        QTableWidgetItem *item=new QTableWidgetItem(ui->statisticsTW->item(i,0)->text());
                        ui->fileTableWidget->setHorizontalHeaderItem(1,item);
                        item=new QTableWidgetItem("描述");
                        ui->fileTableWidget->setHorizontalHeaderItem(2,item);

                    }
                    else if(uniqueNum==2)
                    {
                        ui->fileTableWidget->setColumnCount(4);
                        //以下这一句很重要，变成四列之后，第一行的最后一个表格还没被初始化，所以，取其值的时候会死掉
                        ui->fileTableWidget->setItem(0,3,new QTableWidgetItem(""));
                        QTableWidgetItem *item=new QTableWidgetItem(ui->statisticsTW->item(i,0)->text());
                        ui->fileTableWidget->setHorizontalHeaderItem(2,item);
                        item=new QTableWidgetItem("描述");
                        ui->fileTableWidget->setHorizontalHeaderItem(3,item);
                    }
                }
                else
                {
                    combox->setCurrentIndex(0);
                    QMessageBox::information(this,"提示","关键识别值最多两个,请重新选择");
                    return;
                }
            }
        }
        else
        {
            if(combox->currentIndex()==3)
            {
                uniqueNum++;
                uniqueList.append(ui->statisticsTW->item(i,0)->text()+"="+ui->statisticsTW->item(i,1)->text());
                oneUniqueStr=ui->statisticsTW->item(i,0)->text();
            }
        }
    }
    if(index!=3)
    {
        if(uniqueNum==0)
        {
            ui->fileTableWidget->setColumnCount(3);
            QTableWidgetItem *item=new QTableWidgetItem("参数值");
            ui->fileTableWidget->setHorizontalHeaderItem(1,item);
            item=new QTableWidgetItem("描述");
            ui->fileTableWidget->setHorizontalHeaderItem(2,item);
        }
        else if(uniqueNum==1)
        {
            ui->fileTableWidget->setColumnCount(3);
            QTableWidgetItem *item=new QTableWidgetItem(oneUniqueStr);
            ui->fileTableWidget->setHorizontalHeaderItem(1,item);
            item=new QTableWidgetItem("描述");
            ui->fileTableWidget->setHorizontalHeaderItem(2,item);
        }
    }

    qDebug()<<"uniqueNum: "<<uniqueNum<<"  "<<uniqueList.count();

}

UrlToXml::~UrlToXml()
{
    delete ui;
}

QString UrlToXml::UrlDecode(QString str)
{
    if(str.isEmpty())//两种方法都可以对url解码
    {
        return "";
    }
    QByteArray byteArrayGB2312 = str.toUtf8();
    QByteArray byteArrayPercentEncoded = QByteArray::fromPercentEncoding(byteArrayGB2312);
    return QString(byteArrayPercentEncoded);
}

void UrlToXml::on_lineEdit_textChanged(const QString &arg)
{
    //GET /mobiledata_test/mcc/mc.gif?uid=995755d80cd5d8d5f274bd70cfbae8a4&tkey=3b4240a97ff064709506e3d058f49654a93e86be&vid=&tvid=&mtype=6&mfov=vivo+Xplay3S&memo={"Android_id":"2bf359cf2109c548","IMEI":"864394026306011","isroot":"1","status":"1","CPUSerialNumber":"","IMSI":"","push_status":1,"MAC":"80:41:4e:a0:f9:df","HW_SN":"vivoXplay3S9bb30df4"}&passport=&ltype=&cv=6.3.0&mos=2&mosv=4.4.2&pro=1&mfo=BBK&webtype=WiFi&time=1488945930007&channelid=93&sim=0&playlistid=&catecode=&preid=&newuser=0&enterid=0&startid=1488945885175&loc=&mnc=-2&build=02232253&url=1002&type=1&value=&androidId=2bf359cf2109c548&imei=864394026306011&timer=152574e2f97267a1b51747c9cdd4765a516b770d&xid=UwS0TK2YuDIMfvikwO7iHmDEdsmGDohFUpmgiD61igy6Bzr2gycxs1JvQBrFBcFIan37L0KAzFvuvnApKm2aAgOygi0pmiqdoRGcqkx1yQOuI8pVNejEGorm2XhG2wgjdzSLhX+GUMvVQSMqCbN/ILORm0DUGuvJBpcBnMMPHYw= HTTP/1.1
    ui->statisticsTW->clearContents();
    ui->statisticsTW->setRowCount(0);
    ui->changeBtn->setEnabled(false);

    QString url=UrlDecode(ui->lineEdit->text());
    if(url=="")
    {
        return;
    }

    url=url.trimmed();//有的连接末尾有多个空格或回车
    if(url.contains("GET"))
        url=url.mid(url.indexOf("GET"));
    qDebug()<<"去掉get之前的"<<url;
    //获取并去掉域名
    domainName=url.left(url.indexOf("?")+1);
    if(domainName=="")
    {
        ui->errorLabel->setText("链接无法识别");
        return;
    }
    ui->errorLabel->setText("");

    if(ui->domainNameLineEdit->text()=="识别链接内域名")
    {
        ui->domainNameLineEdit->setText(domainName);
        ui->domainNameLineEdit->removeEventFilter(this);
        ui->domainNameLineEdit->setStyleSheet("QLineEdit{color: #000000;}");
    }
    else
        ui->domainNameLineEdit->setText(domainName);
    url=url.mid(url.indexOf("?")+1);
    //去掉最后的HTTP/1.1
    if(url.contains("HTTP"))
    {
        url=url.left(url.indexOf("HTTP")).trimmed();
    }
    //如果URL以&结尾，需要把&去掉
    while(url.endsWith("&"))
    {
        url=url.left(url.length()-1);
    }
    statisticList=url.split("&");

    if(statisticList.length()>1)
        ui->statisticsTW->setRowCount(statisticList.length());
    ui->changeBtn->setEnabled(true);
    for(int i=0;i<statisticList.length();i++)
    {
        ui->statisticsTW->setItem(i,2,new QTableWidgetItem(""));//这里先把描述这一栏new一个实例，否则以后取值的时候会报错

        QComboBox *comBox = new QComboBox();
        comBox->addItem("不验证");
        comBox->addItem("验证非空");
        comBox->addItem("需要验证");
        comBox->addItem("关键识别值");
        comBox->setStyleSheet("QComboBox QAbstractItemView::item{height:25px;}"
                              "QComboBox {border: 1px solid #e4e4e4;  border-radius: 3px;padding: 1px 2px 1px 2px;min-width: 9em;}"
                              "QComboBox::drop-down{ width: 40px; border-left-style: none;border-top-right-radius: 3px;  border-bottom-right-radius: 3px;}"
                              "QComboBox::down-arrow{image:url(:/arrow.png);}");
        comBox->setView(new QListView());

        ui->statisticsTW->setCellWidget(i,3,comBox);
        connect(comBox,SIGNAL(currentIndexChanged(int)),this,SLOT(CalculateUnique(int)));
    }

    for(int i=0;i<statisticList.length();i++)
    {
        qDebug()<<statisticList.at(i);
        QStringList tmpList=statisticList.at(i).split("=");
        if(tmpList.length()<=1)
        {
            continue;
        }
        ui->statisticsTW->setItem(i,0,new QTableWidgetItem(tmpList.at(0)));
        ui->statisticsTW->setItem(i,1,new QTableWidgetItem(tmpList.at(1)));
    }
}

int UrlToXml::saveXml(QString sXmlFilePath)
{
    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("data");
    root.setAttribute("readme","xml文件");
    doc.appendChild(root);
    QDomElement note = doc.createElement("data");
    note.setAttribute("behaviour_debug",domainName);
    note.setAttribute("unique",uniqueStr);
    root.appendChild(note);
    for(int i=0;i<ui->statisticsTW->rowCount();i++)
    {
        if(verifyList[i]==0)
        {
            QDomElement data = doc.createElement("data");
            QDomElement uid=doc.createElement(ui->statisticsTW->item(i,0)->text());
            uid.setAttribute("des",ui->statisticsTW->item(i,2)->text());
            QDomText node_text;
            uid.setAttribute("onlyCheck","N");
            node_text = doc.createTextNode("unknown");
            uid.appendChild(node_text);
            data.appendChild(uid);
            root.appendChild(data);
        }
        else if(verifyList[i]==1)
        {
            QDomElement data = doc.createElement("data");
            QDomElement uid=doc.createElement(ui->statisticsTW->item(i,0)->text());
            uid.setAttribute("des",ui->statisticsTW->item(i,2)->text());
            QDomText node_text;

            uid.setAttribute("onlyCheck","N");
            node_text = doc.createTextNode("unnull");
            uid.appendChild(node_text);
            data.appendChild(uid);
            root.appendChild(data);
        }
        else if(verifyList[i]==3||verifyList[i]==2)
        {
            QDomElement data = doc.createElement("data");
            QString textStr=ui->statisticsTW->item(i,1)->text();
            if(textStr.contains(";;"))
            {
                QStringList texts=textStr.split(";;");
                for(int j=0;j<texts.count();j++)
                {
                    QStringList desList=ui->statisticsTW->item(i,2)->text().split(";;");
                    QDomElement uid=doc.createElement(ui->statisticsTW->item(i,0)->text());
                    if(desList.count()-1>=j)
                        uid.setAttribute("des",desList[j]);
                    else
                        uid.setAttribute("des",desList[desList.length()-1]);
                    QDomText node_text;
                    uid.setAttribute("onlyCheck","Y");
                    node_text = doc.createTextNode(texts[j]);
                    uid.appendChild(node_text);
                    data.appendChild(uid);
                }
            }
            else
            {
                QDomElement uid=doc.createElement(ui->statisticsTW->item(i,0)->text());
                uid.setAttribute("des",ui->statisticsTW->item(i,2)->text());
                QDomText node_text;
                uid.setAttribute("onlyCheck","Y");
                node_text = doc.createTextNode(textStr);
                uid.appendChild(node_text);
                data.appendChild(uid);
            }
            root.appendChild(data);
        }
    }
    if(sXmlFilePath.mid(sXmlFilePath.length()-4)!=".xml")
        sXmlFilePath+=".xml";
    QFile file(sXmlFilePath);
    if(file.exists())
    {
        QMessageBox::StandardButton rb = QMessageBox::question(NULL, "提示", sXmlFilePath+"已存在，是否覆盖?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            file.remove();
        }
        else
            return 0;
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
    {
        errorFlag=false;
        QMessageBox::information(this,"提示",sXmlFilePath+"文件生成失败");
        return 0;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    doc.save(out,4,QDomNode::EncodingFromTextStream);

    file.close();

    return 0;

}

void UrlToXml::on_changeBtn_clicked()
{
    if(ui->lineEdit->text()=="")
    {
        QMessageBox::information(this,"提示","请输入URL连接");
        return;
    }
    if(!isMultiPattern)
    {
        if(ui->xmlNameLineEdit->text()==""||ui->xmlNameLineEdit->text()=="编辑生成XML文件名")
        {
            QMessageBox::information(this,"提示","请输入文件名");
            return;
        }
    }
    else
    {
        if(ui->fileTableWidget->rowCount()==1)
        {
            QMessageBox::information(this,"提示","请输入批量生成的文件名及参数值");
            return;
        }
    }

    repeatFlag=false;
    errorFlag=true;
    verifyList.clear();
    descList.clear();

    for(int i=0;i<ui->statisticsTW->rowCount();i++)
    {
        QWidget * widget=ui->statisticsTW->cellWidget(i,3);//获得widget
        QComboBox *combox=(QComboBox*)widget;//强制转化为QComboBox
        int index=combox->currentIndex();
        verifyList.append(index);
        descList.append(ui->statisticsTW->item(i,2)->text());
    }
    if(uniqueNum<1)
    {
        if(ui->statisticsTW->rowCount()>1)//统计点有的时候，才需要唯一识别值
        {
            QMessageBox::information(this,"提示","请选择一个或两个关键识别值");
            return;
        }
    }

    if(uniqueList.count()==1)
    {
        uniqueStr=uniqueList.at(0);
    }
    else if(uniqueList.count()==2)
    {
        uniqueStr=uniqueList.at(0)+";;"+uniqueList.at(1);
    }

    if(!isMultiPattern)
        saveXml(xmlPath+QDir::separator()+ui->xmlNameLineEdit->text());
    else
    {
        saveXml(xmlPath+QDir::separator()+"model.xml");//先生成一个模板，按照url里内容
        //生成model文件后先检查下该文件是否有问题，如有问题，怎不进行批量的复制，直接中断生成，并return
        QFile file(xmlPath+QDir::separator()+"model.xml");
        QDomDocument document;
        QString error;
        int row = 0, column = 0;
        if(!document.setContent(&file, false, &error, &row, &column))
        {
            QMessageBox::information(NULL, QString("提示"), QString("解析文件错误，行： ") + QString::number(row, 10) + QString(" ,列： ") + QString::number(column, 10));
            errorFlag=false;
            file.close();
            QFile ff(xmlPath+QDir::separator()+"model.xml") ;//删除模板
            if(ff.exists())
                ff.remove();
            ff.close();
            QMessageBox::information(this,"提示","文件生成失败");
            return;
        }
        else
            file.close();
        bool allReplace=false;
        for(int i=0;i<ui->fileTableWidget->rowCount()-1;i++)//再对model进行复制，按照右侧表格生成新的
        {
            QString newFilePath=xmlPath+QDir::separator()+ui->fileTableWidget->item(i,0)->text();
            if(newFilePath.mid(newFilePath.length()-4)!=".xml")
                newFilePath+=".xml";
            newFilePath=QDir::toNativeSeparators(newFilePath);
            QFile f(newFilePath);
            if(f.exists())
            {
                if(!allReplace)
                {
                    QMessageBox::StandardButton rb = QMessageBox::question(NULL, "提示", newFilePath+"已存在，是否覆盖?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                    if(rb == QMessageBox::Yes)
                    {
                        f.remove();
                        int numCover=0;
                        //检测一共有多少个重复的文件
                        for(int j=i+1;j<ui->fileTableWidget->rowCount()-1;j++)
                        {
                            QString newFilePath=xmlPath+QDir::separator()+ui->fileTableWidget->item(j,0)->text();
                            if(newFilePath.mid(newFilePath.length()-4)!=".xml")
                                newFilePath+=".xml";
                            QFile f(newFilePath);
                            if(f.exists())
                            {
                                numCover++;
                            }
                        }
                        if(numCover>0)
                        {
                            QMessageBox::StandardButton rb1 = QMessageBox::question(NULL, "提示", "是否对以后的 "+QString::number( numCover)+" 个已存在文件全部执行覆盖操作？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                            if(rb1==QMessageBox::Yes)
                                allReplace=true;
                        }
                    }
                    else
                        continue;
                }
                else
                    f.remove();
            }

            //如果右侧列表的关键识别值没有赋值，此处给出提示
            if(ui->fileTableWidget->columnCount()==4)
            {
                if(ui->fileTableWidget->item(i,1)->text()==""&&ui->fileTableWidget->item(i,2)->text()=="")
                {
                    QMessageBox::information(this,"提示",newFilePath+"文件需要至少一个关键识别值！");
                    continue;
                }
            }
            else if(ui->fileTableWidget->columnCount()==3)
            {
                if(ui->fileTableWidget->item(i,1)->text()=="")
                {
                    QMessageBox::information(this,"提示",newFilePath+"文件需要至少一个关键识别值！");
                    continue;
                }
            }
            bool flag=QFile::copy(xmlPath+QDir::separator()+"model.xml",newFilePath);
            Q_UNUSED(flag);//可打印错误信息
            errorFlag&=flag;
            if(!flag)
                QMessageBox::information(this,"提示",newFilePath+"文件生成失败！");
            else
            {
                QString uniquestr="";
                QString nullnote="";//批量生成时，如果有两个关键识别值，而右侧只写了一直关键识别值，那么另外一个关键识别值就算是nullnote，要在xml中将onlycheck改成N，值改成unknown
                if(ui->fileTableWidget->columnCount()==3)
                    uniquestr=ui->fileTableWidget->horizontalHeaderItem(1)->text() +"="+ ui->fileTableWidget->item(i,1)->text();
                else
                {
                    if(ui->fileTableWidget->item(i,1)->text().trimmed()!="")//判断当前字段有没有值，如果没有，那么关键识别值就不要这个字段
                        uniquestr+=ui->fileTableWidget->horizontalHeaderItem(1)->text() +"="+ ui->fileTableWidget->item(i,1)->text();
                    else
                        nullnote=ui->fileTableWidget->horizontalHeaderItem(1)->text();
                    if(ui->fileTableWidget->item(i,2)->text().trimmed()!="")
                    {
                        if(uniquestr!="")
                            uniquestr+=";;";
                        uniquestr+=ui->fileTableWidget->horizontalHeaderItem(2)->text() +"="+ ui->fileTableWidget->item(i,2)->text();
                    }
                    else
                        nullnote=ui->fileTableWidget->horizontalHeaderItem(2)->text();
                }

                changeUnique(newFilePath,uniquestr,ui->fileTableWidget->item(i,ui->fileTableWidget->columnCount()-1)->text(),nullnote);
            }

        }
        QFile ff(xmlPath+QDir::separator()+"model.xml") ;//删除模板
        if(ff.exists())
            ff.remove();
    }
    if(errorFlag)
        QMessageBox::information(this,"提示","文件生成成功");
}

bool UrlToXml::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->lineEdit)
    {
        if(event->type()==QEvent::MouseButtonPress)
        {
            ui->lineEdit->setText("");
            ui->lineEdit->removeEventFilter(this);
            ui->lineEdit->setStyleSheet("QLineEdit{color: #000000;}");
        }
    }
    if(watched==ui->xmlNameLineEdit)
    {
        if(event->type()==QEvent::MouseButtonPress)
        {
            ui->xmlNameLineEdit->setText("");
            ui->xmlNameLineEdit->removeEventFilter(this);
            ui->xmlNameLineEdit->setStyleSheet("QLineEdit{color: #000000;}");
        }
    }
    if(watched==ui->domainNameLineEdit)
    {
        if(event->type()==QEvent::MouseButtonPress)
        {
            if(ui->domainNameLineEdit->text()=="识别链接内域名")
            {
                ui->domainNameLineEdit->setText("");
                ui->domainNameLineEdit->removeEventFilter(this);
                ui->domainNameLineEdit->setStyleSheet("QLineEdit{color: #000000;}");
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

void UrlToXml::on_openFileBtn_clicked()
{
    ExeCmd::openFolder(xmlPath);
}


void UrlToXml::on_multiCBox_clicked()
{
    if(ui->multiCBox->isChecked())
    {
        isMultiPattern=true;
        this->setFixedWidth(1022);
        ui->lineEdit_2->setEnabled(false);
        ui->xmlNameLineEdit->setEnabled(false);
    }
    else
    {
        isMultiPattern=false;
        this->setFixedWidth(689);
        ui->lineEdit_2->setEnabled(true);
        ui->xmlNameLineEdit->setEnabled(true);
    }
}

void UrlToXml::on_browseBtn_clicked()
{
    xmlPath=QFileDialog::getExistingDirectory(this, tr("选择文件夹"),xmlPath,QFileDialog::ShowDirsOnly);
    ui->xmlPathLineEdit->setText(xmlPath);
}

void UrlToXml::on_fileTableWidget_itemChanged(QTableWidgetItem *item)
{
    int count=ui->fileTableWidget->rowCount();
    if(ui->fileTableWidget->item(count-1,0)->text()!="")
    {
        disconnect(ui->fileTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_fileTableWidget_itemChanged(QTableWidgetItem*)));
        ui->fileTableWidget->setRowCount(count+1);
        for(int i=0;i<ui->fileTableWidget->columnCount();i++)
            ui->fileTableWidget->setItem(count,i,new QTableWidgetItem(""));

        connect(ui->fileTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_fileTableWidget_itemChanged(QTableWidgetItem*)));
    }
    else if(item->column()==0 && ui->fileTableWidget->item(item->row(),0)->text()=="" && ui->fileTableWidget->rowCount()>1)
        ui->fileTableWidget->removeRow(item->row());
}

void UrlToXml::on_downLoadExcelBtn_clicked()
{
    QString FilePathName=gConfigDir + QDir::separator() + "批量生成xml_Excel模板.xlsx";
    QString saveFileName = QFileDialog::getSaveFileName(this,"保存文件","批量生成xml_Excel模板.xlsx","office(*.xlsx)");

    if( saveFileName.isEmpty() ) return;
    QFile file(saveFileName);

    bool copy_error =  file.copy( FilePathName,saveFileName );
    Q_UNUSED(copy_error);
}

void UrlToXml::on_importExcelBtn_clicked()
{
    QString excelPath=QFileDialog::getOpenFileName(this,"选择文件",NULL,"*.xlsx");
    readExcelData(excelPath);
    ShowFileInfo();
}

/*void UrlToXml::readExcelData(QString fileName)
{
    repeatFlag=false;//是否有重复，有重复只提示一次
    fileInfoHash.clear();
    Document xlsxDoc(fileName);
    xlsxDoc.selectSheet("Sheet1");
    int rowIndex=2;
    QString cellValue;
    QString cellValue1;
    QVariant var;
    QVariant var1;
    QVariant var2;
    while(1)
    {
        var=xlsxDoc.read(rowIndex,1);
        var1=xlsxDoc.read(rowIndex,2);
        var2=xlsxDoc.read(rowIndex,3);
        if(var.isNull()&&var1.isNull())
            break;
        if(!var.isNull())
        {
            cellValue=var.toString();
        }
        else
        {
            cellValue="";
        }
        if(!var1.isNull())
        {
            cellValue1=var1.toString();
        }
        else
        {
            cellValue1="";
        }
        if(!var2.isNull())
        {
            cellValue1+="="+var2.toString();
        }
        else
        {
            cellValue1+="=";
        }
        if(fileInfoHash.contains(cellValue))
        {
            if(!repeatFlag)
            {
                QMessageBox::information(this,"提示","文件名有重复");
                repeatFlag=true;
            }
        }
        else
            fileInfoHash.insert(cellValue,cellValue1);
        rowIndex++;
    }
}
*/
void UrlToXml::readExcelData(QString fileName)
{
    repeatFlag=false;//是否有重复，有重复只提示一次
    fileInfoList.clear();
    fileNameList.clear();

    if(fileName=="")
        return;

    QAxObject excel("Excel.Application");
    excel.setProperty("Visible", false);
    QAxObject *work_books = excel.querySubObject("WorkBooks");
    work_books->dynamicCall("Open (const QString&)", fileName);

    QAxObject *work_book = excel.querySubObject("ActiveWorkBook");
    QAxObject *work_sheets = work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets
    int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目

    if(sheet_count > 0)
    {
        QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", 1);

        QAxObject *usedRange = work_sheet->querySubObject("UsedRange");
        excelVar = usedRange->dynamicCall("Value");
        delete usedRange;
        QVariantList varRows = excelVar.toList();

        if(varRows.count()<=0)
        {
            QMessageBox::information(this,"提示","Excel为空，请重新选择！");
            return;
        }

        QVariantList var=varRows[0].toList();
        if(ui->fileTableWidget->columnCount()==3)
        {
            if(!(var[0].toString()=="文件名" && var[2].toString()=="描述"))
            {
                QMessageBox::information(this,"提示","Excel格式不正确，请重新选择！");
                return;
            }
        }
        else if(ui->fileTableWidget->columnCount()==4)
        {
            if(!(var[0].toString()=="文件名" && var[3].toString()=="描述"))
            {
                QMessageBox::information(this,"提示","Excel格式不正确，请重新选择！");
                return;
            }
        }

        for(int i=1;i<varRows.size();i++)
        {
            QVariantList var=varRows[i].toList();
            QString cellValue="";
            QString cellValue1="";
            cellValue=var[0].toString();
            if(ui->fileTableWidget->columnCount()==3)
                cellValue1=var[1].toString()+"="+var[2].toString();
            else if(ui->fileTableWidget->columnCount()==4)
                cellValue1=var[1].toString()+"="+var[2].toString()+"="+var[3].toString();
            // if(var[2].toString()!="")
            // cellValue1+="="+var[2].toString();
            if(fileNameList.contains(cellValue))
            {
                if(!repeatFlag)
                {
                    QMessageBox::information(this,"提示","文件名有重复");
                    repeatFlag=true;
                }
            }
            else
            {
                if(cellValue!=""&&cellValue1!="")
                {
                    fileNameList.append(cellValue);
                    filepair.first=cellValue;
                    filepair.second=cellValue1;
                    fileInfoList.append(filepair);
                }
            }
        }
    }
    work_book->dynamicCall("Close()");    // 关闭文件
    excel.dynamicCall("Quit()");    // 退出

    qDebug()<<"done";
}

void UrlToXml::ShowFileInfo()
{
    ui->fileTableWidget->clearContents();
    disconnect(ui->fileTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_fileTableWidget_itemChanged(QTableWidgetItem*)));
    ui->fileTableWidget->setRowCount(fileInfoList.count()+1);
    int num=0;
    for(int m=0;m<fileInfoList.count();m++)
    {
        QStringList valueList=fileInfoList.at(m).second.split("=");
        ui->fileTableWidget->setItem(num,0,new QTableWidgetItem(fileInfoList.at(m).first));
        for(int n=0;n<valueList.count();n++)
        {
            ui->fileTableWidget->setItem(num,n+1,new QTableWidgetItem(valueList[n]));
        }
        num++;
    }
    for(int m=0;m<ui->fileTableWidget->columnCount();m++)
        ui->fileTableWidget->setItem(num,m,new QTableWidgetItem(""));//前两栏的也new个实例

    connect(ui->fileTableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_fileTableWidget_itemChanged(QTableWidgetItem*)));
}

void UrlToXml::changeUnique(QString filePath,QString uniqueStr,QString des, QString nullnote)
{
    qDebug()<<"uniqueStr"<<uniqueStr;
    if(!filePath.isEmpty())
    {
        QFile file(filePath);
        if(!file.exists())
        {
            QMessageBox::warning(NULL,"警告",filePath+" 文件不存在！");
            file.close();
            errorFlag=false;
            return;
        }
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(NULL,"警告",filePath+" 打开错误！");
            errorFlag=false;
            file.close();
            return;
        }
        qDebug()<<"filePath"<<filePath;
        QDomDocument document;
        QString error;
        int row = 0, column = 0;
        if(!document.setContent(&file, false, &error, &row, &column))
        {
            QMessageBox::information(NULL, QString("提示"), QString("解析文件错误，行： ") + QString::number(row, 10) + QString(" ,列： ") + QString::number(column, 10));
            errorFlag=false;
            return ;
        }

        QDomElement docElem = document.documentElement();
        QDomNodeList nodeList = docElem.elementsByTagName("data");
        if (nodeList.count() >0 )
        {
            QDomNode node = nodeList.at(0);
            QDomElement el=node.toElement();
            el.setAttribute("unique",uniqueStr);
        }
        qDebug()<<"uniqueStr"<<uniqueStr;
        QStringList uniquelist=uniqueStr.split(";;");
        for(int i=0;i<uniquelist.count();i++)
        {
            qDebug()<<"uniquelist"<<uniquelist.at(i);
            QStringList uniquetmplist=uniquelist.at(i).split("=");
            qDebug()<<"uniquetmplist.count"<<uniquetmplist.count();
            if(uniquetmplist.count()>1)
            {
                nodeList = docElem.elementsByTagName(uniquetmplist.at(0));
                if (nodeList.count() >0 )
                {
                    QDomNode node = nodeList.at(0);
                    QDomNode oldnode = node.firstChild();
                    node.firstChild().setNodeValue(uniquetmplist.at(1));
                    QDomNode newnode = node.firstChild();
                    node.replaceChild(newnode,oldnode);
                    node.toElement().setAttribute("des",des);
                }
            }
        }
        if(nullnote!="")
        {
            nodeList = docElem.elementsByTagName(nullnote);
            if (nodeList.count() >0 )
            {
                QDomNode node = nodeList.at(0);
                QDomNode oldnode = node.firstChild();
                node.firstChild().setNodeValue("unknown");
                QDomNode newnode = node.firstChild();
                node.replaceChild(newnode,oldnode);
                node.toElement().setAttribute("onlyCheck","N");
            }
        }

        QFile filexml(filePath);
        if(!filexml.exists())
        {
            QMessageBox::warning(NULL,"警告",filePath+" 文件不存在！");
            filexml.close();
            errorFlag=false;
            return;
        }
        if(!filexml.open(QFile::WriteOnly | QFile::Truncate))
        {
            QMessageBox::warning(NULL,"警告",filePath+" 打开错误！");
            errorFlag=false;
            filexml.close();
            return;
        }
        QTextStream ts(&filexml);
        ts.reset();
        ts.setCodec("utf-8");
        document.save(ts, 4, QDomNode::EncodingFromTextStream);
        filexml.close();
    }
}


void UrlToXml::on_xmlPathLineEdit_textChanged(const QString &arg1)
{
    if(ui->xmlPathLineEdit->text()!="")
        connect(ui->xmlPathLineEdit,SIGNAL(editingFinished()),this,SLOT(on_xmlPathLineEdit_editingFinished()));
}

void UrlToXml::on_xmlPathLineEdit_editingFinished()
{
    disconnect(ui->xmlPathLineEdit,SIGNAL(editingFinished()),this,SLOT(on_xmlPathLineEdit_editingFinished()));
    ui->xmlPathLineEdit->selectAll();
    if(!ui->xmlPathLineEdit->text().contains(rightXmlPath))
    {
        QMessageBox::StandardButton bt;
        bt = QMessageBox::question(this, tr("提示"),
                                   QString(tr("所选路径不在工作路径内，是否继续？")),
                                   QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        if(bt==QMessageBox::No)
            ui->xmlPathLineEdit->clear();
        else if(bt==QMessageBox::Yes)
        {

        }
    }
}

