#ifndef ACERCA_DE_H
#define ACERCA_DE_H

#include <QDialog>

namespace Ui {
class acerca_de;
}

class acerca_de : public QDialog
{
    Q_OBJECT

public:
    explicit acerca_de(QWidget *parent = nullptr);
    ~acerca_de();

    void setVersion(const QString &newVersion);

    int valor() const;

private:
    Ui::acerca_de *ui;
    QString m_version;
    int m_valor;
};

#endif // ACERCA_DE_H
