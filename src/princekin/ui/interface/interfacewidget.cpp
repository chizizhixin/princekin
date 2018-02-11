#include "interfacewidget.h"
#include "ui_interfacewidget.h"

InterfaceWidget::InterfaceWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InterfaceWidget)
{
    ui->setupUi(this);
    setWindowTitle("接口测试");

    ui->filterAfterLineEdit->setEnabled(false);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    createToolBar();
    ui->paramTable->verticalHeader()->setVisible(false);

    ui->interDataTable->verticalHeader()->setVisible(false);
    ui->interDataTable->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->interDataTable->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->interDataTable->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    ui->interDataTable->scrollToBottom();

    QHeaderView *headerGoods = ui->interDataTable->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    headerGoods->setSortIndicator(0, Qt::AscendingOrder);
    headerGoods->setSortIndicatorShown(true);

    connect(headerGoods, SIGNAL(sectionClicked(int)), this, SLOT (TableHeaderClicked(int)));

    ui->splitter_2->setStretchFactor(0,5);
    ui->splitter_2->setStretchFactor(1,3);
    ui->splitter_5->setStretchFactor(0,3);
    ui->splitter_5->setStretchFactor(1,8);

    ui->equipListView->setStyleSheet("QListView{font-size:12px;background:#ffffff;border:1px #ffffff;}"
                                     "QListView::item:selected:active{background:#e4e4e4;color:#000000;padding-left:8px;}"
                                     "QListView::item:selected{background:#e4e4e4;color:#000000;padding-left:8px;}"
                                     "QListView::item{height:30px;color:#4d4d4d}");

    ui->packageListView->setStyleSheet("QListView{font-size:12px;background:#ffffff;border:1px #ffffff;}"
                                       "QListView::item:selected:active{background:#e4e4e4;color:#000000;padding-left:8px;}"
                                       "QListView::item:selected{background:#e4e4e4;color:#000000;padding-left:8px;}"
                                       "QListView::item{height:30px;color:#4d4d4d}");

    ui->equipListView->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                          "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->packageListView->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                            "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->packageListView->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {border:0px solid grey;width: 10px;}"
                                                              "QScrollBar::handle:horizontal {background: grey;border: 3px solid grey;border-radius:5px;}");
    ui->interDataTable->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                           "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->interDataTable->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {border:0px solid grey;width: 10px;}"
                                                             "QScrollBar::handle:horizontal {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->baseDataTextEdit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                             "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->paramTable->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                       "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");
    ui->responseTextEdit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:0px solid grey;width: 10px;}"
                                                             "QScrollBar::handle:vertical {background: grey;border: 3px solid grey;border-radius:5px;min-height: 20px;}");

    equipModel = new QStringListModel(equipList);
    packageModel = new QStringListModel(packageList);

    setupDialog=new InterfaceSetup();

    pLogcat=new QProcess(this);
    connect(pLogcat,SIGNAL(readyReadStandardOutput()),this,SLOT(RecieveLogcatOutput()));

    connect(setupDialog,SIGNAL(SendData(QVariant)),this,SLOT(RecieveData(QVariant)));
    connect(settingButton,SIGNAL(clicked(bool)),this,SLOT(SetupBtnClicked()));
    connect(this,SIGNAL(sendStateChange(bool)),setupDialog,SLOT(stateChanged(bool)));
    connect(this,SIGNAL(sendSettingSig(bool,bool,bool,bool,bool,bool,QString,QString,QString,QString,QString,QString)),setupDialog,SLOT(recieveSettingSig(bool,bool,bool,bool,bool,bool,QString,QString,QString,QString,QString,QString)));
}

void InterfaceWidget::TableHeaderClicked(int num)
{
    if(num==0)
    {
        orderFlag="id";
        ui->interDataTable->sortByColumn(num,Qt::AscendingOrder);
    }
    else if(num==6)
    {
        orderFlag="time";
        ui->interDataTable->sortByColumn(num,Qt::DescendingOrder);
    }
}

void InterfaceWidget::SetupBtnClicked()
{
    setupDialog->setAttribute(Qt::WA_QuitOnClose,false);
    setupDialog->show();
    setupDialog->raise();
    setupDialog->move ((QApplication::desktop()->width() - setupDialog->width())/2,(QApplication::desktop()->height() - setupDialog->height())/2);
}

void InterfaceWidget::createToolBar()
{
    toolBar=addToolBar("setting");
    toolBar->setStyleSheet("QToolBar{background:#ffffff;}");
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacer->setStyleSheet("QWidget{background:#ffffff;}");
    toolBar->addWidget(spacer);

    settingButton=new QToolButton(this);
    settingButton->setToolTip("设置");
    settingButton->setIcon(QIcon(":/setting.png"));
    settingButton->setFixedWidth(50);
    settingButton->setStyleSheet("QToolButton{background:#ffffff;}");
    toolBar->addWidget(settingButton);
}

void InterfaceWidget::resizeEvent(QResizeEvent *event)
{
    ui->interDataTable->setColumnWidth(0,40);
    ui->interDataTable->setColumnWidth(1,40);
    ui->interDataTable->setColumnWidth(2,65);
    ui->interDataTable->setColumnWidth(4,100);
    ui->interDataTable->setColumnWidth(5,60);
    ui->interDataTable->setColumnWidth(6,40);
    ui->interDataTable->setColumnWidth(3,ui->interDataTable->width()-360);

    ui->paramTable->setColumnWidth(0,60);
    ui->paramTable->setColumnWidth(1,ui->paramTable->width()-75);
}

InterfaceWidget::~InterfaceWidget()
{
    delete ui;
}

