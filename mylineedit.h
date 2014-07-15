#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget* parent = 0);

signals:
    void enterPressed();

public slots:
protected:
    void keyPressEvent(QKeyEvent* ev);

};

#endif // MYLINEEDIT_H
