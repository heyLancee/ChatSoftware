#include "registdlg.h"

#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

#include "ui_registdlg.h"

RegistDlg::RegistDlg(QWidget *parent) : QDialog(parent), ui(new Ui::RegistDlg) {
  ui->setupUi(this);
  Init();
}

RegistDlg::~RegistDlg() { delete ui; }

void RegistDlg::mousePressEvent(QMouseEvent *event) {
  mouseWindowTopLeft = event->pos();
}

void RegistDlg::mouseMoveEvent(QMouseEvent *event) {
  // 窗口移动
  if (event->buttons() & Qt::LeftButton) {
    mouseDeskTopLeft = event->globalPos();
    windowDeskTopLeft = mouseDeskTopLeft - mouseWindowTopLeft;  // 矢量计算
    this->move(windowDeskTopLeft);  // 移动到目的地
  }
}

void RegistDlg::registSlot(QString string) {
  json js = js.parse(string.toStdString());

  QString mess = "注册";
  if (js["errno"] != 0) {
    mess.append("失败, 错误码: ");
    mess.append(QString::number(js["errno"].get<int>()));
    m_status = false;
  } else {
    userInfo.m_account = js["id"].get<int>();
    mess.append("成功, 账户为: ");
    mess.append(QString::number(js["id"].get<int>()));
    m_status = true;
  }

  QMessageBox::information(
      NULL, "注册", mess, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  return accept();  // Closes the dialog and emits the accepted() signal.
}

void RegistDlg::Init() {
  this->setWindowTitle("注册");
  memset(&userInfo, '\0', sizeof(userInfo));
  m_status = false;
  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);  // 最小化按钮
  setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint);  // 帮助按钮

  int width = this->width() - 10;
  int height = this->height() - 10;
  ui->centerWidget->setGeometry(5, 5, width, height);
  ui->centerWidget->setStyleSheet(
      "QWidget{border-radius:4px;background:rgba(255,255,255,1);}");  // 设置圆角

  this->setWindowFlags(Qt::FramelessWindowHint);  // 去掉标题栏无边框
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  // 实例阴影shadow
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  // 设置阴影距离
  shadow->setOffset(0, 0);
  // 设置阴影颜色
  shadow->setColor(QColor(39, 40, 43, 100));
  // 设置阴影圆角
  shadow->setBlurRadius(10);
  // 给嵌套QWidget设置阴影
  ui->centerWidget->setGraphicsEffect(shadow);

  // 设置信号槽绑定函数
  connect(&NetSignal::getInstance(), &NetSignal::signalRegist, this,
          &RegistDlg::registSlot);
}

void RegistDlg::on_pushBtn_regist_clicked() {
  memset(&userInfo, 0, sizeof(userInfo));
  userInfo.m_userName += ui->lineEdit_username->text().toStdString();
  userInfo.m_password += ui->lineEdit_password->text().toStdString();

  json registInfo;
  registInfo["msgid"] = 2;
  registInfo["name"] = userInfo.m_userName;
  registInfo["password"] = userInfo.m_password;
  // 发送注册消息给服务器
  if (client.connection() == nullptr) {
    QMessageBox::information(NULL, "注册", "连接服务器失败",
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes);
    return;
  }
  client.connection()->send(registInfo.dump());
}

void RegistDlg::on_pushBtn_hide_clicked() {
  QWidget *pWindow = this->window();
  if (pWindow->isTopLevel()) pWindow->showMinimized();
}

void RegistDlg::on_pushBtn_close_clicked() { this->close(); }