void InterfaceWidget::showInit()
{
    ui->interDataTable->setColumnWidth(0,40);
    ui->interDataTable->setColumnWidth(1,40);
    ui->interDataTable->setColumnWidth(2,65);
    ui->interDataTable->setColumnWidth(4,100);
    ui->interDataTable->setColumnWidth(5,40);
    ui->interDataTable->setColumnWidth(6,40);
    ui->interDataTable->setColumnWidth(3,ui->interDataTable->width()-340);

    ui->paramTable->setColumnWidth(0,60);
    ui->paramTable->setColumnWidth(1,ui->paramTable->width()-75);

    isMemCheck=gisMemCheck;
    isCpuCheck=gisCpuCheck;
    isBatteryCheck=gisBatteryCheck;
    isCpuTempCheck=gisCpuTempCheck;
    isWifiCheck=gisWifiCheck;
    isMobileCheck=gisMobileCheck;

    memThres=gMemThres;
    cpuThres=gCpuThres;
    batteryThres=gBatteryThres;
    cpuTempThres=gCpuTempThres;
    wifiThres=gWifiThres;
    mobileThres=gMobileThres;

    emit sendSettingSig(isMemCheck,isCpuCheck,isBatteryCheck,isCpuTempCheck,isWifiCheck,isMobileCheck,memThres,cpuThres,batteryThres,cpuTempThres,wifiThres,mobileThres);
}

void InterfaceWidget::on_startBtn_clicked()
{
    if(!isStartFlag)
    {
        if(deviceName=="")
        {
            QMessageBox::information(this,tr("提示"),tr("请选择一个设备"));
            return;
        }
        if(packageName=="")
        {
            QMessageBox::information(this,tr("提示"),tr("请选择一个应用"));
            return;
        }
        if(ui->packageLineEdit->text().trimmed()!=ui->packageListView->currentIndex().data()&&ui->packageListView->currentIndex().row()!=-1)//如果输入框内的内容是一半，则开始测试后把后一半补全
        {
            //这里如果不去掉链接的话，会导致槽函数的运行，那么packageName等其他的还会再变
            disconnect(ui->packageLineEdit,SIGNAL(textChanged(QString)),this,SLOT(on_packageLineEdit_textChanged(QString)));
            ui->packageLineEdit->setText(ui->packageListView->currentIndex().data().toString());
            connect(ui->packageLineEdit,SIGNAL(textChanged(QString)),this,SLOT(on_packageLineEdit_textChanged(QString)));
        }

        //*****************20170717*****************//
        qOldPackageName=Helper::getFirstLine(gConfigDir + QDir::separator() + "packageName.txt");
        gOldPackageName=qOldPackageName;
        bool b=Helper::isPackageName1(packageName);
        if(!b)
        {
            return;
        }
        //*****************20170717*****************//

        ui->startBtn->setIcon(QIcon(":/stop.png"));
        isStartFlag=true;
        isStartLogat=false;

        ui->filterAfterLineEdit->setEnabled(false);
        ui->timeWarningLineEdit->setEnabled(false);

        QString cmdStrLogcat="adb -s "+deviceName+" shell logcat -c\n";//开始运行之前清空logcat数据
        ExeCmd::runCmd(cmdStrLogcat);

        QStringList tempList=ParseXml::readConfigXmlForIP_Port(gConfigDir + QDir::separator() + "config.xml");
        QString IP=tempList[0];
        QString port=tempList[1];

        //*****************20170717*****************//
        if(packageName=="")
        {
            qLunchTime="手机";
        }
        else
        {
            if(packageName!=qOldPackageName && packageName!="")
            {
                qLunchTime="";
                qFrames="";
            }
            else
            {
                getLunchTime();
            }
        }
        //*****************20170717*****************//

        QString cmdStr="mitmdump -b "+IP+" -p "+port+" -s "+gConfigDir+QDir::separator()+"getresponsedata.py";
        BehaviorWorker *worker=new BehaviorWorker;
        worker->cmdStr=cmdStr;
        QThread *workerThread=new QThread(this);
        connect(workerThread,SIGNAL(started()),worker,SLOT(startStat()));
        connect(worker,SIGNAL(statStop()),workerThread,SLOT(quit()));
        connect(workerThread,SIGNAL(finished()),worker,SLOT(deleteLater()));
        connect(worker,SIGNAL(statStop()),this,SLOT(RecieveStopSignal()));

        connect(worker,SIGNAL(readOutputFromWorker(QString)),this,SLOT(ReadStandardOutput(QString)));
        connect(worker,SIGNAL(readErrorFromWorker(QString)),this,SLOT(ReadStandardError(QString)));
        connect(this,SIGNAL(sendStopStatSignal()),worker,SLOT(recieveStopStatSignal()));

        worker->moveToThread(workerThread);
        workerThread->start();

        emit sendStateChange(true);//开始之后，设置中的内存温度等选项失效

        if(isCpuTempCheck||isBatteryCheck||isMemCheck||isCpuCheck||isWifiCheck||isMobileCheck)
        {
            assist=new AssistMeasure();
            assist->deviceModel=ExeCmd::GetDeviceModel( deviceName )+" + "+deviceName;
            assist->packageName=this->packageName;

            assist->isCpuTempCheck=this->isCpuTempCheck;
            assist->isBatteryCheck=this->isBatteryCheck;
            assist->isMemCheck=this->isMemCheck;
            assist->isCpuCheck=this->isCpuCheck;
            assist->isWifiCheck=this->isWifiCheck;
            assist->isMobileCheck=this->isMobileCheck;

            assist->memThres=this->memThres;
            assist->cpuThres=this->cpuThres;
            assist->batteryThres=this->batteryThres;
            assist->cpuTempThres=this->cpuTempThres;
            assist->wifiThres=this->wifiThres;
            assist->mobileThres=this->mobileThres;

            connect(assist,SIGNAL(sendMemOverThresSignal(bool,QString)),this,SLOT(RecieveMemOverThresSignal(bool,QString)));
            connect(assist,SIGNAL(sendCpuOverThresSignal(bool,QString)),this,SLOT(RecieveCpuOverThresSignal(bool,QString)));
            connect(assist,SIGNAL(sendBatteryOverThresSignal(bool,QString)),this,SLOT(RecieveBatteryOverThresSignal(bool,QString)));
            connect(assist,SIGNAL(sendCpuTempOverThresSignal(bool,QString)),this,SLOT(RecieveCpuTempOverThresSignal(bool,QString)));
            connect(assist,SIGNAL(sendWifiOverThresSignal(bool,QString)),this,SLOT(RecieveWifiOverThresSignal(bool,QString)));
            connect(assist,SIGNAL(sendMobileOverThresSignal(bool,QString)),this,SLOT(RecieveMobileOverThresSignal(bool,QString)));

            assist->StartMeasure();
        }

        gInterfaceStartTime=Helper::getTime2("yyyy-MM-dd_hh:mm:ss");
        qStartTime=Helper::getTime2("yyyyMMdd_hhmmss");
        fileNameForReport=gWorkSpace+QDir::separator() + "report" + QDir::separator()+ "Interface_" + deviceName + "_" + qStartTime;
        Helper::createPath(fileNameForReport);

        qReportTxtPath=fileNameForReport + QDir::separator() + "report_"+  qStartTime +".txt";
        qErrorReportPath=fileNameForReport + QDir::separator() + "InterfaceErrorReport_"+  qStartTime +".txt";

        QString errorFileName=qErrorReportPath;

        //建立logcat文件
        if(isMemCheck||isCpuCheck||isBatteryCheck||isCpuTempCheck||isWifiCheck||isMobileCheck)
        {
            fileLogcat=new QFile(errorFileName);
            if ( !fileLogcat->exists())
                fileLogcat->open( QIODevice::WriteOnly );
        }

        getXXX();
    }
    else
    {
        emit sendStopStatSignal();
    }
}

