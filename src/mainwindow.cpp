#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "httpclient.h"

#include <QJsonDocument>
#include <QDateTime>

QString headerInfo = QString("Content-Type -> %1\n\nServer -> %2\n\nDate -> %3\n\nContent-Length -> %4");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_addParamBtn(nullptr),
    m_addHeaderBtn(nullptr),
    m_addBodyBtn(nullptr)
{
    ui->setupUi(this);

    ui->tableWidget_params->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget_params->verticalHeader()->setVisible(false);
    ui->tableWidget_params->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_params->horizontalHeader()->setVisible(false);
    ui->tableWidget_params->setColumnCount(3);
    ui->tableWidget_params->setRowCount(1);

    ui->tableWidget_body->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget_body->verticalHeader()->setVisible(false);
    ui->tableWidget_body->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_body->horizontalHeader()->setVisible(false);
    ui->tableWidget_body->setColumnCount(3);
    ui->tableWidget_body->setRowCount(1);

    ui->tableWidget_Header->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget_Header->verticalHeader()->setVisible(false);
    ui->tableWidget_Header->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_Header->horizontalHeader()->setVisible(false);
    ui->tableWidget_Header->setColumnCount(3);
    ui->tableWidget_Header->setRowCount(2);

    /***************************************************/
    m_addParamBtn = new QPushButton("添加一行", this);
    connect(m_addParamBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);
    ui->tableWidget_params->setCellWidget(0, 0, m_addParamBtn);

    /***************************************************/
    QLineEdit *type = new QLineEdit("Content-Type");
    type->setReadOnly(true);
    ui->tableWidget_Header->setCellWidget(0, 0, type);

    QComboBox *combo = new QComboBox();
    QStringList sList;
    sList << tr("Content-Type: application/xml") << tr("Content-Type: application/json");
    combo->addItems(sList);
    ui->tableWidget_Header->setCellWidget(0, 1, combo);

    m_addHeaderBtn = new QPushButton("添加一行", this);
    ui->tableWidget_Header->setCellWidget(1, 0, m_addHeaderBtn);
    connect(m_addHeaderBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);

    /***************************************************/
    m_addBodyBtn = new QPushButton("添加一行", this);
    connect(m_addBodyBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);
    ui->tableWidget_body->setCellWidget(0, 0, m_addBodyBtn);

}

MainWindow::~MainWindow()
{
    delete ui;
}

QMap<QString, QString> MainWindow::getRequestParams()
{
    QMap<QString, QString> params;
    for(int i = 0; i < ui->tableWidget_params->rowCount() - 1; i++){
        QLineEdit *key = (QLineEdit*)ui->tableWidget_params->cellWidget(i, 0);
        QLineEdit *value = (QLineEdit*)ui->tableWidget_params->cellWidget(i, 1);
        params.insert(key->text(), value->text());
    }
    return params;
}

QMap<QByteArray, QByteArray> MainWindow::getRequestHeader()
{
    QMap<QByteArray, QByteArray> headers;
    //Content-Type
    QLineEdit *key = (QLineEdit*)ui->tableWidget_Header->cellWidget(0, 0);
    QComboBox *value = (QComboBox*)ui->tableWidget_Header->cellWidget(0, 1);
    headers.insert(key->text().toLocal8Bit(), value->currentText().toLocal8Bit());
    //header
    for(int i = 1; i < ui->tableWidget_Header->rowCount() - 1; i++){
        QLineEdit *key = (QLineEdit*)ui->tableWidget_Header->cellWidget(i, 0);
        QLineEdit *value = (QLineEdit*)ui->tableWidget_Header->cellWidget(i, 1);
        headers.insert(key->text().toLocal8Bit(), value->text().toLocal8Bit());
    }
    return headers;
}

QByteArray MainWindow::getRequestBody()
{
    QByteArray data;
    for(int i = 0; i < ui->tableWidget_body->rowCount() - 1; i++){
        QLineEdit *key = (QLineEdit*)ui->tableWidget_body->cellWidget(i, 0);
        QLineEdit *value = (QLineEdit*)ui->tableWidget_body->cellWidget(i, 1);
        if(i != ui->tableWidget_body->rowCount() - 2){
            data.append(QString("%1=%2&").arg(key->text()).arg(value->text()));
        } else {
            data.append(QString("%1=%2").arg(key->text()).arg(value->text()));
        }
    }
    return data;
}

