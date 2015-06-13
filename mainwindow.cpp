#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    worker = new HttpRequestWorker(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///PUSHBUTTON_SLOTS
void MainWindow::on_pb_connect_clicked()
{
    setCheckableButtons(false);

    QString url_str = ui->le_url->text();
    HttpRequestInput input(url_str, "GET");
    input.add_fea ("X-Auth-User", ui->te_username->toPlainText());
    input.add_fea ("X-Auth-Key",  ui->te_password->toPlainText());

    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
            this,   SLOT(responseAuthentication(QNetworkReply*)));
    worker->execute(&input);
}
void MainWindow::on_pb_addcontainer_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestAddContainer(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_getcontainers_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestGetContainers(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_deletecontainer_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestDeleteContainer(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_getobjects_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestGetContainerObjects(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_upload_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestUploadObject(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_download_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestDownloadObject(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_delete_clicked()
{
    connect(this, SIGNAL(ready_to_work(QString, QString)), this, SLOT(requestDeleteObject(QString,QString)));
    emit ui->pb_connect->clicked();
}
void MainWindow::on_pb_cleanlog_clicked()
{
    ui->tb_log->setText("");
}

///SPECIAL
void MainWindow::setCheckableButtons(bool status)
{
    ui->pb_addcontainer->setCheckable(status);
    ui->pb_connect->setCheckable(status);
    ui->pb_delete->setCheckable(status);
    ui->pb_download->setCheckable(status);
    ui->pb_upload->setCheckable(status);
}
void MainWindow::standarnResponse(QNetworkReply* reply, QString msg)
{
    if (msg == "")
    {
        if (reply->error() == QNetworkReply::NoError)
            msg = "Success!";
        else
            msg = "Error STD_RESP: " + reply->errorString() + "\n\n";
    }
    QMessageBox::information(this, "", msg);

    QString headResponse;
    QList<QByteArray> headerList = reply->rawHeaderList();
    foreach(QByteArray head, headerList)
    {
        headResponse += (head + ": " + reply->rawHeader(head) + "\n");
    }
    ui->tb_log->append(headResponse+"\n");

    QByteArray bytes = reply->readAll();
    ui->tb_log->append("Size of reply body (in bytes):" + QString::number(bytes.size()) + "\n");

    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    ui->tb_log->append(str);

    disconnect(worker, SIGNAL(on_execution_finished(QNetworkReply*)), 0, 0);
    setCheckableButtons(true);
}

///REQUESTS
void MainWindow::requestGetContainers(QString token, QString url)
{
/*
GET /{api version}/{account} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseGetContainers(QNetworkReply*)));
    HttpRequestInput input(url, "GET");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}
void MainWindow::requestAddContainer(QString token, QString url)
{
/*
PUT /{api version}/{account}/{container} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
X-Container-Read: {comma-separated-uids}
X-Container-Write: {comma-separated-uids}
X-Container-Meta-{key}: {value}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseAddContainer(QNetworkReply*)));
    url+=("/"+ui->te_containername->toPlainText());
    HttpRequestInput input(url, "PUT");
    input.add_fea ("X-Auth-Token", token);
//    input.add_fea ("X-Container-Read", "");
//    input.add_fea ("X-Container-Write", "");
//    input.add_fea ("X-Container-Meta-Test", "Test");
    worker->execute(&input);
}
void MainWindow::requestGetContainerObjects(QString token, QString url)
{
/*
GET /{api version}/{container} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseGetContainerObjects(QNetworkReply*)));
    HttpRequestInput input(url, "GET");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}
void MainWindow::requestDeleteContainer(QString token, QString url)
{
/*
DELETE /{api version}/{account}/{container} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDeleteContainer(QNetworkReply*)));
    HttpRequestInput input(url, "DELETE");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}
void MainWindow::requestUploadObject(QString token, QString url)
{
/*
PUT /{api version}/{account}/{container}/{object} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseUploadObject(QNetworkReply*)));
    HttpRequestInput input(url, "PUT");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}
void MainWindow::requestDownloadObject(QString token, QString url)
{
/*
GET /{api version}/{account}/{container}/{object} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDownloadObject(QNetworkReply*)));
    HttpRequestInput input(url, "GET");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}
void MainWindow::requestDeleteObject(QString token, QString url)
{
/*
DELETE /{api version}/{account}/{container}/{object} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDeleteObject(QNetworkReply*)));
    HttpRequestInput input(url, "DELETE");
    input.add_fea ("X-Auth-Token", token);
    worker->execute(&input);
}

///RESPONSES
void MainWindow::responseAuthentication(QNetworkReply *reply)
{
    QString headResponse;
    QString url;
    QString token;

    QString msg;
    if (reply->error() == QNetworkReply::NoError)
        msg = "Success Authentication!";
    else
    {
        msg = "Error Authentication: " + reply->errorString() + "\n";
        disconnect(this, SIGNAL(ready_to_work(QString,QString)), 0, 0);
    }

    QMessageBox::information(this, "", msg);

    QList<QByteArray> headerList = reply->rawHeaderList();
    foreach(QByteArray head, headerList)
    {
        headResponse += (head + ": " + reply->rawHeader(head) + "\n");
        if (head == "X-Auth-Token")
            token = reply->rawHeader(head);
        else if (head == "X-Storage-Url")
            url = reply->rawHeader(head);
    }
    ui->tb_log->append(headResponse+"\n");

    QByteArray bytes = reply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    ui->tb_log->append(str);

    disconnect(worker, SIGNAL(on_execution_finished(QNetworkReply*)), 0, 0);
    emit ready_to_work(token, url);
    disconnect(this, SIGNAL(ready_to_work(QString,QString)), 0, 0);
}
void MainWindow::responseGetContainers(QNetworkReply* reply)
{
    QString msg;
    QByteArray bytes = reply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    ui->tb_log->append("Size of reply body (in bytes):" + QString::number(bytes.size()) + "\n");
    ui->tb_log->append(str);
    QStringList test = str.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    ui->lv_containers->clear();
    ui->lv_containers->addItems(test);

    QVariant code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (reply->error() == QNetworkReply::NoError){
        msg = "Success GetContainers! ";
        if (code == QNetworkReply::AuthenticationRequiredError){
            msg += "No content! ";
        }
        else{
            msg += "Have content! ";
        }
    }
    else{
        msg = "Error GetContainers: " + reply->errorString() + "\n\n";
    }

    standarnResponse(reply, msg);
}
void MainWindow::responseAddContainer(QNetworkReply* reply)
{
    standarnResponse(reply);
}
void MainWindow::responseGetContainerObjects(QNetworkReply* reply)
{
    standarnResponse(reply);
}
void MainWindow::responseDeleteContainer(QNetworkReply* reply)
{
    standarnResponse(reply);
}
void MainWindow::responseUploadObject(QNetworkReply* reply)
{
    standarnResponse(reply);
}
void MainWindow::responseDownloadObject(QNetworkReply* reply)
{
    standarnResponse(reply);
}
void MainWindow::responseDeleteObject(QNetworkReply* reply)
{
    standarnResponse(reply);
}