void InterfaceWidget::RecieveStopSignal()
{
    StopRun();
}

void InterfaceWidget::StopRun()
{
    isStartFlag=false;
    ui->startBtn->setIcon(QIcon(":/start.png"));
    ui->filterAfterLineEdit->setEnabled(true);
    ui->timeWarningLineEdit->setEnabled(true);

    //计算超时率
    interfaceTotalNum=interHash.count();
    if(timeWarning==0)
        timeOutRate=0;
    else
        timeOutRate=timeOutRate/interfaceTotalNum*100;

    qDebug()<<"interfaceTotalNum "<<interfaceTotalNum;
    qDebug()<<"timeOutRate "<<timeOutRate;
    qDebug()<<"aveReponseTime"<<aveReponseTime;
    qDebug()<<"timewarning "<<timeWarning;
    qDebug()<<"mintime "<<minTime;
    qDebug()<<"maxtime "<<maxTime;

    gInterfaceTotalNum=interfaceTotalNum;
    gInterfaceAveReponseTime=aveReponseTime;
    gInterfaceMinTime=minTime;
    gInterfaceMaxTime=maxTime;
    gInterfaceTimeWarning=timeWarning;
    gInterfaceTimeOutRate=timeOutRate;

    ExeCmd::runCmd("taskkill /im mitmdump.exe /f");

    emit sendStateChange(false);

    if(isCpuTempCheck||isBatteryCheck||isMemCheck||isCpuCheck||isWifiCheck||isMobileCheck)
    {
        if(assist!=NULL)
        {
            disconnect(assist,SIGNAL(sendMemOverThresSignal(bool,QString)),this,SLOT(RecieveMemOverThresSignal(bool,QString)));
            disconnect(assist,SIGNAL(sendCpuOverThresSignal(bool,QString)),this,SLOT(RecieveCpuOverThresSignal(bool,QString)));
            disconnect(assist,SIGNAL(sendBatteryOverThresSignal(bool,QString)),this,SLOT(RecieveBatteryOverThresSignal(bool,QString)));
            disconnect(assist,SIGNAL(sendCpuTempOverThresSignal(bool,QString)),this,SLOT(RecieveCpuTempOverThresSignal(bool,QString)));
            disconnect(assist,SIGNAL(sendWifiOverThresSignal(bool,QString)),this,SLOT(RecieveWifiOverThresSignal(bool,QString)));
            disconnect(assist,SIGNAL(sendMobileOverThresSignal(bool,QString)),this,SLOT(RecieveMobileOverThresSignal(bool,QString)));
            assist->StopMeasure();

            createExcel();

        }
    }
    else
    {
        createBaseExcel();
    }

    ExeCmd::StopLogcat(deviceName);
    if(fileLogcat!=NULL)
    {
        fileLogcat->close();
        if(fileLogcat->size()==0)
            QFile::remove(fileLogcat->fileName());
    }

    exportReport();
}

void InterfaceWidget::closeEvent(QCloseEvent *e)
{
    if(isStartFlag)//已经开始的状态，点击停止
    {
        emit sendStopStatSignal();
    }
}

void InterfaceWidget::RecieveData(QVariant var)
{
    signalInterfaceData_s signalInterfaceData;
    signalInterfaceData=var.value<signalInterfaceData_s>();


    this->isMemCheck=signalInterfaceData.isMemCheck;
    this->isCpuCheck=signalInterfaceData.isCpuCheck;
    this->isBatteryCheck=signalInterfaceData.isBatteryCheck;
    this->isCpuTempCheck=signalInterfaceData.isCpuTempCheck;
    this->isWifiCheck=signalInterfaceData.isWifiCheck;
    this->isMobileCheck=signalInterfaceData.isMobileCheck;

    this->memThres=signalInterfaceData.memThres;
    this->cpuThres=signalInterfaceData.cpuThres;
    this->batteryThres=signalInterfaceData.batteryThres;
    this->cpuTempThres=signalInterfaceData.cpuTempThres;
    this->wifiThres=signalInterfaceData.wifiThres;
    this->mobileThres=signalInterfaceData.mobileThres;

    this->isAllTest=signalInterfaceData.isAllTest;
    this->method=signalInterfaceData.method;
    this->filterStr=signalInterfaceData.filterStr;
    qDebug()<<isAllTest;
    qDebug()<<method;
    qDebug()<<filterStr;

}

void InterfaceWidget::exportReport()
{
    if(gWorkSpace=="")
        gWorkSpace=gNewDisk + QDir::separator() + "princekinWorkspace";

    fileNameForReport=gWorkSpace+QDir::separator()+ "report" + QDir::separator()+ "Interface_" + deviceName + "_" + qStartTime;
    Helper::createPath(fileNameForReport);

}

