#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "=== 应用程序启动 ===";
    
    QApplication app(argc, argv);
    qDebug() << "QApplication 已创建";
    
    // 创建一个简单的窗口
    QWidget *window = new QWidget();
    window->setWindowTitle("测试窗口");
    window->resize(800, 600);
    
    // 设置明显的背景色
    window->setStyleSheet("QWidget { background-color: #ff0000; }");
    qDebug() << "窗口已创建，背景色：红色";
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(window);
    
    // 添加一个大标签
    QLabel *label = new QLabel("Hello Qt WebAssembly!");
    label->setStyleSheet("QLabel { font-size: 48px; color: white; background-color: blue; padding: 20px; }");
    layout->addWidget(label);
    qDebug() << "标签已添加";
    
    // 添加一个按钮
    QPushButton *button = new QPushButton("点击测试");
    button->setStyleSheet("QPushButton { font-size: 24px; padding: 10px; }");
    QObject::connect(button, &QPushButton::clicked, []() {
        qDebug() << "按钮被点击了！";
    });
    layout->addWidget(button);
    qDebug() << "按钮已添加";
    
    window->setLayout(layout);
    
    // 显示窗口
    window->show();
    qDebug() << "窗口已调用 show()";
    qDebug() << "窗口可见性:" << window->isVisible();
    qDebug() << "窗口尺寸:" << window->size();
    
    qDebug() << "=== 进入事件循环 ===";
    return app.exec();
}
