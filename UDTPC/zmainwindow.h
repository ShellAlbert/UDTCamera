#ifndef ZMAINWINDOW_H
#define ZMAINWINDOW_H

#include <QWidget>
#include <zimgdisplayer.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>
class ZMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ZMainWindow(QWidget *parent = nullptr);
    ~ZMainWindow();
signals:

public slots:
    void ZSlotConnect();
    void ZSlotMsg(const QString &msg);
private:
    QLabel *m_llIP;
    QComboBox *m_cbIP;
    QToolButton *m_tbConnect;
    QHBoxLayout *m_hLayoutCfg;

    ZImgDisplayer *m_disp1;
    ZImgDisplayer *m_disp2;
    QHBoxLayout *m_hLayout;
    QTextEdit *m_teLog;
    QVBoxLayout *m_vLayout;
};

#endif // ZMAINWINDOW_H