void InterfaceWidget::ReadStandardOutput(QString res)
{
    res=UrlDecode(res);
    if(res.contains("-----START-----")&&res.contains("-----END-----"))
    {
        res=res.left(res.length()-15);
        ContentRes=res.mid(15);
        qDebug()<<"~~"<<ContentRes;
        endflag=false;
    }
    if(res.contains("<< ") && (!endflag))
    {
        qDebug()<<res;
        QStringList tempList=res.split(" ",QString::SkipEmptyParts);
        QString contentLength=tempList.at(tempList.length()-1);
        endflag=true;
        QStringList data=ContentRes.split("-&&-");
        qDebug()<<data.length();
        if(data.length()<8)
            return;
        interDataClass tempData;
        tempData.ID=interNum;
        tempData.url=UrlDecode( data[0] );
        tempData.httpVersion=data[1];
        tempData.method=data[2];
        tempData.time=data[3].left(data[3].indexOf(".")).toInt();
        if(data[4].contains("XXX"))
            tempData.contentType="";
        else
        {
            if(data[4].contains(";"))
                tempData.contentType=data[4].left(data[4].indexOf(";"));
            else
                tempData.contentType=data[4];
        }
        if(tempData.method=="POST")
            tempData.requestParam=data[5];
        else
        {
            int num =tempData.url.indexOf("?");
            if(num!=-1)
            {
                tempData.requestParam=tempData.url.mid(num+1);
            }
        }
        tempData.contentLength=contentLength;
        tempData.statusCode=data[6];
        tempData.response=data[7];

        bool flag=JudgeFilterRes(tempData);
        if(flag)
        {
            if(tempData.time>timeWarning)//计算超时率
                timeOutRate++;
            aveReponseTime=(aveReponseTime*interHash.count()+tempData.time)/(interHash.count()+1);//计算平均响应时间

            interHash.insert(interNum++,tempData);
            if(interHash.count()==1)
            {
                minTime=tempData.time;
                maxTime=tempData.time;
            }
            if(tempData.time>maxTime)
                maxTime=tempData.time;
            if(tempData.time<minTime)
                minTime=tempData.time;

            for(int i=0;i<data.length();i++)
            {
                qDebug()<<data[i];
            }

            if(isLatest)
            {
                ui->baseDataTextEdit->setText("Url: "+tempData.url);
                ui->baseDataTextEdit->append("Status: "+tempData.httpVersion+ " "+tempData.statusCode);
                ui->baseDataTextEdit->append("Time: "+QString::number( tempData.time ));
                ui->baseDataTextEdit->append("Content-Type: "+tempData.contentType);
                if(tempData.contentLength!="")
                    ui->baseDataTextEdit->append("Content-Length: "+tempData.contentLength);
                else
                    ui->baseDataTextEdit->append("Content-Length: ");
                if(tempData.contentType.contains("json")||tempData.response.startsWith("{"))
                {
                    ui->responseTextEdit->setText( formatJson( tempData.response ) );
                }
                else if(tempData.contentType.contains("xml")||tempData.response.startsWith("<"))
                {
                    ui->responseTextEdit->setText(formatXml( tempData.response ));
                }
                else
                    ui->responseTextEdit->setText(tempData.response);

                SplitInterfaceParam(tempData.requestParam);

                ui->interDataTable->scrollToBottom();
            }
            ShowInterfaceData(tempData);
        }
    }

}

void InterfaceWidget::ReadStandardError(QString res)
{
    qDebug()<<"error: "<<res;
    //QMessageBox::information(this,"提示",res);
}

bool InterfaceWidget::JudgeFilterRes(interDataClass tempData)//设置中的字符串过滤
{
    bool flagStr=false;
    bool flagMethod=false;
    if(isAllTest)
    {
        flagMethod=true;
        flagStr=true;
    }
    else
    {
        if(filterStr.trimmed()!="")
        {
            QStringList list=filterStr.split(";");
            for(int m=0;m<list.count();m++)
            {
                if(tempData.url.contains(list[m]))
                {
                    flagStr=true;
                    break;
                }
            }
        }
        else
            flagStr=true;
        if(method=="ALL")
            flagMethod=true;
        else
        {
            if(tempData.method==method)
                flagMethod=true;
            else
                flagMethod=false;
        }
    }
    qDebug()<<flagMethod;
    qDebug()<<flagStr;
    return flagMethod&&flagStr;
}

void InterfaceWidget::RecieveDevicesSigal(QStringList devicesList)
{
    if(numForDeviceSignal<3)
        numForDeviceSignal++;
    QString currentDevice = ui->equipListView->currentIndex().data().toString();
    equipList.clear();
    equipList=devicesList;
    equipModel->setStringList(equipList);
    ui->equipListView->setModel(equipModel);

    int row=equipList.indexOf(currentDevice);
    if(row==-1)
    {
        //没有选中设备时，去掉第一项设备名称的显示，直接显示“第三方应用”
        packageModel->setStringList( QStringList());
        ui->packageListView->setModel(packageModel);
        deviceName="";//当前没有选中的设备时，设备名字清空，否则在点击设备时会出问题；
    }

    QModelIndex index=equipModel->index(row);
    if(equipList.contains(currentDevice))
    {
        ui->equipListView->setCurrentIndex(index);
    }
    if(numForDeviceSignal==1)
    {
        index=equipModel->index(0);
        ui->equipListView->setCurrentIndex(index);
        on_equipListView_clicked(index);
    }
}

void InterfaceWidget::on_equipListView_clicked(const QModelIndex &index)
{
    if((!ui->equipListView->currentIndex().data().toString().contains(deviceName))||deviceName=="")//如果没有换设备，不对当前界面操作
    {
        QModelIndex indexEquip=ui->equipListView->currentIndex();
        deviceName=ExeCmd::getDeviceSN(indexEquip.data().toString());
        qDebug()<<deviceName;
        if(deviceName!="")
        {
            addPackagesList();
            on_packageLineEdit_textChanged("");
        }
        else
        {
            //没有选中设备时，去掉第一项设备名称的显示，直接显示“第三方应用”
            packageModel->setStringList( QStringList());
            ui->packageListView->setModel(packageModel);
        }
    }
}

