#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    // std::unique_ptr<flexiv::middleware2::FastRTPSNode> subNode;
    // flexiv::middleware2::FastRTPSNode test_sub_node =
    // flexiv::middleware2::FastRTPSNode("my_subscriber_new");
    // std::shared_ptr<flexiv::middleware2::FastRTPSSubscriber<test_msgs::msg::KostalLeverPubSubType>>
    // subscriber; subNode =
    // std::make_unique<flexiv::middleware2::FastRTPSNode>("my_subscriber");

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
