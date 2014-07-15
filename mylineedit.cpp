#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Enter)
    {
        emit enterPressed();
        ev->accept();
    } else ev->ignore();
}