void MainWindow::on_pbt_request_clicked()
{
    try{
        ui->textEdit_header->clear();
        ui->textEdit_body->clear();
        HttpClient client;
        //初始化QSslConfiguration
        client.initSslConfig();

        QString interface = ui->lineEdit_interface->text();

        QNetworkRequest request = client.getNetworkRequest(interface, getRequestHeader(), getRequestParams());

        //发送请求
        Response result = client.request(ui->comboBox_method->currentText(), &request, getRequestBody());
        if(result.statusCode != 200){
            throw QString("错误:%1").arg(result.statusCode);
        }

        //解析并格式化输出
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(result.body, &jsonError);
        if(jsonError.error != QJsonParseError::NoError)
        {
            throw jsonError.errorString();
        }
        ui->textEdit_body->append(QString::fromStdString(doc.toJson().toStdString()));
        //输出头
        ui->textEdit_header->append(headerInfo.arg(result.ContentTypeHeader.toString())
                                               .arg(result.ServerHeader.toString())
                                               .arg(QDateTime::currentDateTime().toString())
                                               .arg(result.ContentLengthHeader.toString()));
    }
    catch(QString &msg){
        ui->statusBar->showMessage(msg);
    }
}

void MainWindow::slot_addBtn_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QTableWidget *tableWidget = nullptr;
    QString keyName, valueName;
    if(btn == m_addBodyBtn){
        tableWidget = ui->tableWidget_body;
        keyName = "输入Body参数 key";
        valueName = "输入Body参数 value，可为空";
    } else if(btn == m_addHeaderBtn){
        tableWidget = ui->tableWidget_Header;
        keyName = "输入header key";
        valueName = "输入header value，可为空";
    } else {
        tableWidget = ui->tableWidget_params;
        keyName = "输入参数 key";
        valueName = "输入参数 value，可为空";
    }

    int rowCount = tableWidget->rowCount();
    tableWidget->setRowCount(rowCount + 1);
    QLineEdit *key = new QLineEdit();
    key->setPlaceholderText(keyName);
    tableWidget->setCellWidget(rowCount - 1, 0, key);

    QLineEdit *value = new QLineEdit();
    value->setPlaceholderText(valueName);
    tableWidget->setCellWidget(rowCount - 1, 1, value);

    QPushButton *delBtn = new QPushButton("删除", tableWidget);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::slot_delBtn_clicked);
    tableWidget->setCellWidget(rowCount - 1, 2, delBtn);

    if(btn == m_addBodyBtn){
        delete m_addBodyBtn;
        m_addBodyBtn = new QPushButton("添加一行", this);
        tableWidget->setCellWidget(rowCount, 0, m_addBodyBtn);
        connect(m_addBodyBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);
    } else if(btn == m_addHeaderBtn){
        delete m_addHeaderBtn;
        m_addHeaderBtn = new QPushButton("添加一行", this);
        tableWidget->setCellWidget(rowCount, 0, m_addHeaderBtn);
        connect(m_addHeaderBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);
    } else {
        delete m_addParamBtn;
        m_addParamBtn = new QPushButton("添加一行", this);
        tableWidget->setCellWidget(rowCount, 0, m_addParamBtn);
        connect(m_addParamBtn, &QPushButton::clicked, this, &MainWindow::slot_addBtn_clicked);
    }
}

void MainWindow::slot_delBtn_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QTableWidget *tableWidget = nullptr;
    if(ui->tabWidget1->currentWidget() == ui->tab_requestBody){
        tableWidget = ui->tableWidget_body;
    } else if(ui->tabWidget1->currentWidget() == ui->tab_requestHeader){
        tableWidget = ui->tableWidget_Header;
    } else {
        tableWidget = ui->tableWidget_params;
    }
    for(int i = 0; i < tableWidget->rowCount(); i++){
        QPushButton *tmpBtn = (QPushButton*)tableWidget->cellWidget(i, 2);
        if(btn == tmpBtn)
            tableWidget->removeRow(i);
    }
}
