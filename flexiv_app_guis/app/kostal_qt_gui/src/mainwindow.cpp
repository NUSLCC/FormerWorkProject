// QT associated headers
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "statuslight.h"
#include <QtDebug>
#include <QStatusBar>
#include <QLabel>

// Pub-Sub associated headers
const std::string qtWindowTitle = "Kostal Flexiv System v1.0";
const std::string topicKostal = "topic_kostal";
test_msgs::msg::KostalLever sub_msg;

void Foo()
{
    std::cout << "Sub" << std::endl;
    auto tcpPose_array = sub_msg.tcp_pose();
    for (auto it = tcpPose_array.begin(); it != tcpPose_array.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromStdString(qtWindowTitle));
    QStatusBar* statusbar = new QStatusBar();
    setStatusBar(statusbar);
    QLabel* statuslabel = new QLabel("Hint", this);
    statusbar->addWidget(statuslabel);
    connect(
        this, &MainWindow::signal_start, this, &MainWindow::slot_start_func);
    connect(this, &MainWindow::signal_stop, this, &MainWindow::slot_stop_func);
    connect(this, &MainWindow::signal_exit, this, &MainWindow::slot_exit_func);
    setStatusLight(ui->label_system_status, 1, 16); // change light's status
    subscriber
        = subNode
              .CreateDefaultSubscription<test_msgs::msg::KostalLeverPubSubType>(
                  topicKostal, &Foo, (void*)&sub_msg);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::passNode(flexiv::middleware2::FastRTPSNode* nodeIn,
    flexiv::middleware2::FastRTPSNode* nodeOut)
{
    nodeOut = nodeIn;
}

void MainWindow::passSubscriber(
    std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<
        test_msgs::msg::KostalLeverPubSubType>>
        subIn,
    std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<
        test_msgs::msg::KostalLeverPubSubType>>
        subOut)
{
    subIn = subOut;
}

void MainWindow::on_pushButton_start_clicked()
{
    emit this->signal_start("Start is pressed");
}

void MainWindow::on_pushButton_stop_clicked()
{
    std::cout << "Stop" << std::endl;
    emit this->signal_stop("Stop is pressed");
}

void MainWindow::on_pushButton_exit_clicked()
{
    std::cout << "Exit" << std::endl;
    emit this->signal_exit();
}

void MainWindow::slot_start_func(QString str)
{
    // qDebug()<<"The start slot text is: "<<str<<"\n";
    // std::unique_ptr<flexiv::middleware2::FastRTPSNode> test_sub_node;
    // std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<test_msgs::msg::KostalLeverPubSubType>>
    // test_subscriber; test_sub_node =
    // std::make_unique<flexiv::middleware2::FastRTPSNode>("my_subscriber");
    // test_subscriber =
    // test_sub_node.CreateDefaultSubscription<test_msgs::msg::KostalLeverPubSubType>(topicKostal,
    // &Foo, (void*)&sub_msg);
    std::cout << "Hit enter to stop" << std::endl;
    std::cin.get();
}

void MainWindow::slot_stop_func(QString str)
{
    // std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<test_msgs::msg::KostalLeverPubSubType>>
    // test_subscriber; test_sub_node =
    // std::make_unique<flexiv::middleware2::FastRTPSNode>("my_subscriber");
    // test_subscriber =
    // test_sub_node.CreateDefaultSubscription<test_msgs::msg::KostalLeverPubSubType>(topic_camPose,
    // &Foo, (void*)&sub_msg);
    qDebug() << "The stop slot text is: " << str << "\n";
    // test_sub_node.StopAll();
}

void MainWindow::slot_exit_func()
{
    qDebug() << "The exit slot is triggered \n";
    QMainWindow::close();
}