void InterfaceWidget::addPackagesList()
{
    packageList.clear();

    QString cmdStrThirdPackages="adb -s "+deviceName+" shell pm list packages -3\n";
    QProcess p(0);
    p.setReadChannelMode(QProcess::MergedChannels);
    p.start(cmdStrThirdPackages);
    p.waitForStarted();
    while(p.waitForFinished()==false)
    {
    }

    while(p.canReadLine())
    {
        QString mStr=p.readLine();
        if(mStr.contains("package:"))
        {
            QStringList mSplitResult=mStr.split("package:");
            packageList.append(mSplitResult.at(1).trimmed());
        }
    }

    p.close();
    qSort(packageList.begin(),packageList.end());
    packageModel->setStringList(packageList);
    ui->packageListView->setModel(packageModel);
}

void InterfaceWidget::ShowInterfaceData(interDataClass tempData)
{
    int rowCount;

    if(orderFlag=="time")
    {
        if(ui->interDataTable->rowCount()==0||interHash[ui->interDataTable->item(ui->interDataTable->rowCount()-1,0)->text().toInt()].time>tempData.time)
        {
            rowCount=ui->interDataTable->rowCount();
            ui->interDataTable->setRowCount(rowCount+1);
        }
        else if(interHash[ui->interDataTable->item(0,0)->text().toInt()].time<tempData.time)
        {
            rowCount=0;
            ui->interDataTable->insertRow(rowCount);
        }
        else if(ui->interDataTable->rowCount()!=0&&interHash[ui->interDataTable->item(ui->interDataTable->rowCount()-1,0)->text().toInt()].time<tempData.time)
        {
            for(int i=0;i<ui->interDataTable->rowCount()-1;i++)
            {
                if((interHash[ui->interDataTable->item(i,0)->text().toInt()].time>=tempData.time)&&(interHash[ui->interDataTable->item(i+1,0)->text().toInt()].time<tempData.time))
                {
                    rowCount=i+1;
                    ui->interDataTable->insertRow(rowCount);
                    break;
                }
            }
        }
        else
        {
            rowCount=ui->interDataTable->rowCount();
            ui->interDataTable->setRowCount(rowCount+1);
        }
    }
    else if(orderFlag=="id")
    {
        rowCount=ui->interDataTable->rowCount();
        ui->interDataTable->setRowCount(rowCount+1);
    }

    if(tempData.time>timeWarning&&ui->timeWarningLineEdit->text().trimmed()!="")//超过警戒值就都设置成红色
    {
        QTableWidgetItem *item=new QTableWidgetItem;
        item->setData(0,tempData.ID);
        item->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,0,item);

        QTableWidgetItem *itemMethod=new QTableWidgetItem;
        itemMethod->setData(0,tempData.method);
        itemMethod->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,1,itemMethod);

        QTableWidgetItem *itemHttpversion=new QTableWidgetItem;
        itemHttpversion->setData(0,tempData.httpVersion);
        itemHttpversion->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,2,itemHttpversion);

        QTableWidgetItem *itemUrl=new QTableWidgetItem;
        itemUrl->setData(0,tempData.url);
        itemUrl->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,3,itemUrl);

        QTableWidgetItem *itemContentType=new QTableWidgetItem;
        itemContentType->setData(0,tempData.contentType);
        itemContentType->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,4,itemContentType);

        QTableWidgetItem *itemContentLength=new QTableWidgetItem;
        itemContentLength->setData(0,tempData.contentLength);
        itemContentLength->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,5,itemContentLength);

        QTableWidgetItem *itemTime=new QTableWidgetItem;
        itemTime->setData(0,tempData.time);
        itemTime->setTextColor(QColor(Qt::red));
        ui->interDataTable->setItem(rowCount,6,itemTime);
    }
    else
    {
        QTableWidgetItem *item=new QTableWidgetItem;
        item->setData(0,tempData.ID);
        ui->interDataTable->setItem(rowCount,0,item);
        ui->interDataTable->setItem(rowCount,1,new QTableWidgetItem(tempData.method));
        ui->interDataTable->setItem(rowCount,2,new QTableWidgetItem(tempData.httpVersion));
        ui->interDataTable->setItem(rowCount,3,new QTableWidgetItem(tempData.url));
        ui->interDataTable->setItem(rowCount,4,new QTableWidgetItem(tempData.contentType));
        ui->interDataTable->setItem(rowCount,5,new QTableWidgetItem(tempData.contentLength));
        QTableWidgetItem *itemTime=new QTableWidgetItem;
        itemTime->setData(0,tempData.time);
        ui->interDataTable->setItem(rowCount,6,itemTime);
    }
    ui->interDataTable->selectRow(rowCount);
}

QString InterfaceWidget::UrlDecode(QString str)
{
    if(str.isEmpty())//两种方法都可以对url解码
    {
        return "";
    }
    QByteArray byteArrayGB2312 = str.toUtf8();
    QByteArray byteArrayPercentEncoded = QByteArray::fromPercentEncoding(byteArrayGB2312);
    return QString(byteArrayPercentEncoded);
}
void InterfaceWidget::SplitInterfaceParam(QString url)
{
    ui->paramTable->clearContents();
    if(url=="")
    {
        ui->paramTable->setRowCount(0);
        return;
    }
    if(url.endsWith("&"))
        url=url.left(url.length()-1);
    url=UrlDecode(url);
    QStringList paramList=url.split("&");
    // if(paramList.length()>0)
    ui->paramTable->setRowCount(paramList.length());
    for(int i=0;i<paramList.length();i++)
    {
        QStringList tmpList=paramList.at(i).split("=");
        if(tmpList.length()<=1)
        {
            continue;
        }
        ui->paramTable->setItem(i,0,new QTableWidgetItem(tmpList.at(0)));
        ui->paramTable->setItem(i,1,new QTableWidgetItem(tmpList.at(1)));
    }
}

