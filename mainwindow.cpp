#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QMimeType>
#include <QMimeDatabase>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    worker = new HttpRequestWorker(this);
    setEnabledButtons(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///PUSHBUTTON_SLOTS
void MainWindow::on_pb_connect_clicked()
{
    setEnabledButtons(false);

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
void MainWindow::on_pb_reloadobj_clicked()
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
void MainWindow::setEnabledButtons(bool status)
{
    ui->pb_connect->setEnabled(status);
    ui->pb_addcontainer->setEnabled(status);
    ui->pb_getcontainers->setEnabled(status);

    if(ui->lv_containers->count() && ui->lv_containers->currentRow() != -1) {
        ui->pb_deletecontainer->setEnabled(status);
        ui->pb_reloadobj->setEnabled(status);
        ui->pb_upload->setEnabled(status);
    } else {
        ui->pb_deletecontainer->setEnabled(false);
        ui->pb_reloadobj->setEnabled(false);
        ui->pb_upload->setEnabled(false);
    }

    if(ui->lv_objects->count() && ui->lv_objects->currentRow() != -1) {
        ui->pb_download->setEnabled(status);
        ui->pb_delete->setEnabled(status);
    } else {
        ui->pb_download->setEnabled(false);
        ui->pb_delete->setEnabled(false);
    }

    ui->pb_cleanlog->setEnabled(status);
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
    //QMessageBox::information(this, "", msg);

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

    ui->tb_log->append(msg);

    disconnect(worker, SIGNAL(on_execution_finished(QNetworkReply*)), 0, 0);
    setEnabledButtons(true);
}

///REQUESTS
void MainWindow::requestGetContainers(QString token, QString url)
{
/*
GET /{api version}/{account} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    setEnabledButtons(false);
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
    setEnabledButtons(false);
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
    setEnabledButtons(false);
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseGetContainerObjects(QNetworkReply*)));
    QListWidgetItem* item = ui->lv_containers->currentItem();
    url.append("/" + item->text());
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
    setEnabledButtons(false);
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDeleteContainer(QNetworkReply*)));
    QListWidgetItem* item = ui->lv_containers->currentItem();
    url.append("/" + item->text());
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
    setEnabledButtons(false);
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseUploadObject(QNetworkReply*)));

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".");
    QFileInfo fileInfo(fileName);
    QMimeDatabase db;
    QMimeType fileType = db.mimeTypeForFile(fileInfo);

    QListWidgetItem* container = ui->lv_containers->currentItem();
    url.append("/" + container->text() + "/" + fileInfo.fileName());

    HttpRequestInput input(url, "PUT");
    input.add_fea ("X-Auth-Token", token);
    input.add_file(fileInfo.fileName(), fileInfo.filePath(), NULL, fileType.name());
    worker->execute(&input);
}
void MainWindow::requestDownloadObject(QString token, QString url)
{
/*
GET /{api version}/{account}/{container}/{object} HTTP/1.1
Host: {fqdn}
X-Auth-Token: {auth-token}
*/
    setEnabledButtons(false);
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDownloadObject(QNetworkReply*)));
    QListWidgetItem* container = ui->lv_containers->currentItem();
    QListWidgetItem* object = ui->lv_objects->currentItem();
    url.append("/" + container->text() + "/" + object->text());
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
    setEnabledButtons(false);
    connect(worker, SIGNAL(on_execution_finished(QNetworkReply*)),
                    this,   SLOT(responseDeleteObject(QNetworkReply*)));
    QListWidgetItem* container = ui->lv_containers->currentItem();
    QListWidgetItem* object = ui->lv_objects->currentItem();
    url.append("/" + container->text() + "/" + object->text());
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
    ui->tb_log->append(msg);

    setEnabledButtons(true);
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
        if (code == QNetworkReply::AuthenticationRequiredError){
            msg += "Not Success!";
        }
        else{
            msg += "Success GetContainers!";
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
    emit ui->pb_getcontainers->clicked();
}
void MainWindow::responseGetContainerObjects(QNetworkReply* reply)
{
    QByteArray bytes = reply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    QStringList test = str.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    ui->lv_objects->clear();
    ui->lv_objects->addItems(test);

    standarnResponse(reply);
}
void MainWindow::responseDeleteContainer(QNetworkReply* reply)
{
    standarnResponse(reply);
    emit ui->pb_getcontainers->clicked();
}
void MainWindow::responseUploadObject(QNetworkReply* reply)
{
    standarnResponse(reply);
    emit ui->pb_reloadobj->clicked();
}
void MainWindow::responseDownloadObject(QNetworkReply* reply)
{
    QByteArray bytes = reply->readAll();
    QFile file(ui->lv_objects->currentItem()->text());
    QString msg;
    if(file.open(QIODevice::WriteOnly))
    {
        int index = 0;
        unsigned occur = 0;
        while ((index = bytes.indexOf('\n', index)) >= 0){
            ++occur;
            if (occur == 5){
                bytes.remove(0, index + 1);
                break;
            }
        }
        bytes.remove(bytes.lastIndexOf('\n'), bytes.size());
        file.write(bytes);
        file.close();
        msg.append("Success!\n");
    }
    else
        msg.append("Error: " + file.errorString() + "\n");

    standarnResponse(reply, msg);
}
void MainWindow::responseDeleteObject(QNetworkReply* reply)
{
    standarnResponse(reply);
    emit ui->pb_reloadobj->clicked();
}

///LIST_WIDGETS_SLOTS
void MainWindow::on_lv_containers_itemSelectionChanged()
{
    setEnabledButtons(ui->pb_connect->isEnabled());
    emit on_pb_reloadobj_clicked();
}
void MainWindow::on_lv_objects_itemSelectionChanged()
{
    setEnabledButtons(ui->pb_connect->isEnabled());
}
