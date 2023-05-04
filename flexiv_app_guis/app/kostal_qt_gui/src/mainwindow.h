#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <flexiv/middleware2/fast_rtps_node.h>
#include "KostalLever.h"
#include "KostalLeverPubSubTypes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void passNode(flexiv::middleware2::FastRTPSNode* nodeIn,
        flexiv::middleware2::FastRTPSNode* nodeOut);
    void passSubscriber(std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<
                            test_msgs::msg::KostalLeverPubSubType>>
                            subIn,
        std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<
            test_msgs::msg::KostalLeverPubSubType>>
            subOut);

signals:
    void signal_start(QString str);
    void signal_stop(QString str);
    void signal_exit();

private slots:
    void on_pushButton_start_clicked();
    void slot_start_func(QString str);

    void on_pushButton_stop_clicked();
    void slot_stop_func(QString str);

    void on_pushButton_exit_clicked();
    void slot_exit_func();

private:
    Ui::MainWindow* ui;
    flexiv::middleware2::FastRTPSNode subNode
        = flexiv::middleware2::FastRTPSNode("my_subscriber");
    // flexiv::middleware2::FastRTPSSubscriber<test_msgs::msg::KostalLeverPubSubType>
    // test_subscriber;
    std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<
        test_msgs::msg::KostalLeverPubSubType>>
        subscriber;
};
#endif // MAINWINDOW_H