void InterfaceWidget::on_interDataTable_itemClicked(QTableWidgetItem *item)
{
    interDataClass tempData=interHash[ui->interDataTable->item(item->row(),0)->text().toInt()];
    ui->baseDataTextEdit->setText("Url: "+tempData.url);
    ui->baseDataTextEdit->append("Status: "+tempData.httpVersion+ " "+tempData.statusCode);
    ui->baseDataTextEdit->append("Time: "+QString::number( tempData.time ));
    ui->baseDataTextEdit->append("Content-Type: "+tempData.contentType);
    if(tempData.contentLength!="")
        ui->baseDataTextEdit->append("Content-Length: "+tempData.contentLength);
    else
        ui->baseDataTextEdit->append("Content-Length: ");
    if(tempData.contentType.contains("json")||tempData.response.startsWith("{"))
    {
        QString res=formatJson( tempData.response);
        ui->responseTextEdit->setText(res);
    }
    else if(tempData.contentType.contains("xml")||tempData.response.startsWith("<"))
    {
        ui->responseTextEdit->setText(formatXml( tempData.response ));
    }
    else
        ui->responseTextEdit->setText(tempData.response);
    SplitInterfaceParam(tempData.requestParam);

    if(item->row()<ui->interDataTable->rowCount()-1)
        isLatest=false;
    else
        isLatest=true;
}

void InterfaceWidget::on_equipBtn_clicked()
{
    equipList.clear();
    equipModel = new QStringListModel(equipList);

    QString cmdLine="adb devices";
    QProcess p(0);
    p.setReadChannelMode(QProcess::MergedChannels);
    p.start(cmdLine);
    p.waitForStarted();
    while(p.waitForFinished()==false)
    {
    }

    QString mStr;
    QStringList mSplitResult;

    while(p.canReadLine())
    {
        mStr=p.readLine();
        if(mStr.contains("device") && !mStr.contains("devices"))
        {
            mSplitResult=mStr.split("device");
            if(!mSplitResult.at(0).contains("error"))
            {
                equipList.append(ExeCmd::GetDeviceModel(mSplitResult.at(0).trimmed())+" + "+mSplitResult.at(0).trimmed());
                equipModel->setStringList(equipList);
            }
        }
    }
    p.close();

    ui->equipListView->setModel(equipModel);
    deviceName="";//点击设备按钮后，还没选择设备，此时情况设备名

    QModelIndex indexEquip=ui->equipListView->model()->index(0,0);
    ui->equipListView->setCurrentIndex(indexEquip);

    deviceName=ExeCmd::getDeviceSN(ui->equipListView->currentIndex().data().toString());
    qDebug()<<deviceName;
    if(deviceName!="")
    {
        addPackagesList();
        on_packageLineEdit_textChanged("");
    }
}

void InterfaceWidget::on_packageListView_clicked(const QModelIndex &index)
{
    QModelIndex indexPackage=ui->packageListView->currentIndex();
    packageName=indexPackage.data().toString();
    ui->packageLineEdit->setText(packageName);
}

void InterfaceWidget::RecieveMemOverThresSignal(bool flag,QString deviceName)
{
    isMemOverThres=flag;
}

void InterfaceWidget::RecieveCpuOverThresSignal(bool flag,QString deviceName)
{
    isCpuOverThres=flag;
}

void InterfaceWidget::RecieveWifiOverThresSignal(bool flag,QString deviceName)
{
    isWifiOverThres=flag;
}

void InterfaceWidget::RecieveMobileOverThresSignal(bool flag,QString deviceName)
{
    isMobileOverThres=flag;
}

void InterfaceWidget::RecieveBatteryOverThresSignal(bool flag,QString deviceName)
{
    isBatteryOverThres=flag;
}

void InterfaceWidget::RecieveCpuTempOverThresSignal(bool flag,QString deviceName)
{
    isCpuTempOverThres=flag;
    if(isBatteryOverThres||isCpuTempOverThres||isMemOverThres||isCpuOverThres||isWifiOverThres||isMobileOverThres)
    {
        qDebug()<<"超过阀值了";

        if(!isStartLogat)//但凡有超过阈值的现象就开始抓logcat，直至测试停止
        {
            QString cmdStr="adb -s "+deviceName+" shell logcat -v time -b main -b system\n";
            qDebug()<<cmdStr;
            if(pLogcat->state())
                pLogcat->write(cmdStr.toLatin1());
            else
                pLogcat->start("cmd", QStringList()<<"/c"<< cmdStr );
        }
        isStartLogat=true;
    }
}

void InterfaceWidget::RecieveLogcatOutput()
{
    QString tempStr=QString::fromUtf8(pLogcat->readAllStandardOutput());
    if(fileLogcat->isOpen())
        fileLogcat->write(tempStr.toStdString().c_str());
}

void InterfaceWidget::on_clearBtn_clicked()
{
    ui->interDataTable->clearContents();
    ui->interDataTable->setRowCount(0);
    ui->paramTable->clearContents();
    ui->paramTable->setRowCount(0);
    ui->baseDataTextEdit->setText("");
    ui->responseTextEdit->setText("");

    timeOutRate=0;
    timeWarning=0;
    ui->timeWarningLineEdit->setText("");
    interfaceTotalNum=0;
    aveReponseTime=0;
    minTime=0;
    maxTime=0;

    interHash.clear();
    interNum=1;

    ui->filterAfterLineEdit->setEnabled(false);
}

void InterfaceWidget::on_reportDetailBtn_clicked()
{
    ExeCmd::openFolder(fileNameForReport);
}

