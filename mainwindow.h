#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httprequestworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    HttpRequestWorker *worker;

    void setCheckableButtons(bool status);
    void standarnResponse(QNetworkReply* reply, QString msg = 0);

private slots:

    void on_pb_connect_clicked();
    void on_pb_cleanlog_clicked();
    void on_pb_getcontainers_clicked();

    void on_pb_addcontainer_clicked();
    void on_pb_deletecontainer_clicked();
    void on_pb_getobjects_clicked();

    void on_pb_upload_clicked();
    void on_pb_download_clicked();
    void on_pb_delete_clicked();

    void requestGetContainers          (QString token, QString url);
    void requestAddContainer           (QString token, QString url);
    void requestGetContainerObjects    (QString token, QString url);
    void requestDeleteContainer        (QString token, QString url);
    void requestUploadObject           (QString token, QString url);
    void requestDownloadObject         (QString token, QString url);
    void requestDeleteObject           (QString token, QString url);

    void responseAuthentication(QNetworkReply* reply);
    void responseGetContainers(QNetworkReply* reply);
    void responseAddContainer(QNetworkReply* reply);
    void responseGetContainerObjects(QNetworkReply* reply);
    void responseDeleteContainer(QNetworkReply* reply);
    void responseUploadObject(QNetworkReply* reply);
    void responseDownloadObject(QNetworkReply* reply);
    void responseDeleteObject(QNetworkReply* reply);

signals:
    void ready_to_work (QString token, QString url);
    void work_finished ();
};

#endif // MAINWINDOW_H
