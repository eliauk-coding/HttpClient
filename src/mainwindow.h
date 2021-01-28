#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QNetworkRequest;
class QPushButton;
QT_END_NAMESPACE

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
    QMap<QString, QString> getRequestParams();
    QMap<QByteArray, QByteArray> getRequestHeader();
    QByteArray getRequestBody();

private slots:
    void on_pbt_request_clicked();
    void slot_addBtn_clicked();
    void slot_delBtn_clicked();
private:
    Ui::MainWindow *ui;
    QPushButton *m_addParamBtn;
    QPushButton *m_addHeaderBtn;
    QPushButton *m_addBodyBtn;
};

#endif // MAINWINDOW_H