QString InterfaceWidget::formatJson(QString json)
{
    QString result ;

    int length = json.length();
    int number = 0;
    QChar key = 0;

    //遍历输入字符串。
    for (int i = 0; i < length; i++)
    {
        //1、获取当前字符。
        //key = json.charAt(i);
        key=json.at(i);

        //2、如果当前字符是前方括号、前花括号做如下处理：
        if((key == '[') || (key == '{') )
        {
            //（1）如果前面还有字符，并且字符为“：”，打印：换行和缩进字符字符串。
            if((i - 1 > 0) && (json.at(i - 1) == ':'))
            {
                result.append('\n');
                result.append(indent(number));
            }

            //（2）打印：当前字符。
            result.append(key);

            //（3）前方括号、前花括号，的后面必须换行。打印：换行。
            result.append('\n');

            //（4）每出现一次前方括号、前花括号；缩进次数增加一次。打印：新行缩进。
            number++;
            result.append(indent(number));

            //（5）进行下一次循环。
            continue;
        }

        //3、如果当前字符是后方括号、后花括号做如下处理：
        if((key == ']') || (key == '}') )
        {
            //（1）后方括号、后花括号，的前面必须换行。打印：换行。
            result.append('\n');

            //（2）每出现一次后方括号、后花括号；缩进次数减少一次。打印：缩进。
            number--;
            result.append(indent(number));

            //（3）打印：当前字符。
            result.append(key);

            //（4）如果当前字符后面还有字符，并且字符不为“，”，打印：换行。
            if(((i + 1) < length) && (json.at(i + 1) != ','))
            {
                result.append('\n');
            }

            //（5）继续下一次循环。
            continue;
        }

        //4、如果当前字符是逗号。逗号后面换行，并缩进，不改变缩进次数。
        if((key == ','))
        {
            result.append(key);
            result.append('\n');
            result.append(indent(number));
            continue;
        }

        //5、打印：当前字符。
        result.append(key);
    }

    return result;
}


QString InterfaceWidget::indent(int number)
{
    QString result ;
    for(int i = 0; i < number; i++)
    {
        result.append(SPACE);
    }
    return result;
}

QString InterfaceWidget::formatXml(QString xml)
{
    QString result ;

    int number = 0;
    QChar key = 0;
    QChar key1 = 0;
    bool specialStatus=false;//<data><data2>状态

    QString head="";
    int num=xml.indexOf("?>");
    if(num!=-1)
    {
        head=xml.left(num+2);
        xml=xml.mid(num+2);

        result.append(head);
    }

    int length = xml.length();
    for(int i=0;i<length-1;i++)
    {
        key=xml.at(i);
        key1=xml.at(i+1);
        if(key=='<'&&key1!=47)
        {
            if(specialStatus)
                number++;
            result.append("\n");
            result.append(indent(number));
            result.append(key);
            for(++i;i<length;i++)
            {
                key=xml.at(i);
                result.append(key);
                if(key=='>')//找到一个完整的<data>
                    break;
            }
            specialStatus=true;
            continue;
        }
        if(key=='<'&&key1==47)
        {
            if(xml.at(i-1)=='>')
            {
                number--;
                result.append("\n");
                result.append(indent(number));
            }
            result.append(key);
            for(++i;i<length;i++)
            {
                key=xml.at(i);
                result.append(key);
                if(key=='>')//找到一个完整的<data>
                    break;
            }
            specialStatus=false;
            continue;
        }

        result.append(key);
        specialStatus=false;
    }

    return result;
}

void InterfaceWidget::on_timeWarningLineEdit_textChanged(const QString &arg1)
{
    timeWarning=ui->timeWarningLineEdit->text().toInt();

    ui->interDataTable->clearContents();
    ui->interDataTable->setRowCount(0);
    timeOutRate=0;//超时率先清零，然后重新计算
    for(int i=1;i<=interHash.count();i++)
    {
        ShowInterfaceData(interHash[i]);
        //计算超时率
        if(interHash[i].time>timeWarning)
            timeOutRate++;
    }
}

void InterfaceWidget::on_filterAfterLineEdit_textChanged(const QString &arg1)
{
    ui->interDataTable->clearContents();
    ui->interDataTable->setRowCount(0);

    filterStrAfter=ui->filterAfterLineEdit->text();
    if(filterStrAfter.trimmed()=="")
    {
        for(int i=1;i<=interHash.count();i++)
        {
            ShowInterfaceData(interHash[i]);
        }
        return;
    }
    QStringList filterList=filterStrAfter.split(";");
    for(int i=1;i<=interHash.count();i++)
    {

        bool flag=false;
        for(int j=0;j<filterList.count();j++)
        {
            // qDebug()<<filterList.count();
            if(interHash[i].url.contains(filterList[j]))
            {
                flag=true;
                break;
            }
        }
        if(flag)
        {
            ShowInterfaceData(interHash[i]);
        }
    }
}




void InterfaceWidget::createExcel()
{
    createInterfaceExcel();
    ExcelController::Controller *controller=new ExcelController::Controller;

    controller->setMark("interface");
    controller->setXlsxSaveDir(fileNameForReport);
    controller->setStartTime(qStartTime);
    controller->setDeviceId(deviceName);
    controller->setLunchTime(qLunchTime);
    controller->setFrames(qFrames);

    controller->setMemWarningValue(memThres);
    controller->setCpuWarningValue(cpuThres);
    controller->setCpuTempWarningValue(cpuTempThres);
    controller->setBatteryTempWarningValue(batteryThres);
    controller->setWifiWarningValue(wifiThres);
    controller->setMobileWarningValue(mobileThres);

    controller->setPackageName(packageName);
    controller->setMeasureVector(assist->reportInfo);

    connect(controller,SIGNAL(sendWorkerFinished()),this,SLOT(receiveWorkerFinished()));
    controller->doController();
}

void InterfaceWidget::createBaseExcel()
{
    createInterfaceExcel();
    ExcelController::Controller *controller=new ExcelController::Controller;

    controller->setMark("base-interface");
    controller->setXlsxSaveDir(fileNameForReport);
    controller->setStartTime(qStartTime);
    controller->setDeviceId(deviceName);
    controller->setLunchTime(qLunchTime);
    controller->setFrames(qFrames);

    controller->setMemWarningValue(memThres);
    controller->setCpuWarningValue(cpuThres);
    controller->setCpuTempWarningValue(cpuTempThres);
    controller->setBatteryTempWarningValue(batteryThres);
    controller->setWifiWarningValue(wifiThres);
    controller->setMobileWarningValue(mobileThres);

    controller->setPackageName(packageName);

    connect(controller,SIGNAL(sendWorkerFinished()),this,SLOT(receiveWorkerFinished()));
    controller->doController();
}

void InterfaceWidget::receiveWorkerFinished()
{
    QMessageBox::information(NULL,"提示","excel生成完毕");
}

void InterfaceWidget::createInterfaceExcel()
{
    int url_id;
    QString url_method;
    QString url_protocol;
    QString url_web;
    QString url_type;
    QString url_length;
    int url_time;

    QString sourceXlsx=gConfigDir + QDir::separator() + "interface_report.xlsx";
    QString qTargetXlsx=fileNameForReport + QDir::separator() + "report_" + qStartTime + ".xlsx";
    QFile::copy(sourceXlsx,qTargetXlsx);

    Format format=getNormalFormat();

    Document xlsxDoc(qTargetXlsx);
    xlsxDoc.selectSheet("接口信息");

    for(int i=1;i<=interHash.count();i++)
    {
        url_id=interHash.value(i).ID;
        url_method=interHash.value(i).method;
        url_protocol=interHash.value(i).httpVersion;
        url_web=interHash.value(i).url;
        url_type=interHash.value(i).contentType;
        url_length=interHash.value(i).contentLength;
        url_time=interHash.value(i).time;

        xlsxDoc.write("A"+QString::number(i+1),QString::number(url_id),format);
        xlsxDoc.write("B"+QString::number(i+1),url_method,format);
        xlsxDoc.write("C"+QString::number(i+1),url_protocol,format);
        xlsxDoc.write("D"+QString::number(i+1),url_web,format);
        xlsxDoc.write("E"+QString::number(i+1),url_type,format);
        xlsxDoc.write("F"+QString::number(i+1),url_length,format);
        xlsxDoc.write("G"+QString::number(i+1),QString::number(url_time),format);
    }
    xlsxDoc.setColumnWidth(4,100);
    xlsxDoc.save();

}

Format InterfaceWidget::getNormalFormat()
{
    Format format;
    format.setHorizontalAlignment(Format::AlignHCenter);
    format.setVerticalAlignment(Format::AlignVCenter);
    format.setBorderStyle(Format::BorderThin);
    return format;
}

void InterfaceWidget::getXXX()
{
    getFrames();
}


void InterfaceWidget::getFrames()
{
    if(packageName=="")
    {
        qFrames="0";
    }
    else
    {
        QString cmdLine="adb -s " + deviceName + " shell dumpsys gfxinfo " + packageName;
        QThreadPool *pool=new QThreadPool;
        using FramesController::Controller;
        Controller *controllerInstance=new Controller;
        controllerInstance->setAutoDelete(true);
        controllerInstance->setDeviceId(deviceName);
        controllerInstance->setCmdLine(cmdLine);
        connect(controllerInstance,SIGNAL( sendResult(const QString &,const QString &) ),this,SLOT(receiveFramesResult(const QString &,const QString &) )  ,Qt::DirectConnection  );
        pool->start(controllerInstance);
    }
}

void InterfaceWidget::receiveFramesResult(const QString &arg_deviceId,const QString &arg_text)
{
    qFrames=arg_text;
}


void InterfaceWidget::getLunchTime()
{
    QString cmdLine="cmd /c adb -s " + deviceName + " shell dumpsys activity top | findstr ACTIVITY";

    AdbShellController::Controller *controller=new AdbShellController::Controller;
    controller->setMark("getAll","isActive");
    controller->setCmdLine(cmdLine);
    connect(controller,SIGNAL(sendWorkerResult(const QString&,const QString&)),this,SLOT(receiveWorkerResult1(const QString&,const QString&)));
    controller->doController();
}

void InterfaceWidget::receiveWorkerResult1(const QString&arg_str,const QString&arg_mark)
{
    if(arg_mark=="isActive")
    {
        if(arg_str.contains(packageName))
        {
        }
        else
        {
            QString mFile=gConfigDir + QDir::separator() + "lunchActivity.txt";
            QString mLunchActivity=Helper::getPackageName(mFile);
            QString mCmdLine="adb -s " + deviceName + " shell am start -W -n " + mLunchActivity;
            AdbShellController::Controller *controller=new AdbShellController::Controller;
            controller->setMark("getAll","lunchTime");
            controller->setCmdLine(mCmdLine);
            connect(controller,SIGNAL(sendWorkerResult(const QString&,const QString&)),this,SLOT(receiveWorkerResult2(const QString&,const QString&)));
            controller->doController();
        }
    }
}

void InterfaceWidget::receiveWorkerResult2(const QString&arg_str,const QString&arg_mark)
{
    if(arg_mark=="lunchTime")
    {
        qLunchTime=arg_str;
        bool ok;
        int dec=qLunchTime.toInt(&ok, 10);
        float floattime=dec*0.001;
        qLunchTime=QString::number(floattime, 'f', 2);
    }
}

void InterfaceWidget::on_interDataTable_itemChanged(QTableWidgetItem *item)
{
    if(ui->interDataTable->rowCount()>0&&(!isStartFlag))
        ui->filterAfterLineEdit->setEnabled(true);
    else
        ui->filterAfterLineEdit->setEnabled(false);
}

void InterfaceWidget::on_packageLineEdit_textChanged(const QString &arg1)
{
    QString text=ui->packageLineEdit->text();
    bool isFind=false;
    for(int i=0;i<packageList.count();i++)
    {
        if(packageList.at(i).startsWith(text))
        {
            QModelIndex index=ui->packageListView->model()->index(i,0);
            ui->packageListView->setCurrentIndex(index);
            packageName=ui->packageListView->currentIndex().data().toString();//有匹配项时，包名直接等于匹配项
            isFind=true;
            break;
        }
    }
    if(!isFind)//没有匹配项时清除选项
    {
        QModelIndex index=ui->packageListView->model()->index(-1,0);
        ui->packageListView->setCurrentIndex(index);
        packageName=ui->packageLineEdit->text();
    }
}
